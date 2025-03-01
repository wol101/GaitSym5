/*
 *  FacetedObject.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 13/09/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

// #pragma warning( disable : 4100 )

#include "FacetedObject.h"
#include "DataFile.h"
#include "GSUtil.h"
#include "PGDMath.h"
#include "SimulationWidget.h"
#include "Preferences.h"
#include "pystring.h"

#define TINYPLY_IMPLEMENTATION
#include "tinyply.h"

#include <QFileInfo>
#include <QDir>
#include <QDebug>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <regex>
#include <array>

using namespace std::literals::string_literals;

MeshStore FacetedObject::m_meshStore;

// create object
FacetedObject::FacetedObject()
{
    // qDebug() << QTime::currentTime().toString() << "FacetedObject()";
}

FacetedObject::~FacetedObject()
{
    // qDebug() << QTime::currentTime().toString() << "~FacetedObject m_mesh.use_count() = " << m_mesh.use_count();
    if (m_mesh)
    {
        m_mesh->removeFromParent();
        m_mesh->geometry()->dispose();
        m_mesh->material()->dispose();
    }
}


// utility front end to try and parse different sorts of mesh files
int FacetedObject::ParseMeshFile(const std::string &filename)
{
    std::string lowercase = pystring::lower(filename);
    if (pystring::endswith(lowercase, ".obj"s)) return ParseOBJFile(filename);
    if (pystring::endswith(lowercase, ".ply"s)) return ParsePLYFile(filename);
    return __LINE__;
}

// parse an OBJ file to a FacetedObject
// returns 0 on success
// this tries to be quite fast and not very flexible
// it might be worth rewriting to use std::from_char and std::to_char
int FacetedObject::ParseOBJFile(const std::string &filename)
{
    m_filename = filename;
    MeshStoreObject *meshStoreObject = m_meshStore.getMesh(filename);
    if (meshStoreObject)
    {
        m_vertexList = meshStoreObject->vertexList;
        m_normalList = meshStoreObject->normalList;
        m_colourList = meshStoreObject->colourList;
        m_uvList = meshStoreObject->uvList;
        m_lowerBound[0] = meshStoreObject->lowerBound[0];
        m_lowerBound[1] = meshStoreObject->lowerBound[1];
        m_lowerBound[2] = meshStoreObject->lowerBound[2];
        m_upperBound[0] = meshStoreObject->upperBound[0];
        m_upperBound[1] = meshStoreObject->upperBound[1];
        m_upperBound[2] = meshStoreObject->upperBound[2];
        return 0;
    }

    // read the whole file into memory
    GaitSym::DataFile theFile;
    if (theFile.ReadFile(filename) == true) return __LINE__;
    const char *ptr = theFile.GetRawData();
    const char *endPtr = ptr + theFile.GetSize();

    pgd::Vector3 vertex;
    std::vector<pgd::Vector3> vertexList;
    pgd::Vector3 normal;
    std::vector<pgd::Vector3> normalList;
    pgd::Vector2 uv;
    std::vector<pgd::Vector2> uvList;
    struct Triangle
    {
        size_t vertex[3];
        size_t normal[3];
        size_t uv[3];
        OBJMaterial *material;
    };
    Triangle triangle;
    std::vector<Triangle> triangleList;
    // having checked a few files ball park is 20M file, 100k vertices, 200k faces, 200k normals
    size_t estimated_max_capacity = theFile.GetSize() / 25; // this will give a factor of 4 to 8 leeway
    vertexList.reserve(estimated_max_capacity);
    normalList.reserve(estimated_max_capacity);
    uvList.reserve(estimated_max_capacity);
    triangleList.reserve(estimated_max_capacity);

    enum FaceFormat { unknown, vertex_only, vertex_texture, vertex_normal, vertex_texture_normal };
    FaceFormat faceFormat = unknown;

    std::map<std::string, OBJMaterial> materialMap;
    OBJMaterial *currentMaterial = nullptr;
    std::string line;
    line.reserve(1024);
    while (ptr < endPtr)
    {
        // non content (whitespace)
        if (*ptr < 32)
        {
            while (*ptr < 32 && ptr < endPtr) { ptr++; }
            continue;
        }

        // comments
        if (*ptr == '#')
        {
            ptr++;
            while (*ptr != '\n' && *ptr != '\r' && ptr < endPtr) { ptr++; }
            ptr++;
            continue;
        }

        // materials definitions
        if (*ptr == 'm')
        {
            // first extract the line
            line.clear();
            while (*ptr != '\n' && *ptr != '\r' && ptr < endPtr)
            {
                line.append(1, *ptr);
                ptr++;
            }
            if (pystring::startswith(line, "mtllib "s))
            {
                std::string materialsFile = pystring::os::path::join(pystring::os::path::dirname(filename), line.substr("mtllib "s.size(), std::string::npos));
                if (ParseOBJMaterialFile(materialsFile, &materialMap))
                {
                    qDebug() << "Error reading material file \"" << materialsFile.c_str() << "\"";
                }
            }
            continue;
        }

        // materials use
        if (*ptr == 'u')
        {
            // first extract the line
            line.clear();
            while (*ptr != '\n' && *ptr != '\r' && ptr < endPtr)
            {
                line.append(1, *ptr);
                ptr++;
            }
            if (pystring::startswith(line, "usemtl "s))
            {
                std::string material = pystring::strip(line.substr("usemtl "s.size(), std::string::npos));
                auto it = materialMap.find(material);
                if (it == materialMap.end()) currentMaterial = nullptr;
                else currentMaterial = &it->second;
            }
            continue;
        }

        // vertices and normals and texture coords
        if (*ptr == 'v')
        {
            ptr++;
            if (ptr >= endPtr) continue;
            if (*ptr == ' ')
            {
                vertex.x = GaitSym::GSUtil::fast_a_to_double(ptr, &ptr);
                if (ptr >= endPtr) continue;
                vertex.y = GaitSym::GSUtil::fast_a_to_double(ptr, &ptr);
                if (ptr >= endPtr) continue;
                vertex.z = GaitSym::GSUtil::fast_a_to_double(ptr, &ptr);
                vertexList.push_back(vertex);
                if (vertex.x < m_lowerBound[0]) m_lowerBound[0] = vertex.x;
                if (vertex.y < m_lowerBound[1]) m_lowerBound[1] = vertex.y;
                if (vertex.z < m_lowerBound[2]) m_lowerBound[2] = vertex.z;
                if (vertex.x > m_upperBound[0]) m_upperBound[0] = vertex.x;
                if (vertex.y > m_upperBound[1]) m_upperBound[1] = vertex.y;
                if (vertex.z > m_upperBound[2]) m_upperBound[2] = vertex.z;
                ptr++;
                continue;
            }
            if (*ptr == 'n')
            {
                ptr++;
                if (ptr >= endPtr) continue;
                normal.x = GaitSym::GSUtil::fast_a_to_double(ptr, &ptr);
                if (ptr >= endPtr) continue;
                normal.y = GaitSym::GSUtil::fast_a_to_double(ptr, &ptr);
                if (ptr >= endPtr) continue;
                normal.z = GaitSym::GSUtil::fast_a_to_double(ptr, &ptr);
                normalList.push_back(normal);
                ptr++;
                continue;
            }
            if (*ptr == 't')
            {
                ptr++;
                if (ptr >= endPtr) continue;
                uv.x = GaitSym::GSUtil::fast_a_to_double(ptr, &ptr);
                if (ptr >= endPtr) continue;
                uv.y = GaitSym::GSUtil::fast_a_to_double(ptr, &ptr);
                uvList.push_back(uv);
                ptr++;
                continue;
            }
            // not recognised so skip to the end of line
            while (*ptr != '\n' && *ptr != '\r' && ptr < endPtr) { ptr++; }
            ptr++;
            continue;
        }

        // faces (only triangular faces supported)
        if (*ptr == 'f')
        {
            ptr++;
            while (faceFormat == unknown)
            {
                const std::regex vertex_only_tester("[ \t]*[0-9]+[ \t]+[0-9]+[ \t]+[0-9]+[ \t]*"s);
                const std::regex vertex_texture_tester("[ \t]*[0-9]+/[0-9]+[ \t]+[0-9]+/[0-9]+[ \t]+[0-9]+/[0-9]+[ \t]*"s);
                const std::regex vertex_normal_tester("[ \t]*[0-9]+//[0-9]+[ \t]+[0-9]+//[0-9]+[ \t]+[0-9]+//[0-9]+[ \t]*"s);
                const std::regex vertex_texture_normal_tester("[ \t]*[0-9]+/[0-9]+/[0-9]+[ \t]+[0-9]+/[0-9]+/[0-9]+[ \t]+[0-9]+/[0-9]+/[0-9]+[ \t]*"s);
                const char *localEndPtr = ptr + 1;
                while (*localEndPtr != '\0' && *localEndPtr != '\r' && *localEndPtr != '\n' && localEndPtr < endPtr) localEndPtr++;
                std::string testStr(ptr, size_t(localEndPtr - ptr));
                std::smatch sm;
                if (std::regex_match(testStr, sm, vertex_only_tester)) { faceFormat = vertex_only; break; }
                if (std::regex_match(testStr, sm, vertex_texture_tester)) { faceFormat = vertex_texture; break; }
                if (std::regex_match(testStr, sm, vertex_normal_tester)) { faceFormat = vertex_normal; break; }
                if (std::regex_match(testStr, sm, vertex_texture_normal_tester)) { faceFormat = vertex_texture_normal; break; }
                break;
            }

            if (faceFormat == vertex_only)
            {
                triangle.vertex[0] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                if (ptr >= endPtr) continue;
                triangle.vertex[1] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                if (ptr >= endPtr) continue;
                triangle.vertex[2] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                triangle.normal[0] = SIZE_MAX;
                triangle.normal[1] = SIZE_MAX;
                triangle.normal[2] = SIZE_MAX;
                triangle.uv[0] = SIZE_MAX;
                triangle.uv[1] = SIZE_MAX;
                triangle.material = currentMaterial;
                triangleList.push_back(triangle);
                ptr++;
                if (m_badMesh)   // currently duplicate the triangle but with reversed winding but this could be improved
                {

                    std::swap(triangle.vertex[1], triangle.vertex[2]);
                    std::swap(triangle.normal[1], triangle.normal[2]);
                    std::swap(triangle.uv[1], triangle.uv[2]);
                    triangleList.push_back(triangle);
                }
                continue;
            }

            if (faceFormat == vertex_normal)
            {
                triangle.vertex[0] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                ptr += 2;
                if (ptr >= endPtr) continue;
                triangle.normal[0] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                if (ptr >= endPtr) continue;
                triangle.vertex[1] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                ptr += 2;
                if (ptr >= endPtr) continue;
                triangle.normal[1] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                if (ptr >= endPtr) continue;
                triangle.vertex[2] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                ptr += 2;
                if (ptr >= endPtr) continue;
                triangle.normal[2] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                triangle.uv[0] = SIZE_MAX;
                triangle.uv[1] = SIZE_MAX;
                triangle.material = currentMaterial;
                triangleList.push_back(triangle);
                ptr++;
                if (m_badMesh)   // currently duplicate the triangle but with reversed winding but this could be improved
                {

                    std::swap(triangle.vertex[1], triangle.vertex[2]);
                    std::swap(triangle.normal[1], triangle.normal[2]);
                    std::swap(triangle.uv[1], triangle.uv[2]);
                    triangleList.push_back(triangle);
                }
                continue;
            }

            if (faceFormat == vertex_texture)
            {
                triangle.vertex[0] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                ptr++;
                if (ptr >= endPtr) continue;
                triangle.uv[0] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                if (ptr >= endPtr) continue;
                triangle.normal[0] = SIZE_MAX;
                triangle.vertex[1] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                ptr++;
                if (ptr >= endPtr) continue;
                triangle.uv[1] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                if (ptr >= endPtr) continue;
                triangle.normal[1] = SIZE_MAX;
                triangle.vertex[2] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                ptr++;
                if (ptr >= endPtr) continue;
                triangle.uv[2] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                triangle.normal[2] = SIZE_MAX;
                triangle.material = currentMaterial;
                triangleList.push_back(triangle);
                ptr++;
                if (m_badMesh)   // currently duplicate the triangle but with reversed winding but this could be improved
                {

                    std::swap(triangle.vertex[1], triangle.vertex[2]);
                    std::swap(triangle.normal[1], triangle.normal[2]);
                    std::swap(triangle.uv[1], triangle.uv[2]);
                    triangleList.push_back(triangle);
                }
                continue;
            }

            if (faceFormat == vertex_texture_normal)
            {
                triangle.vertex[0] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                ptr++;
                if (ptr >= endPtr) continue;
                triangle.uv[0] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                ptr++;
                if (ptr >= endPtr) continue;
                triangle.normal[0] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                if (ptr >= endPtr) continue;
                triangle.vertex[1] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                ptr++;
                if (ptr >= endPtr) continue;
                triangle.uv[1] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                ptr++;
                if (ptr >= endPtr) continue;
                triangle.normal[1] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                if (ptr >= endPtr) continue;
                triangle.vertex[2] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                ptr++;
                if (ptr >= endPtr) continue;
                triangle.uv[2] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                ptr++;
                if (ptr >= endPtr) continue;
                triangle.normal[2] = GaitSym::GSUtil::fast_a_to_uint64_t(ptr, &ptr) - 1;
                triangle.material = currentMaterial;
                triangleList.push_back(triangle);
                ptr++;
                if (m_badMesh)   // currently duplicate the triangle but with reversed winding but this could be improved
                {
                    std::swap(triangle.vertex[1], triangle.vertex[2]);
                    std::swap(triangle.normal[1], triangle.normal[2]);
                    std::swap(triangle.uv[1], triangle.uv[2]);
                    triangleList.push_back(triangle);
                }
                continue;
            }
        }

        // skip till end of line
        while (ptr < endPtr && *ptr != '\n' && *ptr != '\r') { ptr++; }
        ptr++;
    }

    m_vertexList.clear();
    m_vertexList.reserve(triangleList.size() * 9);
    m_normalList.clear();
    m_normalList.reserve(triangleList.size() * 9);
    m_colourList.clear();
    m_colourList.reserve(triangleList.size() * 9);
    m_uvList.clear();
    m_uvList.reserve(triangleList.size() * 6);
    float colour[3] = {m_blendColour.redF(), m_blendColour.greenF(), m_blendColour.blueF() };
    for (auto &&it : triangleList)
    {
        m_vertexList.push_back(vertexList[it.vertex[0]].x);
        m_vertexList.push_back(vertexList[it.vertex[0]].y);
        m_vertexList.push_back(vertexList[it.vertex[0]].z);
        m_vertexList.push_back(vertexList[it.vertex[1]].x);
        m_vertexList.push_back(vertexList[it.vertex[1]].y);
        m_vertexList.push_back(vertexList[it.vertex[1]].z);
        m_vertexList.push_back(vertexList[it.vertex[2]].x);
        m_vertexList.push_back(vertexList[it.vertex[2]].y);
        m_vertexList.push_back(vertexList[it.vertex[2]].z);
        if (it.normal[0] != SIZE_MAX)
        {
            m_normalList.push_back(normalList[it.normal[0]].x);
            m_normalList.push_back(normalList[it.normal[0]].y);
            m_normalList.push_back(normalList[it.normal[0]].z);
            m_normalList.push_back(normalList[it.normal[1]].x);
            m_normalList.push_back(normalList[it.normal[1]].y);
            m_normalList.push_back(normalList[it.normal[1]].z);
            m_normalList.push_back(normalList[it.normal[2]].x);
            m_normalList.push_back(normalList[it.normal[2]].y);
            m_normalList.push_back(normalList[it.normal[2]].z);
        }
        else
        {
            ComputeFaceNormal(vertexList[it.vertex[0]].data(), vertexList[it.vertex[1]].data(), vertexList[it.vertex[2]].data(), normal.data());
            m_normalList.push_back(normal.x);
            m_normalList.push_back(normal.y);
            m_normalList.push_back(normal.z);
            m_normalList.push_back(normal.x);
            m_normalList.push_back(normal.y);
            m_normalList.push_back(normal.z);
            m_normalList.push_back(normal.x);
            m_normalList.push_back(normal.y);
            m_normalList.push_back(normal.z);
        }
        if (it.material)
        {
            for (size_t i = 0; i < 9; i++)
            {
                m_colourList.push_back(it.material->Kd[i % 3]);
            }
        }
        else
        {
            for (size_t i = 0; i < 9; i++) m_colourList.push_back(colour[i % 3]);
        }
        if (it.uv[0] != SIZE_MAX)
        {
            m_uvList.push_back(uvList[it.uv[0]].x);
            m_uvList.push_back(uvList[it.uv[0]].y);
            m_uvList.push_back(uvList[it.uv[1]].x);
            m_uvList.push_back(uvList[it.uv[1]].y);
            m_uvList.push_back(uvList[it.uv[2]].x);
            m_uvList.push_back(uvList[it.uv[2]].y);
        }
        else
        {
            m_uvList.push_back(0);
            m_uvList.push_back(0);
            m_uvList.push_back(0);
            m_uvList.push_back(0);
            m_uvList.push_back(0);
            m_uvList.push_back(0);
        }
    }

    m_meshStore.setTargetMemory(Preferences::valueDouble("MeshStoreMemoryFraction"));
    m_meshStore.addMesh(filename, m_vertexList, m_normalList, m_colourList, m_uvList, m_lowerBound, m_upperBound);

    return 0;
}

int FacetedObject::ParseOBJMaterialFile(const std::string &filename, std::map<std::string, OBJMaterial> *materialMap)
{
    GaitSym::DataFile materialsFile;
    if (materialsFile.ReadFile(filename))
    {
        std::string strippedFilename = pystring::strip(filename);
        if (materialsFile.ReadFile(strippedFilename))
        {
            if (strippedFilename[0] == '"' && strippedFilename.back() == '"')
            {
                if (materialsFile.ReadFile(strippedFilename.substr(1, strippedFilename.size() - 2)))
                    return __LINE__;
            }
            else
            {
                return __LINE__;
            }
        }
    }

    std::string materialsData(materialsFile.GetRawData(), materialsFile.GetSize());
    std::vector<std::string> lines;
    pystring::splitlines(materialsData, lines);
    std::string currentMaterialName;
    OBJMaterial currentMaterial = {};
    for (size_t i =0; i < lines.size(); i++)
    {
        std::string line = pystring::strip(lines[i]);
        if (pystring::startswith(line, "#"s)) continue;
        if (pystring::startswith(line, "newmtl "s))
        {
            if (currentMaterialName.size())
            {
                (*materialMap)[currentMaterialName] = currentMaterial;
                currentMaterial = {};
            }
            currentMaterialName = pystring::strip(line.substr("newmtl "s.size(), std::string::npos));
            continue;
        }

        std::vector<std::string> tokens;
        pystring::split(line, tokens);
        if (tokens.size() >= 2 && tokens[0] == "Ns"s)
        {
            currentMaterial.Ns = GaitSym::GSUtil::Double(tokens[1]);
            continue;
        }
        if (tokens.size() >= 4 && tokens[0] == "Ka"s)
        {
            currentMaterial.Ka[0] =GaitSym::GSUtil::Double(tokens[1]);
            currentMaterial.Ka[1] =GaitSym::GSUtil::Double(tokens[2]);
            currentMaterial.Ka[2] =GaitSym::GSUtil::Double(tokens[3]);
            continue;
        }
        if (tokens.size() >= 4 && tokens[0] == "Kd"s)
        {
            currentMaterial.Kd[0] =GaitSym::GSUtil::Double(tokens[1]);
            currentMaterial.Kd[1] =GaitSym::GSUtil::Double(tokens[2]);
            currentMaterial.Kd[2] =GaitSym::GSUtil::Double(tokens[3]);
            continue;
        }
        if (tokens.size() >= 4 && tokens[0] == "Ks"s)
        {
            currentMaterial.Ks[0] =GaitSym::GSUtil::Double(tokens[1]);
            currentMaterial.Ks[1] =GaitSym::GSUtil::Double(tokens[2]);
            currentMaterial.Ks[2] =GaitSym::GSUtil::Double(tokens[3]);
            continue;
        }
        if (tokens.size() >= 4 && tokens[0] == "Ke"s)
        {
            currentMaterial.Ke[0] =GaitSym::GSUtil::Double(tokens[1]);
            currentMaterial.Ke[1] =GaitSym::GSUtil::Double(tokens[2]);
            currentMaterial.Ke[2] =GaitSym::GSUtil::Double(tokens[3]);
            continue;
        }
        if (tokens.size() >= 2 && tokens[0] == "Ni"s)
        {
            currentMaterial.Ni = GaitSym::GSUtil::Double(tokens[1]);
            continue;
        }
        if (tokens.size() >= 2 && tokens[0] == "d"s)
        {
            currentMaterial.d = GaitSym::GSUtil::Double(tokens[1]);
            continue;
        }
        if (tokens.size() >= 2 && tokens[0] == "illum"s)
        {
            currentMaterial.illum = GaitSym::GSUtil::Int(tokens[1]);
            continue;
        }
    }
    if (currentMaterialName.size())
    {
        (*materialMap)[currentMaterialName] = currentMaterial;
        currentMaterial = {};
    }
    return 0;
}

int FacetedObject::ParsePLYFile(const std::string &filename)
{
    m_filename = filename;
    MeshStoreObject *meshStoreObject = m_meshStore.getMesh(filename);
    if (meshStoreObject)
    {
        m_vertexList = meshStoreObject->vertexList;
        m_normalList = meshStoreObject->normalList;
        m_colourList = meshStoreObject->colourList;
        m_uvList = meshStoreObject->uvList;
        m_lowerBound[0] = meshStoreObject->lowerBound[0];
        m_lowerBound[1] = meshStoreObject->lowerBound[1];
        m_lowerBound[2] = meshStoreObject->lowerBound[2];
        m_upperBound[0] = meshStoreObject->upperBound[0];
        m_upperBound[1] = meshStoreObject->upperBound[1];
        m_upperBound[2] = meshStoreObject->upperBound[2];
        return 0;
    }
    try
    {
        std::ifstream ss;
        ss.exceptions(std::ios::failbit|std::ios::badbit|std::ios::eofbit);
#if (defined(_WIN32) || defined(WIN32)) && !defined(__MINGW32__)
        ss.open(GaitSym::DataFile::ConvertUTF8ToWide(filename), std::ios::binary);
#else
        ss.open(filename, std::ios::binary);
#endif
        tinyply::PlyFile file;
        file.parse_header(ss);

        for (auto &&c : file.get_comments()) qDebug() << "Comment: " << c.c_str() << "\n";
        std::vector<tinyply::PlyElement> elementVector = file.get_elements();
        std::map<std::string, tinyply::PlyProperty *> vertexProperties;
        std::map<std::string, tinyply::PlyProperty *> faceProperties;
        for (auto &&e : elementVector)
        {
            qDebug() << "element - " << e.name.c_str() << " (" << e.size << ")" << "\n";
            if (e.name == "vertex") for (auto &&p : e.properties) vertexProperties[p.name] = &p;
            else if (e.name == "face") for (auto &&p : e.properties) faceProperties[p.name] = &p;
            for (auto &&p : e.properties)
            {
                if (p.isList) qDebug() << "\tproperty - " << p.name.c_str() << " (" << tinyply::PropertyTable[p.listType].str.c_str() << ")" << " (" << tinyply::PropertyTable[p.propertyType].str.c_str() << ")" << "\n";
                else
                    qDebug() << "\tproperty - " << p.name.c_str() << " (" << tinyply::PropertyTable[p.propertyType].str.c_str() << ")" << "\n";
            }
        }

        // Tinyply treats parsed data as untyped byte buffers. See below for examples.
        std::shared_ptr<tinyply::PlyData> vertices, normals, faces, texcoords, colours;

        // The header information can be used to programmatically extract properties on elements
        // known to exist in the header prior to reading the data. For brevity of this sample, properties
        // like vertex position are hard-coded:
        if (vertexProperties.find("x") != vertexProperties.end())
        {
            try
            {
                vertices = file.request_properties_from_element("vertex", { "x", "y", "z" });
            }
            catch (const std::exception &e)
            {
                qDebug() << "tinyply exception: " << e.what() << "\n";
            }
        }

        if (vertexProperties.find("nx") != vertexProperties.end())
        {
            try
            {
                normals = file.request_properties_from_element("vertex", { "nx", "ny", "nz" });
            }
            catch (const std::exception &e)
            {
                qDebug() << "tinyply exception: " << e.what() << "\n";
            }
        }

        if (vertexProperties.find("u") != vertexProperties.end())
        {
            try
            {
                texcoords = file.request_properties_from_element("vertex", { "u", "v" });
            }
            catch (const std::exception &e)
            {
                qDebug() << "tinyply exception: " << e.what() << "\n";
            }
        }

        if (vertexProperties.find("red") != vertexProperties.end())
        {
            try
            {
                colours = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" });
            }
            catch (const std::exception &e)
            {
                qDebug() << "tinyply exception: " << e.what() << "\n";
            }
        }

        // Providing a list size hint (the last argument) is a 2x performance improvement. If you have
        // arbitrary ply files, it is best to leave this 0.
        // note: tinyply does not cope with varying numbers of vertices in each face and GaitSym wants triangles
        if (faceProperties.find("vertex_indices") != faceProperties.end())
        {
            try
            {
                faces = file.request_properties_from_element("face", { "vertex_indices" }, 3);
            }
            catch (const std::exception &e)
            {
                qDebug() << "tinyply exception: " << e.what() << "\n";
            }
        }

        file.read(ss);

        if (vertices) qDebug() << "\tRead " << vertices->count << " total vertices " << "\n";
        if (normals) qDebug() << "\tRead " << normals->count << " total vertex normals " << "\n";
        if (texcoords) qDebug() << "\tRead " << texcoords->count << " total vertex texcoords " << "\n";
        if (colours) qDebug() << "\tRead " << colours->count << " total vertex colours " << "\n";
        if (faces) qDebug() << "\tRead " << faces->count << " total faces " << "\n";

        AllocateMemory(faces->count);
        if (vertices->t == tinyply::Type::FLOAT32)
        {
            int32_t *vertexIndexPtr = reinterpret_cast<int32_t *>(faces->buffer.get());
            float *vertexPtr = reinterpret_cast<float *>(vertices->buffer.get());
            double triangle[9];
            for (size_t i = 0; i < faces->count; i++)
            {
                for (size_t j = 0; j < 3; j++) (&triangle[0])[j] = double((&vertexPtr[3 * vertexIndexPtr[i * 3 + 0]])[j]);
                for (size_t j = 0; j < 3; j++) (&triangle[3])[j] = double((&vertexPtr[3 * vertexIndexPtr[i * 3 + 1]])[j]);
                for (size_t j = 0; j < 3; j++) (&triangle[6])[j] = double((&vertexPtr[3 * vertexIndexPtr[i * 3 + 2]])[j]);
                AddTriangle(triangle);
//                qDebug("Triangle %d (%d,%d,%d)", i, vertexIndexPtr[i * 3 + 0], vertexIndexPtr[i * 3 + 1], vertexIndexPtr[i * 3 + 2]);

            }
        }
        if (vertices->t == tinyply::Type::FLOAT64)
        {
            int32_t *vertexIndexPtr = reinterpret_cast<int32_t *>(faces->buffer.get());
            double *vertexPtr = reinterpret_cast<double *>(vertices->buffer.get());
            double triangle[9];
            for (size_t i = 0; i < faces->count; i++)
            {
                std::copy_n(&triangle[0], 3, &vertexPtr[3 * vertexIndexPtr[i * 3 + 0]]);
                std::copy_n(&triangle[3], 3, &vertexPtr[3 * vertexIndexPtr[i * 3 + 1]]);
                std::copy_n(&triangle[6], 3, &vertexPtr[3 * vertexIndexPtr[i * 3 + 2]]);
                AddTriangle(triangle);
            }
        }

        m_meshStore.setTargetMemory(Preferences::valueDouble("MeshStoreMemoryFraction"));
        m_meshStore.addMesh(filename, m_vertexList, m_normalList, m_colourList, m_uvList, m_lowerBound, m_upperBound);
    }
    catch (const std::exception &e)
    {
        qDebug() << "Caught tinyply exception: " << e.what() << "\n";
        return __LINE__;
    }
    return 0;
}

int FacetedObject::ReadFromMemory(const char *data, size_t len, bool binary, const std::string &meshName)
{
    m_filename = meshName;
    MeshStoreObject *meshStoreObject = m_meshStore.getMesh(meshName);
    if (meshStoreObject)
    {
        m_vertexList = meshStoreObject->vertexList;
        m_normalList = meshStoreObject->normalList;
        m_colourList = meshStoreObject->colourList;
        m_uvList = meshStoreObject->uvList;
        m_lowerBound[0] = meshStoreObject->lowerBound[0];
        m_lowerBound[1] = meshStoreObject->lowerBound[1];
        m_lowerBound[2] = meshStoreObject->lowerBound[2];
        m_upperBound[0] = meshStoreObject->upperBound[0];
        m_upperBound[1] = meshStoreObject->upperBound[1];
        m_upperBound[2] = meshStoreObject->upperBound[2];
        return 0;
    }

    m_vertexList.clear();
    m_normalList.clear();
    m_colourList.clear();
    m_uvList.clear();
    if (binary)
    {
        if (len < sizeof(size_t) + 6 * sizeof(double)) return __LINE__;
        const char *ptr = data;
        size_t numTriangles = *reinterpret_cast<size_t *>(*ptr);
        ptr += sizeof(size_t);
        if (len < sizeof(size_t) + 6 * sizeof(double) + numTriangles * 3 * 11 * sizeof(double)) return __LINE__;
        m_lowerBound[0] = *reinterpret_cast<double *>(*ptr);
        ptr += sizeof(double);
        m_lowerBound[1] = *reinterpret_cast<double *>(*ptr);
        ptr += sizeof(double);
        m_lowerBound[2] = *reinterpret_cast<double *>(*ptr);
        ptr += sizeof(double);
        m_upperBound[0] = *reinterpret_cast<double *>(*ptr);
        ptr += sizeof(double);
        m_upperBound[1] = *reinterpret_cast<double *>(*ptr);
        ptr += sizeof(double);
        m_upperBound[2] = *reinterpret_cast<double *>(*ptr);
        ptr += sizeof(double);
        size_t numElements = numTriangles * 9;
        m_vertexList.reserve(numElements);
        m_normalList.reserve(numElements);
        m_colourList.reserve(numElements);
        m_uvList.reserve(numTriangles * 6);
        for (size_t i = 0; i < numElements; i++)
        {
            m_vertexList.push_back(*reinterpret_cast<double *>(*ptr));
            ptr += sizeof(double);
        }
        for (size_t i = 0; i < numElements; i++)
        {
            m_normalList.push_back(*reinterpret_cast<double *>(*ptr));
            ptr += sizeof(double);
        }
        for (size_t i = 0; i < numElements; i++)
        {
            m_colourList.push_back(*reinterpret_cast<float *>(*ptr));
            ptr += sizeof(double);
        }
        for (size_t i = 0; i < numTriangles * 6; i++)
        {
            m_uvList.push_back(*reinterpret_cast<double *>(*ptr));
            ptr += sizeof(double);
        }
    }
    else
    {
        if (data[len]) return __LINE__; // must be null terminated for ASCII case
        const char *endPtr;
        size_t numTriangles = GaitSym::GSUtil::fast_a_to_uint64_t(data, &endPtr);
        m_lowerBound[0] = GaitSym::GSUtil::fast_a_to_double(endPtr, &endPtr);
        m_lowerBound[1] = GaitSym::GSUtil::fast_a_to_double(endPtr, &endPtr);
        m_lowerBound[2] = GaitSym::GSUtil::fast_a_to_double(endPtr, &endPtr);
        m_upperBound[0] = GaitSym::GSUtil::fast_a_to_double(endPtr, &endPtr);
        m_upperBound[1] = GaitSym::GSUtil::fast_a_to_double(endPtr, &endPtr);
        m_upperBound[2] = GaitSym::GSUtil::fast_a_to_double(endPtr, &endPtr);
        size_t numElements = numTriangles * 9;
        m_vertexList.reserve(numElements);
        m_normalList.reserve(numElements);
        m_colourList.reserve(numElements);
        m_uvList.reserve(numTriangles * 6);
        for (size_t i = 0; i < numElements; i++) m_vertexList.push_back(GaitSym::GSUtil::fast_a_to_double(endPtr, &endPtr));
        for (size_t i = 0; i < numElements; i++) m_normalList.push_back(GaitSym::GSUtil::fast_a_to_double(endPtr, &endPtr));
        for (size_t i = 0; i < numElements; i++) m_colourList.push_back(float(GaitSym::GSUtil::fast_a_to_double(endPtr, &endPtr)));
        for (size_t i = 0; i < numTriangles * 6; i++) m_uvList.push_back(GaitSym::GSUtil::fast_a_to_double(endPtr, &endPtr));
    }
    m_meshStore.setTargetMemory(Preferences::valueDouble("MeshStoreMemoryFraction"));
    m_meshStore.addMesh(meshName, m_vertexList, m_normalList, m_colourList, m_uvList, m_lowerBound, m_upperBound);
    return 0;
}

void FacetedObject::SaveToMemory(std::vector<char> *data, bool binary)
{
    if (binary)
    {
        data->clear();
        data->resize(sizeof(size_t) + 6 * sizeof(double) + m_vertexList.size() * sizeof(double) +
                     m_normalList.size() * sizeof(double) +
                     m_colourList.size() * sizeof(float) +
                     m_uvList.size() * sizeof(double));
        size_t *ptr = reinterpret_cast<size_t *>(data->data());
        *ptr++ = m_vertexList.size() / 3;
        double *dPtr = reinterpret_cast<double *>(ptr);
        *dPtr++ = m_lowerBound[0];
        *dPtr++ = m_lowerBound[1];
        *dPtr++ = m_lowerBound[2];
        *dPtr++ = m_upperBound[0];
        *dPtr++ = m_upperBound[1];
        *dPtr++ = m_upperBound[2];
        for (size_t i = 0; i < m_vertexList.size(); i++) *dPtr++ = m_vertexList[i];
        for (size_t i = 0; i < m_normalList.size(); i++) *dPtr++ = m_normalList[i];
        for (size_t i = 0; i < m_colourList.size(); i++) *dPtr++ = m_colourList[i];
        for (size_t i = 0; i < m_uvList.size(); i++) *dPtr++ = m_uvList[i];
    }
    else
    {
        data->clear();
        char buf[32];
        int l = std::sprintf(buf, "%zud\n", m_vertexList.size() / 3);
        std::copy_n(buf, l, std::back_inserter(*data));
        l = std::sprintf(buf, "%.17g\n", m_lowerBound[0]);
        std::copy_n(buf, l, std::back_inserter(*data));
        l = std::sprintf(buf, "%.17g\n", m_lowerBound[1]);
        std::copy_n(buf, l, std::back_inserter(*data));
        l = std::sprintf(buf, "%.17g\n", m_lowerBound[2]);
        std::copy_n(buf, l, std::back_inserter(*data));
        l = std::sprintf(buf, "%.17g\n", m_upperBound[0]);
        std::copy_n(buf, l, std::back_inserter(*data));
        l = std::sprintf(buf, "%.17g\n", m_upperBound[1]);
        std::copy_n(buf, l, std::back_inserter(*data));
        l = std::sprintf(buf, "%.17g\n", m_upperBound[2]);
        std::copy_n(buf, l, std::back_inserter(*data));
        for (size_t i = 0; i < m_vertexList.size(); i++)
        {
            l = std::sprintf(buf, "%.17g\n", m_vertexList[i]);
            std::copy_n(buf, l, std::back_inserter(*data));
        }
        for (size_t i = 0; i < m_normalList.size(); i++)
        {
            l = std::sprintf(buf, "%.17g\n", m_normalList[i]);
            std::copy_n(buf, l, std::back_inserter(*data));
        }
        for (size_t i = 0; i < m_colourList.size(); i++)
        {
            l = std::sprintf(buf, "%.10g\n", m_colourList[i]);
            std::copy_n(buf, l, std::back_inserter(*data));
        }
        for (size_t i = 0; i < m_uvList.size(); i++)
        {
            l = std::sprintf(buf, "%.17g\n", m_uvList[i]);
            std::copy_n(buf, l, std::back_inserter(*data));
        }
        data->push_back('\0');
    }
}

int FacetedObject::ReadFromResource(const QString &resourceName)
{
    m_filename = resourceName.toStdString();
    MeshStoreObject *meshStoreObject = m_meshStore.getMesh(resourceName.toStdString());
    if (meshStoreObject)
    {
        m_vertexList = meshStoreObject->vertexList;
        m_normalList = meshStoreObject->normalList;
        m_colourList = meshStoreObject->colourList;
        m_uvList = meshStoreObject->uvList;
        m_lowerBound[0] = meshStoreObject->lowerBound[0];
        m_lowerBound[1] = meshStoreObject->lowerBound[1];
        m_lowerBound[2] = meshStoreObject->lowerBound[2];
        m_upperBound[0] = meshStoreObject->upperBound[0];
        m_upperBound[1] = meshStoreObject->upperBound[1];
        m_upperBound[2] = meshStoreObject->upperBound[2];
        return 0;
    }

    QFile file(resourceName);
    if (!file.open(QIODevice::ReadOnly)) return __LINE__;
    QByteArray data = file.readAll();
    file.close();
    QTextStream dataStream(data, QIODevice::ReadOnly);

    m_vertexList.clear();
    m_normalList.clear();
    m_colourList.clear();
    m_uvList.clear();

    QString key;
    dataStream >> key;
    if (key != "xyzxnynznrgb") return __LINE__;
    size_t numTriangles;
    dataStream >> numTriangles;
    if (numTriangles <= 0) return __LINE__;
    size_t numVertices = numTriangles * 3;
    size_t numElements = numVertices * 3;
    dataStream >> m_lowerBound[0] >> m_lowerBound[1] >> m_lowerBound[2] >> m_upperBound[0] >> m_upperBound[1] >> m_upperBound[2];

    m_vertexList.reserve(numElements);
    m_normalList.reserve(numElements);
    m_colourList.reserve(numElements);
    m_uvList.reserve(numVertices * 2);
    double x, y, z, nx, ny, nz;
    float r, g, b;
    for (size_t i = 0; i < numVertices; i++)
    {
        dataStream >> x >> y >> z >> nx >> ny >> nz >> r >> g >> b;
        m_vertexList.push_back(x); m_vertexList.push_back(y); m_vertexList.push_back(z);
        m_normalList.push_back(nx); m_normalList.push_back(ny); m_normalList.push_back(nz);
        m_colourList.push_back(r); m_colourList.push_back(g); m_colourList.push_back(b);
        m_uvList.push_back(0); m_uvList.push_back(0);
    }
    m_meshStore.setTargetMemory(Preferences::valueDouble("MeshStoreMemoryFraction"));
    m_meshStore.addMesh(resourceName.toStdString(), m_vertexList, m_normalList, m_colourList, m_uvList, m_lowerBound, m_upperBound);
    return 0;
}

void FacetedObject::Draw()
{
    if (m_vertexList.size() == 0) return;
    if (!m_mesh)
    {
        auto geometry = threepp::BufferGeometry::create();
        std::vector<float> position;
        position.reserve(m_vertexList.size());
        for (size_t i =0; i < m_vertexList.size(); i++) { position.push_back(float(m_vertexList[i])); }
        std::vector<float> normal;
        normal.reserve(m_normalList.size());
        for (size_t i =0; i < m_normalList.size(); i++) { normal.push_back(float(m_normalList[i])); }
        std::vector<float> color;
        color.reserve(m_colourList.size());
        for (size_t i =0; i < m_colourList.size(); i++) { color.push_back(m_colourList[i]); } //FIX ME - this copy may well not be necessary
        std::vector<float> uv;
        uv.reserve(m_uvList.size());
        for (size_t i =0; i < m_uvList.size(); i++) { uv.push_back(float(m_uvList[i])); }
        geometry->setAttribute("position", threepp::FloatBufferAttribute::create(position, 3));
        geometry->setAttribute("normal", threepp::FloatBufferAttribute::create(normal, 3));
        geometry->setAttribute("color", threepp::FloatBufferAttribute::create(color, 3));
        geometry->setAttribute("uv", threepp::FloatBufferAttribute::create(uv, 2));
        auto material = threepp::MeshStandardMaterial::create();
        if (m_blendFraction < 1.0)
        {
            material->vertexColors = true;
        }
        else
        {
            material->color.setRGB(m_blendColour.redF(), m_blendColour.greenF(), m_blendColour.blueF());
        }
        material->wireframe = m_wireframe;
        material->roughness = 0.5;
        material->metalness = 0.5;
        if (m_blendColour.alphaF() < 1.0)
        {
            material->transparent = true;
            material->opacity = m_blendColour.alphaF();
            material->side = threepp::Side::Double;
        }
        else
        {
            material->transparent = false;
        }
        m_mesh = threepp::Mesh::create(geometry, material);
        m_mesh->castShadow = m_castShadow;
        m_mesh->receiveShadow = m_receiveShadow;
        // qDebug() << QTime::currentTime().toString() << " Mesh created";
    }
    else
    {
        // qDebug() << QTime::currentTime().toString() << " Mesh reused";
    }
    m_mesh->position.set(m_displayPosition.x, m_displayPosition.y, m_displayPosition.z);
    m_mesh->scale.set(m_displayScale.x, m_displayScale.y, m_displayScale.z);
    m_mesh->quaternion.set(m_displayQuaternion.x, m_displayQuaternion.y, m_displayQuaternion.z, m_displayQuaternion.n);
    m_mesh->visible = m_visible;
    if (m_scene && m_mesh->parent != m_scene.get()) { m_scene->add(m_mesh); }
}

// Write a FacetedObject out as a POVRay file
void FacetedObject::WritePOVRay(std::string filename)
{
    std::ostringstream objData;
    WritePOVRay(objData);
    try
    {
        std::ofstream f;
        f.exceptions(std::ios::failbit|std::ios::badbit);
#if (defined(_WIN32) || defined(WIN32)) && !defined(__MINGW32__)
        f.open(GaitSym::DataFile::ConvertUTF8ToWide(filename));
#else
        f.open(filename);
#endif
        f << objData.str();
        f.close();
    }
    catch (...)
    {
        std::cerr << "Error writing " << filename << "\n";
    }
}


// write the object out as a POVRay string
// currently assumes all faces are triangles (call Triangulate if conversion is necessary)
void FacetedObject::WritePOVRay(std::ostringstream &theString)
{
    size_t i, j;
    pgd::Vector3 prel, p, result;

    theString.precision(7); // should be plenty

    theString << "object {\n";
    theString << "  mesh {\n";

    // first faces
    for (i = 0; i < m_vertexList.size() / 9; i++)
    {
        theString << "    triangle {\n";
        for (j = 0; j < 3; j++)
        {
            prel.Set(m_vertexList[i * 9 + j * 3], m_vertexList[i * 9 + j * 3 + 1], m_vertexList[i * 9 + j * 3 + 2]);
            p = m_displayRotation * prel;
            result = p + m_displayPosition;
            result[0] = p[0] + m_displayPosition[0];
            result[1] = p[1] + m_displayPosition[1];
            result[2] = p[2] + m_displayPosition[2];
            theString << "      <" << result[0] << "," << result[1] << "," << result[2] << ">\n";
        }
        theString << "    }\n";
    }

    // now colour
    theString << "    pigment {\n";
    theString << "      color rgbf<" << m_blendColour.redF() << "," << m_blendColour.greenF() << "," << m_blendColour.blueF() << "," << 1 - m_blendColour.alphaF() << ">\n";
    theString << "    }\n";
    theString << "  }\n";
    theString << "}\n\n";
}

// Write a FacetedObject out as an OBJ file
void FacetedObject::WriteOBJFile(std::string filename)
{
    std::ostringstream objData;
    WriteOBJFile(objData);
    try
    {
        std::ofstream f;
        f.exceptions(std::ios::failbit|std::ios::badbit);
#if (defined(_WIN32) || defined(WIN32)) && !defined(__MINGW32__)
        f.open(GaitSym::DataFile::ConvertUTF8ToWide(filename));
#else
        f.open(filename);
#endif
        f << objData.str();
        f.close();
    }
    catch (...)
    {
        std::cerr << "Error writing " << filename << "\n";
    }
}

const pgd::Vector3 &FacetedObject::GetDisplayPosition() const
{
    return m_displayPosition;
}

const pgd::Matrix3x3 &FacetedObject::GetDisplayRotation() const
{
    return m_displayRotation;
}

const pgd::Vector3 &FacetedObject::GetDisplayScale() const
{
    return m_displayScale;
}

size_t FacetedObject::GetNumVertices() const
{
    return m_vertexList.size() / 3;
}

const double *FacetedObject::GetVertex(size_t i) const
{
    return &m_vertexList.at(3 * i);
}

const double *FacetedObject::GetNormal(size_t i) const
{
    return &m_normalList.at(3 * i);
}

const double *FacetedObject::GetVertexList() const
{
    return m_vertexList.data();
}

const double *FacetedObject::GetNormalList() const
{
    return m_normalList.data();
}

const float *FacetedObject::GetColourList() const
{
    return m_colourList.data();
}

size_t FacetedObject::GetNumTriangles() const
{
    return m_vertexList.size() / 9;
}

const double *FacetedObject::GetTriangle(size_t i) const
{
    return &m_vertexList.at(9 * i);
}

pgd::Vector3 FacetedObject::upperBound() const
{
    return m_upperBound;
}

SimulationWidget *FacetedObject::simulationWidget() const
{
    return m_simulationWidget;
}

void FacetedObject::setSimulationWidget(SimulationWidget *glWidget)
{
    m_simulationWidget = glWidget;
}


void FacetedObject::setBlendColour(const QColor &blendColour, double blendFraction)
{
    if (m_blendColour == blendColour && m_blendFraction == blendFraction) return;
    m_blendColour = blendColour;
    m_blendFraction = blendFraction;
}

pgd::Vector3 FacetedObject::lowerBound() const
{
    return m_lowerBound;
}

// Write a FacetedObject out as a OBJ
void FacetedObject::WriteOBJFile(std::ostringstream &out)
{
    size_t i, j;
    pgd::Vector3 prel, result;

    out.precision(7); // should be plenty

    if (m_useRelativeOBJ)
    {
        // write out the vertices, faces, groups and objects
        // this is the relative version - inefficient but allows concatenation of objects
        for (i = 0; i < m_vertexList.size() / 9; i++)
        {
            for (j = 0; j < 3; j++)
            {
                prel[0] = m_vertexList[i * 9 + j * 3];
                prel[1] = m_vertexList[i * 9 + j * 3 + 1];
                prel[2] = m_vertexList[i * 9 + j * 3 + 2];
                ApplyDisplayTransformation(prel, &result);
                out << "v " << result[0] << " " << result[1] << " " << result[2] << "\n";
            }

            out << "f ";
            for (j = 0; j < 3; j++)
            {
                if (j == 3)
                    out << j - 3 << "\n";
                else
                    out << j - 3 << " ";
            }
        }
    }
    else
    {
        for (i = 0; i < m_vertexList.size() / 9; i++)
        {
            for (j = 0; j < 3; j++)
            {
                prel[0] = m_vertexList[i * 9 + j * 3];
                prel[1] = m_vertexList[i * 9 + j * 3 + 1];
                prel[2] = m_vertexList[i * 9 + j * 3 + 2];
                ApplyDisplayTransformation(prel, &result);
                out << "v " << result[0] << " " << result[1] << " " << result[2] << "\n";
            }
        }

        for (i = 0; i < m_vertexList.size() / 9; i++)
        {
            out << "f ";
            for (j = 0; j < 3; j++)
            {
                // note this files vertex list start at 1 not zero
                if (j == 2)
                    out << i * 3 + j + 1 + m_vertexOffset << "\n";
                else
                    out << i * 3 + j + 1 + m_vertexOffset << " ";
            }
        }
        m_vertexOffset += m_vertexList.size() / 3;
    }
}

// Write a FacetedObject out as a USD file
void FacetedObject::WriteUSDFile(std::ostringstream &out, const std::string &name)
{
    // add the preamble
    out << "def Xform \"" << name << "_xform\" (kind = \"component\")\n";
    out << "{\n";

    // create the extent string
    pgd::Vector3 lb, ub;
    ApplyDisplayTransformation(m_lowerBound, &lb);
    ApplyDisplayTransformation(m_upperBound, &ub);
    std::vector<char> buffer(512);
    size_t l = std::snprintf(buffer.data(), buffer.size(), "(%g,%g,%g),(%g,%g,%g)", lb.x, lb.y, lb.z, ub.x, ub.y, ub.z);
    std::string extent(buffer.data(), l);

    // now we need to split the mesh into triangles with the same colours
    pgd::Vector3 v;
    std::map<pgd::Vector3, std::vector<size_t>> colourMap;
    size_t numVertices =  m_vertexList.size() / 3;
    size_t numTriangles = numVertices / 3;
    for (size_t i = 0; i < numTriangles; i++)
    {
        v.x = m_colourList[i * 9];
        v.y = m_colourList[i * 9 + 1];
        v.z = m_colourList[i * 9 + 2];
        auto it = colourMap.find(v);
        if (it == colourMap.end()) { colourMap[v] = std::vector<size_t>(); colourMap[v].reserve(numTriangles); }
        colourMap[v].push_back(i * 3);
    }

    // output the materials
    std::vector<pgd::Vector3> colourList;
    colourList.reserve(colourMap.size());
    for (auto &&it : colourMap)
    {
        double r = m_blendColour.redF() * m_blendFraction + (1 - m_blendFraction) * it.first.x;
        double g = m_blendColour.greenF() * m_blendFraction + (1 - m_blendFraction) * it.first.y;
        double b = m_blendColour.blueF() * m_blendFraction + (1 - m_blendFraction) * it.first.z;
        size_t l = std::snprintf(buffer.data(), buffer.size(), "(%g,%g,%g)", r, g, b);
        std::string diffuseColor(buffer.data(), l);
        out << "def Material \"" << name << "_material_" << colourList.size() << "\"\n";
        out << "{\n";
        out << "    token outputs:surface.connect = </World/" << name << "_xform/" << name << "_material_" << colourList.size() << "/previewShader.outputs:surface>\n";
        out << "    def Shader \"previewShader\"\n";
        out << "    {\n";
        out << "        uniform token info:id = \"UsdPreviewSurface\"\n";
        out << "        color3f inputs:diffuseColor = " << diffuseColor << "\n";
        out << "        float inputs:metallic = 0.5\n";
        out << "        float inputs:roughness = 0.25\n";
        out << "        token outputs:surface\n";
        out << "    }\n";
        out << "}\n";
        colourList.push_back(it.first);
    }

    // output a separate mesh for each material
    size_t colourCount = 0;
    for (auto &&it : colourMap)
    {
        // set all the faceVertexCounts to 3
        std::vector<char> faceVertexCounts;
        faceVertexCounts.reserve(it.second.size() * 2);
        for (size_t i = 0; i < it.second.size(); i++)
        {
            faceVertexCounts.push_back('3');
            faceVertexCounts.push_back(',');
        }
        faceVertexCounts.back() = 0;

        // handle the individual triangles
        std::vector<char> faceVertexIndices;
        std::vector<char> normals;
        std::vector<char> points;
        faceVertexIndices.reserve(it.second.size() * 30);
        normals.reserve(it.second.size() * 150);
        points.reserve(it.second.size() * 150);
        pgd::Vector3 v1, v2;
        for (size_t i = 0; i < it.second.size(); i++)
        {
            std::snprintf(buffer.data(), buffer.size(), "%zu,%zu,%zu,", i * 3, i * 3 + 1, i * 3 + 2);
            for (char *ptr = buffer.data(); *ptr != 0; ptr++) { faceVertexIndices.push_back(*ptr); }

            for (size_t j = 0; j < 3; j++)
            {
                v1.x = m_vertexList[(it.second[i] + j) * 3]; // qDebug() << (it.second[i] + j) * 3;
                v1.y = m_vertexList[(it.second[i] + j) * 3 + 1]; // qDebug() << (it.second[i] + j) * 3 + 1;
                v1.z = m_vertexList[(it.second[i] + j) * 3 + 2]; // qDebug() << (it.second[i] + j) * 3 + 2;
                ApplyDisplayTransformation(v1, &v2);
                std::snprintf(buffer.data(), buffer.size(), "(%g,%g,%g),", v2.x, v2.y, v2.z);
                for (char *ptr = buffer.data(); *ptr != 0; ptr++) { points.push_back(*ptr); }

                v1.x = m_normalList[(it.second[i] + j) * 3];
                v1.y = m_normalList[(it.second[i] + j) * 3 + 1];
                v1.z = m_normalList[(it.second[i] + j) * 3 + 2];
                ApplyDisplayTransformation(v1, &v2);
                std::snprintf(buffer.data(), buffer.size(), "(%g,%g,%g),", v2.x, v2.y, v2.z);
                for (char *ptr = buffer.data(); *ptr != 0; ptr++) { normals.push_back(*ptr); }
            }
        }
        faceVertexIndices.back() = 0;
        points.back() = 0;
        normals.back() = 0;

        std::string_view faceVertexCountsView(faceVertexCounts.data(), faceVertexCounts.size() - 1);
        std::string_view faceVertexIndicesView(faceVertexIndices.data(),faceVertexIndices .size() - 1);
        std::string_view normalsView(normals.data(), normals.size() - 1);
        std::string_view pointsView(points.data(), points.size() - 1);

        out << "def Mesh \"" << name << "_mesh_" << colourCount << "\"\n";
        out << "{\n";
        out << "    float3[] extent = [" << extent << "]\n";
        out << "    int[] faceVertexCounts = [" << faceVertexCountsView << "]\n";
        out << "    int[] faceVertexIndices = [" << faceVertexIndicesView << "]\n";
        out << "    rel material:binding = </World/" << name << "_xform/" << name << "_material_" << colourCount << ">\n";
        out << "    normal3f[] normals = [" << normalsView << "] (\n";
        out << "        interpolation = \"faceVarying\"\n";
        out << "    )\n";
        out << "    point3f[] points = [" << pointsView << "]\n";
        out << "    uniform token subdivisionScheme = \"none\"\n";
        out << "}\n";

        colourCount++;
    }

    out << "}\n";
}


// utility to calculate a face normal
// this assumes anticlockwise winding
// for a triangle p1, p2, p3, if the vector A = p2 - p1 and the vector B = p3 - p1
// then the normal N = A x B and can be calculated by:
// this can be simplified to:
// Nx = Ay * Bz - Az * By
// Ny = Az * Bx - Ax * Bz
// Nz = Ax * By - Ay * Bx
void FacetedObject::ComputeFaceNormal(const double *v1, const double *v2, const double *v3, double normal[3])
{
    double a[3], b[3];

    // calculate in plane vectors
    a[0] = v2[0] - v1[0];
    a[1] = v2[1] - v1[1];
    a[2] = v2[2] - v1[2];
    b[0] = v3[0] - v1[0];
    b[1] = v3[1] - v1[1];
    b[2] = v3[2] - v1[2];

    // cross(a, b, normal);
    normal[0] = a[1] * b[2] - a[2] * b[1];
    normal[1] = a[2] * b[0] - a[0] * b[2];
    normal[2] = a[0] * b[1] - a[1] * b[0];

    // normalize(normal);
    double norm = sqrt(normal[0] * normal[0] +
                       normal[1] * normal[1] +
                       normal[2] * normal[2]);

    if (norm > 0.0)
    {
        normal[0] /= norm;
        normal[1] /= norm;
        normal[2] /= norm;
    }
}

// move the object
// note this must be used before the first Draw() call
void FacetedObject::Move(double x, double y, double z)
{
    if (x == 0.0 && y == 0.0 && z == 0.0) return;
    for (size_t i = 0; i < m_vertexList.size() / 3; i++)
    {
        m_vertexList[i * 3] += x;
        m_vertexList[i * 3 + 1] += y;
        m_vertexList[i * 3 + 2] += z;
    }
    m_lowerBound[0] += x;
    m_lowerBound[1] += y;
    m_lowerBound[2] += z;
    m_upperBound[0] += x;
    m_upperBound[1] += y;
    m_upperBound[2] += z;
}

// scale the object
// note this must be used before the first Draw() call
void FacetedObject::Scale(double x, double y, double z)
{
    if (x == 1.0 && y == 1.0 && z == 1.0) return;
    for (size_t i = 0; i < m_vertexList.size() / 3; i++)
    {
        m_vertexList[i * 3] *= x;
        m_vertexList[i * 3 + 1] *= y;
        m_vertexList[i * 3 + 2] *= z;
    }
    m_lowerBound[0] *= x;
    m_lowerBound[1] *= y;
    m_lowerBound[2] *= z;
    m_upperBound[0] *= x;
    m_upperBound[1] *= y;
    m_upperBound[2] *= z;
}

// rotate the object
// note this must be used before the first Draw() call
void FacetedObject::Rotate(double x, double y, double z, double angleDegrees)
{
    Q_ASSERT_X(x != 0 || y != 0 || z != 0, "Axis must be non-zero", "FacetedObject::Rotate");
    if (angleDegrees == 0) return;
    pgd::Quaternion q = pgd::MakeQFromAxisAngle(x, y, z, pgd::DegToRad(angleDegrees));
    pgd::Vector3 v;
    for (size_t i = 0; i < m_vertexList.size() / 3; i++)
    {
        v = pgd::QVRotate(q, pgd::Vector3(m_vertexList[i * 3], m_vertexList[i * 3 + 1], m_vertexList[i * 3 + 2]));
        m_vertexList[i * 3] = v.x;
        m_vertexList[i * 3 + 1] = v.y;
        m_vertexList[i * 3 + 2] = v.z;
    }
    v = pgd::QVRotate(q, m_lowerBound);
    m_lowerBound[0] = v.x;
    m_lowerBound[1] = v.y;
    m_lowerBound[2] = v.z;
    v = pgd::QVRotate(q, m_upperBound);
    m_upperBound[0] = v.x;
    m_upperBound[1] = v.y;
    m_upperBound[2] = v.z;
}


// this routine triangulates the polygon and calls AddTriangle to do the actual data adding
// vertices are a packed list of floating point numbers
// x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4
void FacetedObject::AddPolygon(const double *vertices, size_t nSides, const double *normals, const double *UVs)
{
    if (UVs == nullptr)
    {
        if (normals == nullptr)
        {
            // calculate the normals
            double triNormal[9];
            ComputeFaceNormal(vertices, vertices + 3, vertices + 6, triNormal);
            for (size_t i = 3; i < 9; i++) triNormal[i] = triNormal[i % 3];
            // add faces as triangles
            double triangle[9];
            triangle[0] = vertices[0];
            triangle[1] = vertices[1];
            triangle[2] = vertices[2];
            for (size_t j = 2; j < nSides; j++)
            {
                triangle[3] = vertices[(j - 1) * 3];
                triangle[4] = vertices[(j - 1) * 3 + 1];
                triangle[5] = vertices[(j - 1) * 3 + 2];
                triangle[6] = vertices[(j * 3)];
                triangle[7] = vertices[(j * 3) + 1];
                triangle[8] = vertices[(j * 3) + 2];
                AddTriangle(triangle, triNormal, nullptr);
            }
        }
        else
        {
            // add faces as triangles
            double triangle[9];
            double triNormal[9];
            triangle[0] = vertices[0];
            triangle[1] = vertices[1];
            triangle[2] = vertices[2];
            triNormal[0] = normals[0];
            triNormal[1] = normals[1];
            triNormal[2] = normals[2];
            for (size_t j = 2; j < nSides; j++)
            {
                triangle[3] = vertices[(j - 1) * 3];
                triangle[4] = vertices[(j - 1) * 3 + 1];
                triangle[5] = vertices[(j - 1) * 3 + 2];
                triangle[6] = vertices[(j * 3)];
                triangle[7] = vertices[(j * 3) + 1];
                triangle[8] = vertices[(j * 3) + 2];
                triNormal[3] = normals[(j - 1) * 3];
                triNormal[4] = normals[(j - 1) * 3 + 1];
                triNormal[5] = normals[(j - 1) * 3 + 2];
                triNormal[6] = normals[(j * 3)];
                triNormal[7] = normals[(j * 3) + 1];
                triNormal[8] = normals[(j * 3) + 2];
                AddTriangle(triangle, triNormal, nullptr);
            }
        }
    }
    else
    {
        if (normals == nullptr)
        {
            // calculate the normals
            double triNormal[9];
            double triUVs[6];
            ComputeFaceNormal(vertices, vertices + 3, vertices + 6, triNormal);
            for (size_t i = 3; i < 9; i++) triNormal[i] = triNormal[i % 3];
            // add faces as triangles
            double triangle[9];
            triangle[0] = vertices[0];
            triangle[1] = vertices[1];
            triangle[2] = vertices[2];
            triUVs[0] = UVs[0];
            triUVs[1] = UVs[1];
            for (size_t j = 2; j < nSides; j++)
            {
                triangle[3] = vertices[(j - 1) * 3];
                triangle[4] = vertices[(j - 1) * 3 + 1];
                triangle[5] = vertices[(j - 1) * 3 + 2];
                triangle[6] = vertices[(j * 3)];
                triangle[7] = vertices[(j * 3) + 1];
                triangle[8] = vertices[(j * 3) + 2];
                triUVs[2] = UVs[(j - 1) * 2];
                triUVs[3] = UVs[(j - 1) * 2 + 1];
                triUVs[4] = UVs[(j * 2)];
                triUVs[5] = UVs[(j * 2) + 1];
                AddTriangle(triangle, triNormal, triUVs);
            }
        }
        else
        {
            // add faces as triangles
            double triangle[9];
            double triNormal[9];
            double triUVs[6];
            triangle[0] = vertices[0];
            triangle[1] = vertices[1];
            triangle[2] = vertices[2];
            triNormal[0] = normals[0];
            triNormal[1] = normals[1];
            triNormal[2] = normals[2];
            triUVs[0] = UVs[0];
            triUVs[1] = UVs[1];
            for (size_t j = 2; j < nSides; j++)
            {
                triangle[3] = vertices[(j - 1) * 3];
                triangle[4] = vertices[(j - 1) * 3 + 1];
                triangle[5] = vertices[(j - 1) * 3 + 2];
                triangle[6] = vertices[(j * 3)];
                triangle[7] = vertices[(j * 3) + 1];
                triangle[8] = vertices[(j * 3) + 2];
                triNormal[3] = normals[(j - 1) * 3];
                triNormal[4] = normals[(j - 1) * 3 + 1];
                triNormal[5] = normals[(j - 1) * 3 + 2];
                triNormal[6] = normals[(j * 3)];
                triNormal[7] = normals[(j * 3) + 1];
                triNormal[8] = normals[(j * 3) + 2];
                triUVs[2] = UVs[(j - 1) * 2];
                triUVs[3] = UVs[(j - 1) * 2 + 1];
                triUVs[4] = UVs[(j * 2)];
                triUVs[5] = UVs[(j * 2) + 1];
                AddTriangle(triangle, triNormal, triUVs);
            }
        }
    }
}


// this is the only Add* routine that actually adds data to the faceted object
// except for AddFacetedObject with the useDirectAccess option
// it gets called by add polygon
// vertices is a packed list of floating point numbers
// x1, y1, z1, x2, y2, z2, x3, y3, z3
void FacetedObject::AddTriangle(const double *vertices, const double *normals, const double *UVs)
{
    Q_ASSERT_X(m_vertexList.capacity() - m_vertexList.size() >= 9, "FacetedObject::AddTriangle", "Warning: not enough triangle space reserved");
    pgd::Vector3 vertex;
    for (size_t i = 0; i < 3; i++)
    {
        vertex.x = vertices[i * 3 + 0];
        vertex.y = vertices[i * 3 + 1];
        vertex.z = vertices[i * 3 + 2];
        m_vertexList.push_back(vertex.x);
        m_vertexList.push_back(vertex.y);
        m_vertexList.push_back(vertex.z);
        if (vertex.x < m_lowerBound[0]) m_lowerBound[0] = vertex.x;
        if (vertex.y < m_lowerBound[1]) m_lowerBound[1] = vertex.y;
        if (vertex.z < m_lowerBound[2]) m_lowerBound[2] = vertex.z;
        if (vertex.x > m_upperBound[0]) m_upperBound[0] = vertex.x;
        if (vertex.y > m_upperBound[1]) m_upperBound[1] = vertex.y;
        if (vertex.z > m_upperBound[2]) m_upperBound[2] = vertex.z;
    }
    if (normals)
    {
        for (size_t i = 0; i < 9; i++) m_normalList.push_back(normals[i]);
    }
    else
    {
        // calculate the normals
        double normal[3];
        ComputeFaceNormal(vertices, vertices + 3, vertices + 6, normal);
        for (size_t i = 0; i < 9; i++) m_normalList.push_back(normal[i % 3]);
    }
    if (UVs)
    {
        for (size_t i = 0; i < 6; i++) m_uvList.push_back(UVs[i]);
    }
    else
    {
        for (size_t i = 0; i < 6; i++) m_uvList.push_back(0);
    }
    double colour[3] = { m_blendColour.redF(), m_blendColour.greenF(), m_blendColour.blueF() };
    for (size_t i = 0; i < 9; i++) m_colourList.push_back(colour[i % 3]);
}

// this routine triangulates the polygon and calls AddTriangle to do the actual data adding
// vertices are a packed list of floating point numbers
// x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4
void FacetedObject::AddPolygon(const float *floatVertices, size_t nSides, const float *floatNormals, const float *floatUVs)
{
    if (floatUVs == nullptr)
    {
        size_t nVertices = nSides * 3;
        std::vector<double> doubleVertices;
        doubleVertices.reserve(nVertices);
        for (size_t i = 0; i < nVertices; i++) doubleVertices.push_back(double(floatVertices[i]));
        if (floatNormals)
        {
            std::vector<double> normals;
            normals.reserve(nVertices);
            for (size_t i = 0; i < nVertices; i++) normals.push_back(double(floatNormals[i]));
            AddPolygon(doubleVertices.data(), nSides, normals.data(), nullptr);
        }
        else
        {
            AddPolygon(doubleVertices.data(), nSides, nullptr, nullptr);
        }
    }
    else
    {
        size_t nUVs = nSides * 2;
        std::vector<double> doubleUVs;
        doubleUVs.reserve(nUVs);
        for (size_t i = 0; i < nUVs; i++) doubleUVs.push_back(double(floatUVs[i]));
        size_t nVertices = nSides * 3;
        std::vector<double> doubleVertices;
        doubleVertices.reserve(nVertices);
        for (size_t i = 0; i < nVertices; i++) doubleVertices.push_back(double(floatVertices[i]));
        if (floatNormals)
        {
            std::vector<double> normals;
            normals.reserve(nVertices);
            for (size_t i = 0; i < nVertices; i++) normals.push_back(double(floatNormals[i]));
            AddPolygon(doubleVertices.data(), nSides, normals.data(), doubleUVs.data());
        }
        else
        {
            AddPolygon(doubleVertices.data(), nSides, nullptr, doubleUVs.data());
        }
    }
}


// this is just a convenience function to allow AddTraingles to be called with floats rather than doubles
// vertices is a packed list of floating point numbers
// x1, y1, z1, x2, y2, z2, x3, y3, z3
void FacetedObject::AddTriangle(const float *floatVertices, const float *floatNormals, const float *floatUVs)
{
    if (floatUVs == nullptr)
    {
        double vertices[9];
        for (int i = 0; i < 9; i++) vertices[i] = double(floatVertices[i]);
        if (floatNormals)
        {
            double normals[9];
            for (int i = 0; i < 9; i++) normals[i] = double(floatNormals[i]);
            AddTriangle(vertices, normals, nullptr);
        }
        else
        {
            AddTriangle(vertices, nullptr, nullptr);
        }
    }
    else
    {
        double UVs[6];
        for (int i = 0; i < 6; i++) UVs[i] = double(floatUVs[i]);
        double vertices[9];
        for (int i = 0; i < 9; i++) vertices[i] = double(floatVertices[i]);
        if (floatNormals)
        {
            double normals[9];
            for (int i = 0; i < 9; i++) normals[i] = double(floatNormals[i]);
            AddTriangle(vertices, normals, UVs);
        }
        else
        {
            AddTriangle(vertices, nullptr, UVs);
        }
    }
}

// this routine handles the memory allocation
// allocation is the number of vertices to store
void FacetedObject::AllocateMemory(size_t numTriangles)
{
//    qDebug() << "Allocated " << numTriangles << " triangles\n";
    m_vertexList.reserve(numTriangles * 9);
    m_normalList.reserve(numTriangles * 9);
    m_colourList.reserve(numTriangles * 9);
    m_uvList.reserve(numTriangles * 6);
}

// calculate mass properties
// based on dMassSetTrimesh
/*
 * dMassSetTrimesh, implementation by Gero Mueller.
 * Based on Brian Mirtich, "Fast and Accurate Computation of
 * Polyhedral Mass Properties," journal of graphics tools, volume 1,
 * number 2, 1996.
 */

#define SQR(x)          ((x)*(x))                       //!< Returns x square
#define CUBE(x)         ((x)*(x)*(x))                   //!< Returns x cube

// the mass properties are calculated around the world origin
// these can be moved to around the centroid if wanted using the parallel axes rules

void FacetedObject::CalculateMassProperties(double density, bool clockwise, const pgd::Vector3 &translation, double *mass, pgd::Vector3 *centreOfMass, pgd::Matrix3x3 *inertialTensor)
{
    // assumes anticlockwise winding unless clockwise is set
    unsigned int triangles = static_cast<unsigned int>((m_vertexList.size() / 3) / 3);

    double nx, ny, nz;
    unsigned int i, j, A, B, C;
    // face integrals
    double Fa, Fb, Fc, Faa, Fbb, Fcc, Faaa, Fbbb, Fccc, Faab, Fbbc, Fcca;

    // projection integrals
    double P1, Pa, Pb, Paa, Pab, Pbb, Paaa, Paab, Pabb, Pbbb;

    double T0 = 0;
    double T1[3] = {0., 0., 0.};
    double T2[3] = {0., 0., 0.};
    double TP[3] = {0., 0., 0.};

    pgd::Vector3 v[3];
    for ( i = 0; i < triangles; i++ )
    {
        if (clockwise == false)
        {
            for (j = 0; j < 3; j++)
            {
                v[j][0] = m_vertexList[i * 9 + j * 3] + translation.x;
                v[j][1] = m_vertexList[i * 9 + j * 3 + 1] + translation.y;
                v[j][2] = m_vertexList[i * 9 + j * 3 + 2] + translation.z;
            }
        }
        else
        {
            for (j = 0; j < 3; j++)
            {
                v[j][2] = m_vertexList[i * 9 + j * 3] + translation.x;
                v[j][1] = m_vertexList[i * 9 + j * 3 + 1] + translation.y;
                v[j][0] = m_vertexList[i * 9 + j * 3 + 2] + translation.z;
            }
        }

        pgd::Vector3 n, a, b;
        a = v[1] - v[0];
        b = v[2] - v[0];
        n = pgd::Cross(b, a);
        nx = std::fabs(n[0]);
        ny = std::fabs(n[1]);
        nz = std::fabs(n[2]);

        if ( nx > ny && nx > nz )
            C = 0;
        else
            C = (ny > nz) ? 1 : 2;

        // Even though all triangles might be initially valid,
        // a triangle may degenerate into a segment after applying
        // space transformation.
        if (n[C] != 0.0)
        {
            A = (C + 1) % 3;
            B = (A + 1) % 3;

            // calculate face integrals
            {
                double w;
                double k1, k2, k3, k4;

                //compProjectionIntegrals(f);
                {
                    double a0 = 0.0, a1 = 0.0, da;
                    double b0 = 0.0, b1 = 0.0, db;
                    double a0_2, a0_3, a0_4, b0_2, b0_3, b0_4;
                    double a1_2, a1_3, b1_2, b1_3;
                    double C1, Ca, Caa, Caaa, Cb, Cbb, Cbbb;
                    double Cab, Kab, Caab, Kaab, Cabb, Kabb;

                    P1 = Pa = Pb = Paa = Pab = Pbb = Paaa = Paab = Pabb = Pbbb = 0.0;

                    for ( j = 0; j < 3; j++)
                    {
                        switch (j)
                        {
                        case 0:
                            a0 = v[0][A];
                            b0 = v[0][B];
                            a1 = v[1][A];
                            b1 = v[1][B];
                            break;
                        case 1:
                            a0 = v[1][A];
                            b0 = v[1][B];
                            a1 = v[2][A];
                            b1 = v[2][B];
                            break;
                        case 2:
                            a0 = v[2][A];
                            b0 = v[2][B];
                            a1 = v[0][A];
                            b1 = v[0][B];
                            break;
                        }
                        da = a1 - a0;
                        db = b1 - b0;
                        a0_2 = a0 * a0;
                        a0_3 = a0_2 * a0;
                        a0_4 = a0_3 * a0;
                        b0_2 = b0 * b0;
                        b0_3 = b0_2 * b0;
                        b0_4 = b0_3 * b0;
                        a1_2 = a1 * a1;
                        a1_3 = a1_2 * a1;
                        b1_2 = b1 * b1;
                        b1_3 = b1_2 * b1;

                        C1 = a1 + a0;
                        Ca = a1 * C1 + a0_2;
                        Caa = a1 * Ca + a0_3;
                        Caaa = a1 * Caa + a0_4;
                        Cb = b1 * (b1 + b0) + b0_2;
                        Cbb = b1 * Cb + b0_3;
                        Cbbb = b1 * Cbb + b0_4;
                        Cab = 3 * a1_2 + 2 * a1 * a0 + a0_2;
                        Kab = a1_2 + 2 * a1 * a0 + 3 * a0_2;
                        Caab = a0 * Cab + 4 * a1_3;
                        Kaab = a1 * Kab + 4 * a0_3;
                        Cabb = 4 * b1_3 + 3 * b1_2 * b0 + 2 * b1 * b0_2 + b0_3;
                        Kabb = b1_3 + 2 * b1_2 * b0 + 3 * b1 * b0_2 + 4 * b0_3;

                        P1 += db * C1;
                        Pa += db * Ca;
                        Paa += db * Caa;
                        Paaa += db * Caaa;
                        Pb += da * Cb;
                        Pbb += da * Cbb;
                        Pbbb += da * Cbbb;
                        Pab += db * (b1 * Cab + b0 * Kab);
                        Paab += db * (b1 * Caab + b0 * Kaab);
                        Pabb += da * (a1 * Cabb + a0 * Kabb);
                    }

                    P1 /= 2.0;
                    Pa /= 6.0;
                    Paa /= 12.0;
                    Paaa /= 20.0;
                    Pb /= -6.0;
                    Pbb /= -12.0;
                    Pbbb /= -20.0;
                    Pab /= 24.0;
                    Paab /= 60.0;
                    Pabb /= -60.0;
                }

                w = - pgd::Dot(n, v[0]);

                k1 = 1 / n[C];
                k2 = k1 * k1;
                k3 = k2 * k1;
                k4 = k3 * k1;

                Fa = k1 * Pa;
                Fb = k1 * Pb;
                Fc = -k2 * (n[A] * Pa + n[B] * Pb + w * P1);

                Faa = k1 * Paa;
                Fbb = k1 * Pbb;
                Fcc = k3 * (SQR(n[A]) * Paa + 2 * n[A] * n[B] * Pab + SQR(n[B]) * Pbb +
                            w * (2 * (n[A] * Pa + n[B] * Pb) + w * P1));

                Faaa = k1 * Paaa;
                Fbbb = k1 * Pbbb;
                Fccc = -k4 * (CUBE(n[A]) * Paaa + 3 * SQR(n[A]) * n[B] * Paab
                              + 3 * n[A] * SQR(n[B]) * Pabb + CUBE(n[B]) * Pbbb
                              + 3 * w * (SQR(n[A]) * Paa + 2 * n[A] * n[B] * Pab + SQR(n[B]) * Pbb)
                              + w * w * (3 * (n[A] * Pa + n[B] * Pb) + w * P1));

                Faab = k1 * Paab;
                Fbbc = -k2 * (n[A] * Pabb + n[B] * Pbbb + w * Pbb);
                Fcca = k3 * (SQR(n[A]) * Paaa + 2 * n[A] * n[B] * Paab + SQR(n[B]) * Pabb
                             + w * (2 * (n[A] * Paa + n[B] * Pab) + w * Pa));
            }


            T0 += n[0] * ((A == 0) ? Fa : ((B == 0) ? Fb : Fc));

            T1[A] += n[A] * Faa;
            T1[B] += n[B] * Fbb;
            T1[C] += n[C] * Fcc;
            T2[A] += n[A] * Faaa;
            T2[B] += n[B] * Fbbb;
            T2[C] += n[C] * Fccc;
            TP[A] += n[A] * Faab;
            TP[B] += n[B] * Fbbc;
            TP[C] += n[C] * Fcca;
        }
    }

    T1[0] /= 2;
    T1[1] /= 2;
    T1[2] /= 2;
    T2[0] /= 3;
    T2[1] /= 3;
    T2[2] /= 3;
    TP[0] /= 2;
    TP[1] /= 2;
    TP[2] /= 2;

    *mass = density * T0;
// #define _I(i,j) I[(i)*4+(j)]
// regex _I\(([0-9]+),([0-9]+)\) to I[(\1)*4+(\2)]
//    m->_I(0,0) = density * (T2[1] + T2[2]);
//    m->_I(1,1) = density * (T2[2] + T2[0]);
//    m->_I(2,2) = density * (T2[0] + T2[1]);
//    m->_I(0,1) = - density * TP[0];
//    m->_I(1,0) = - density * TP[0];
//    m->_I(2,1) = - density * TP[1];
//    m->_I(1,2) = - density * TP[1];
//    m->_I(2,0) = - density * TP[2];
//    m->_I(0,2) = - density * TP[2];
    inertialTensor->Set( density * (T2[1] + T2[2]), -density * TP[0],           -density * TP[2],
                        -density * TP[0],            density * (T2[2] + T2[0]), -density * TP[1],
                        -density * TP[2],           -density * TP[1],            density * (T2[0] + T2[1]));
    centreOfMass->x = T1[0] / T0;
    centreOfMass->y = T1[1] / T0;
    centreOfMass->z = T1[2] / T0;
}

// reverse the face winding
void FacetedObject::ReverseWinding()
{
    double t;
    size_t numTriangles = (m_vertexList.size() / 3) / 3;
    size_t i, j;
    for (i = 0; i < numTriangles; i++)
    {
        for (j = 0; j < 3; j++)
        {
            t = m_vertexList[i * 9 + 3 + j];
            m_vertexList[i * 9 + 3 + j] = m_vertexList[i * 9 + 6 + j];
            m_vertexList[i * 9 + 6 + j] = t;
        }
    }
    for (i = 0; i < m_normalList.size(); i++) m_normalList[i] = -m_normalList[i];
}

// add the faces from one faceted object to another
// useDirectAccess version does not use AddTriangle
// there is probably no good reason currently not to use useDirectAccess
void FacetedObject::AddFacetedObject(const FacetedObject *object, bool useDisplayRotation, bool useDirectAccess)
{
    if (useDirectAccess)
    {
        size_t offset = m_vertexList.size();
        m_vertexList.resize(offset + object->m_vertexList.size());
        m_normalList.resize(offset + object->m_normalList.size());
        m_colourList.resize(offset + object->m_colourList.size());
        size_t offsetUV = m_uvList.size();
        m_uvList.resize(offsetUV + object->m_uvList.size());
        if (useDisplayRotation)
        {
            for (size_t i = 0; i < object->m_vertexList.size(); i += 3)
            {
                ApplyDisplayTransformation(&object->m_vertexList[i], reinterpret_cast<pgd::Vector3 *>(&m_vertexList[i + offset]));
                ApplyDisplayRotation(&object->m_normalList[i], reinterpret_cast<pgd::Vector3 *>(&m_normalList[i + offset]));
            }
            for (size_t i = 0; i < object->m_colourList.size(); i++) m_colourList[i + offset] = object->m_colourList[i];
            for (size_t i = 0; i < object->m_uvList.size(); i++) m_uvList[i + offsetUV] = object->m_uvList[i];
            pgd::Vector3 lower, upper;
            ApplyDisplayTransformation(m_lowerBound, &lower);
            ApplyDisplayTransformation(m_upperBound, &upper);
            if (lower.x < m_lowerBound[0]) m_lowerBound[0] = lower.x;
            if (lower.y < m_lowerBound[1]) m_lowerBound[1] = lower.y;
            if (lower.z < m_lowerBound[2]) m_lowerBound[2] = lower.z;
            if (upper.x > m_upperBound[0]) m_upperBound[0] = upper.x;
            if (upper.y > m_upperBound[1]) m_upperBound[1] = upper.y;
            if (upper.z > m_upperBound[2]) m_upperBound[2] = upper.z;
        }
        else
        {
            for (size_t i = 0; i < object->m_vertexList.size(); i++)
            {
                m_vertexList[i + offset] = object->m_vertexList[i];
                m_normalList[i + offset] = object->m_normalList[i];
                m_colourList[i + offset] = object->m_colourList[i];
            }
            for (size_t i = 0; i < object->m_uvList.size(); i++) m_uvList[i + offsetUV] = object->m_uvList[i];
            if (object->m_lowerBound[0] < m_lowerBound[0]) m_lowerBound[0] = object->m_lowerBound[0];
            if (object->m_lowerBound[1] < m_lowerBound[1]) m_lowerBound[1] = object->m_lowerBound[1];
            if (object->m_lowerBound[2] < m_lowerBound[2]) m_lowerBound[2] = object->m_lowerBound[2];
            if (object->m_upperBound[0] > m_upperBound[0]) m_upperBound[0] = object->m_upperBound[0];
            if (object->m_upperBound[1] > m_upperBound[1]) m_upperBound[1] = object->m_upperBound[1];
            if (object->m_upperBound[2] > m_upperBound[2]) m_upperBound[2] = object->m_upperBound[2];
        }
    }
    else
    {
        size_t numTriangles = object->GetNumTriangles();
        const double *triangle;
        const double *p1;
        double *p2;
        double triangle2[9];
        pgd::Vector3 v1, v1r;

        if (useDisplayRotation)
        {
            for (size_t i = 0; i < numTriangles; i++)
            {
                triangle = object->GetTriangle(i);
                for (int j = 0; j < 3; j++)
                {
                    p1 = triangle + 3 * j;
                    v1[0] = p1[0];
                    v1[1] = p1[1];
                    v1[2] = p1[2];
                    v1r = m_displayRotation * v1;
                    p2 = triangle2 + 3 * j;
                    p2[0] = v1r[0] + m_displayPosition[0];
                    p2[1] = v1r[1] + m_displayPosition[1];
                    p2[2] = v1r[2] + m_displayPosition[2];
                }
                AddTriangle(triangle2);
            }
        }
        else
        {
            for (size_t i = 0; i < numTriangles; i++)
            {
                triangle = object->GetTriangle(i);
                AddTriangle(triangle);
            }
        }
    }
}

// does a raw append of the data stores from elsewhere
// this can be very fragile if the sizes of the lists are not right
// the vertexList is 9 times the number of triangles
// the normalList is 9 times the number of triangles
// the colourList is 9 times the number of triangles
// the uvList is 6 times the number of triangles
void FacetedObject::RawAppend(const std::vector<double> *vertexList, const std::vector<double> *normalList, const std::vector<float> *colourList, const std::vector<double> *uvList)
{
    if (vertexList)
    {
        m_vertexList.reserve(m_vertexList.size() + vertexList->size());
        m_vertexList.insert(m_vertexList.end(), vertexList->begin(), vertexList->end());
    }
    if (normalList)
    {
        m_normalList.reserve(m_normalList.size() + normalList->size());
        m_normalList.insert(m_normalList.end(), normalList->begin(), normalList->end());
    }
    if (colourList)
    {
        m_colourList.reserve(m_colourList.size() + colourList->size());
        m_colourList.insert(m_colourList.end(), colourList->begin(), colourList->end());
    }
    if (uvList)
    {
        m_uvList.reserve(m_uvList.size() + uvList->size());
        m_uvList.insert(m_uvList.end(), uvList->begin(), uvList->end());
    }
    UpdateBoundingBox();
}

void FacetedObject::RawAppend(const std::vector<std::array<double, 3>> *vertexList, const std::vector<std::array<double, 3>> *normalList, const std::vector<std::array<float, 3>> *colourList, const std::vector<std::array<double, 2>> *uvList)
{
    if (vertexList)
    {
        m_vertexList.reserve(m_vertexList.size() + vertexList->size() * 3);
        for (auto &&i : *vertexList) { for (auto &&j : i) { m_vertexList.push_back(j); } }
    }
    if (normalList)
    {
        m_normalList.reserve(m_normalList.size() + normalList->size() * 3);
        for (auto &&i : *normalList) { for (auto &&j : i) { m_normalList.push_back(j); } }
    }
    if (colourList)
    {
        m_colourList.reserve(m_colourList.size() + colourList->size() * 3);
        for (auto &&i : *colourList) { for (auto &&j : i) { m_colourList.push_back(j); } }
    }
    if (uvList)
    {
        m_uvList.reserve(m_uvList.size() + uvList->size() * 2);
        for (auto &&i : *uvList) { for (auto &&j : i) { m_uvList.push_back(j); } }
    }
    UpdateBoundingBox();
}

void FacetedObject::RawAppend(const std::vector<std::array<double, 3>> *vertexList, const std::vector<std::array<double, 3>> *normalList, const std::vector<std::array<float, 4>> *colourList, const std::vector<std::array<double, 2>> *uvList)
{
    if (vertexList)
    {
        m_vertexList.reserve(m_vertexList.size() + vertexList->size() * 3);
        for (auto &&i : *vertexList) { for (auto &&j : i) { m_vertexList.push_back(j); } }
    }
    if (normalList)
    {
        m_normalList.reserve(m_normalList.size() + normalList->size() * 3);
        for (auto &&i : *normalList) { for (auto &&j : i) { m_normalList.push_back(j); } }
    }
    if (colourList)
    {
        m_colourList.reserve(m_colourList.size() + colourList->size() * 3);
        for (auto &&i : *colourList) { for (size_t j = 0; j < i.size() - 1; j++) { m_colourList.push_back(i[j]); } }
    }
    if (uvList)
    {
        m_uvList.reserve(m_uvList.size() + uvList->size() * 2);
        for (auto &&i : *uvList) { for (auto &&j : i) { m_uvList.push_back(j); } }
    }
    UpdateBoundingBox();
}

void FacetedObject::UpdateBoundingBox()
{
    m_lowerBound = {std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    m_upperBound = {-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()};
    double *ptr = m_vertexList.data();
    for (size_t i = 0; i < m_vertexList.size(); i += 3)
    {
        if (*ptr < m_lowerBound.x) m_lowerBound.x = *ptr;
        if (*ptr > m_upperBound.x) m_upperBound.x = *ptr;
        ++ptr;
        if (*ptr < m_lowerBound.y) m_lowerBound.y = *ptr;
        if (*ptr > m_upperBound.y) m_upperBound.y = *ptr;
        ++ptr;
        if (*ptr < m_lowerBound.z) m_lowerBound.z = *ptr;
        if (*ptr > m_upperBound.z) m_upperBound.z = *ptr;
        ++ptr;
    }
}

// modified from
// Möller–Trumbore intersection algorithm
// From Wikipedia, the free encyclopedia
// The Möller–Trumbore ray-triangle intersection algorithm, named after its inventors Tomas Möller and Ben Trumbore,
// is a fast method for calculating the intersection of a ray and a triangle in three dimensions without needing
// precomputation of the plane equation of the plane containing the triangle.

bool FacetedObject::RayIntersectsTriangle(const pgd::Vector3 &rayOrigin,
                                          const pgd::Vector3 &rayVector,
                                          const pgd::Vector3 &vertex0,
                                          const pgd::Vector3 &vertex1,
                                          const pgd::Vector3 &vertex2,
                                          pgd::Vector3 *outIntersectionPoint)
{
    const double EPSILON = std::numeric_limits<double>::epsilon();
    pgd::Vector3 edge1, edge2, h, s, q;
    double a,f,u,v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = rayVector.Cross(edge2);
    a = edge1.Dot(h);
    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.
    f = 1.0/a;
    s = rayOrigin - vertex0;
    u = f * s.Dot(h);
    if (u < 0.0 || u > 1.0)
        return false;
    q = s.Cross(edge1);
    v = f * rayVector.Dot(q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    double t = f * edge2.Dot(q);
    if (t > EPSILON && t < 1/EPSILON) // ray intersection
    {
        *outIntersectionPoint = rayOrigin + rayVector * t;
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

// modified from
// Fast Ray-Box Intersection
// by Andrew Woo
// from "Graphics Gems", Academic Press, 1990

#define HITBOUNDINBOX_NUMDIM  3
bool FacetedObject::HitBoundingBox(const double minB[HITBOUNDINBOX_NUMDIM], const double maxB[HITBOUNDINBOX_NUMDIM],      /*box */
                                   const double rayOrigin[HITBOUNDINBOX_NUMDIM], const double rayVector[HITBOUNDINBOX_NUMDIM],     /*ray */
                                   double coord[HITBOUNDINBOX_NUMDIM])                                                    /* hit point */
{
    bool inside = true;
    char quadrant[HITBOUNDINBOX_NUMDIM];
    int i;
    int whichPlane;
    double maxT[HITBOUNDINBOX_NUMDIM];
    double candidatePlane[HITBOUNDINBOX_NUMDIM];
    const char RIGHT = 0;
    const char LEFT = 1;
    const char MIDDLE = 2;

    /* Find candidate planes; this loop can be avoided if
    rays cast all from the eye (assume perpsective view) */
    for (i = 0; i < HITBOUNDINBOX_NUMDIM; i++)
    {
        if (rayOrigin[i] < minB[i])
        {
            quadrant[i] = LEFT;
            candidatePlane[i] = minB[i];
            inside = false;
        }
        else if (rayOrigin[i] > maxB[i])
        {
            quadrant[i] = RIGHT;
            candidatePlane[i] = maxB[i];
            inside = false;
        }
        else
        {
            quadrant[i] = MIDDLE;
        }
    }

    /* Ray origin inside bounding box */
    if (inside)
    {
        std::copy(rayOrigin, rayOrigin + HITBOUNDINBOX_NUMDIM, coord);
        return (true);
    }


    /* Calculate T distances to candidate planes */
    for (i = 0; i < HITBOUNDINBOX_NUMDIM; i++)
    {
        if (quadrant[i] != MIDDLE && rayVector[i] !=0.)
        {
            maxT[i] = (candidatePlane[i] - rayOrigin[i]) / rayVector[i];
        }
        else
        {
            maxT[i] = -1.;
        }
    }

    /* Get largest of the maxT's for final choice of intersection */
    whichPlane = 0;
    for (i = 1; i < HITBOUNDINBOX_NUMDIM; i++)
    {
        if (maxT[whichPlane] < maxT[i]) whichPlane = i;
    }

    /* Check final candidate actually inside box */
    if (maxT[whichPlane] < 0.) return (false);
    for (i = 0; i < HITBOUNDINBOX_NUMDIM; i++)
    {
        if (whichPlane != i)
        {
            coord[i] = rayOrigin[i] + maxT[whichPlane] * rayVector[i];
            if (coord[i] < minB[i] || coord[i] > maxB[i]) return (false);
        }
        else
        {
            coord[i] = candidatePlane[i];
        }
    }

    return (true);              /* ray hits box */
}

// this routine works in model coordinates and rayVector must be unit length
int FacetedObject::FindIntersection(const pgd::Vector3 &rayOrigin, const pgd::Vector3 &rayVector, std::vector<pgd::Vector3> *intersectionCoordList, std::vector<size_t> *intersectionIndexList) const
{
    if (!m_visible || !m_vertexList.size()) return 0;

    // first check bounding box
    double coord[3];
    bool bbHit = HitBoundingBox(m_lowerBound.constData(), m_upperBound.constData(), rayOrigin.constData(), rayVector.constData(), coord);
    if (!bbHit) return 0;

    // now loop through the triangles
    bool triHit;
    int hitCount = 0;
    pgd::Vector3 outIntersectionPoint;
    for (size_t i = 0; i < m_vertexList.size(); i += 9)
    {
#ifdef PRECHECK_BOUNDINGBOX
        double lowerBound[3] = {std::min({m_vertexList[i], m_vertexList[i + 3], m_vertexList[i + 6]}), std::min({m_vertexList[i + 1], m_vertexList[i + 4], m_vertexList[i + 7]}), std::min({m_vertexList[i + 2], m_vertexList[i + 5], m_vertexList[i + 8]})};
        double upperBound[3] = {std::max({m_vertexList[i], m_vertexList[i + 3], m_vertexList[i + 6]}), std::max({m_vertexList[i + 1], m_vertexList[i + 4], m_vertexList[i + 7]}), std::max({m_vertexList[i + 2], m_vertexList[i + 5], m_vertexList[i + 8]})};
        bbHit = HitBoundingBox(lowerBound, upperBound, rayOrigin.constData(), rayVector.constData(), coord);
        if (bbHit)
        {
            triHit = RayIntersectsTriangle(rayOrigin, rayVectorNorm, &m_vertexList[i], &m_vertexList[i + 3], &m_vertexList[i + 6], &outIntersectionPoint);
            if (triHit)
            {
                hitCount++;;
                if (intersectionCoordList) intersectionCoordList->push_back(outIntersectionPoint);
                if (intersectionIndexList) intersectionIndexList->push_back(i);
            }
        }
#else
        triHit = RayIntersectsTriangle(rayOrigin, rayVector, &m_vertexList[i], &m_vertexList[i + 3], &m_vertexList[i + 6], &outIntersectionPoint);
        if (triHit)
        {
            hitCount++;
            if (intersectionCoordList) intersectionCoordList->push_back(outIntersectionPoint);
            if (intersectionIndexList) intersectionIndexList->push_back(i);
        }
#endif

    }
    return hitCount;
}

void FacetedObject::ApplyDisplayTransformation(const pgd::Vector3 &inVec, pgd::Vector3 *outVec)
{
    pgd::Vector3 scaled(inVec.x * m_displayScale[0], inVec.y * m_displayScale[1], inVec.z * m_displayScale[2]);
    *outVec = (m_displayRotation * scaled) + m_displayPosition;
}

void FacetedObject::ApplyDisplayRotation(const pgd::Vector3 &inVec, pgd::Vector3 *outVec)
{
    *outVec = m_displayRotation * inVec;
}

void FacetedObject::SetDisplayPosition(double x, double y, double z)
{
    m_displayPosition[0] = x;
    m_displayPosition[1] = y;
    m_displayPosition[2] = z;
    m_modelValid = false;
}

void FacetedObject::SetDisplayScale(double x, double y, double z)
{
    m_displayScale[0] = x;
    m_displayScale[1] = y;
    m_displayScale[2] = z;
    m_modelValid = false;
}

void FacetedObject::SetDisplayPosition(const pgd::Vector3 &displayPosition)
{
    m_displayPosition = displayPosition;
    m_modelValid = false;
}

void FacetedObject::SetDisplayScale(const pgd::Vector3 &displayScale)
{
    m_displayScale = displayScale;
    m_modelValid = false;
}

void FacetedObject::SetDisplayRotation(const pgd::Matrix3x3 &R)
{
    m_displayRotation = R;
    m_displayQuaternion = pgd::MakeQfromM(R);
    m_modelValid = false;
}

// pgd::Quaternion q [ w, x, y, z ], where w is the real part and (x, y, z) form the vector part.
void FacetedObject::SetDisplayRotationFromQuaternion(const pgd::Quaternion &q)
{
    m_displayQuaternion = q;
    m_displayRotation = pgd::MakeMFromQ(q);
    m_modelValid = false;
}

void FacetedObject::ClearMeshStore()
{
    m_meshStore.clear();
}

bool FacetedObject::visible() const
{
    return m_visible;
}

void FacetedObject::setVisible(bool visible)
{
    m_visible = visible;
}

const QMatrix4x4 &FacetedObject::model()
{
    if (!m_modelValid)
    {
        m_modelValid = true;
        QMatrix4x4 translationRotation(
            static_cast<float>(m_displayRotation.e11), static_cast<float>(m_displayRotation.e12), static_cast<float>(m_displayRotation.e13), static_cast<float>(m_displayPosition.x),
            static_cast<float>(m_displayRotation.e21), static_cast<float>(m_displayRotation.e22), static_cast<float>(m_displayRotation.e23), static_cast<float>(m_displayPosition.y),
            static_cast<float>(m_displayRotation.e31), static_cast<float>(m_displayRotation.e32), static_cast<float>(m_displayRotation.e33), static_cast<float>(m_displayPosition.z),
            0,                                        0,                                        0,                                         1);
        QMatrix4x4 scale(
            static_cast<float>(m_displayScale.x), 0,                                    0,                                    0,
            0,                                    static_cast<float>(m_displayScale.y), 0,                                    0,
            0,                                    0,                                    static_cast<float>(m_displayScale.z), 0,
            0,                                    0,                                    0,                                    1);
        m_model = translationRotation * scale;
    }
    return m_model;
}

void FacetedObject::setModel(const QMatrix4x4 &model)
{
    m_model = model;
}

#if 0
QOpenGLTexture *FacetedObject::texture() const
{
    return m_texture.get();
}

void FacetedObject::setTexture(std::unique_ptr<QOpenGLTexture> &&texture)
{
    m_texture = std::move(texture);
}
#endif

double FacetedObject::decal() const
{
    return m_decal;
}

void FacetedObject::setDecal(double decal)
{
    m_decal = decal;
}

std::string FacetedObject::filename() const
{
    return m_filename;
}

std::shared_ptr<threepp::Scene> FacetedObject::scene()
{
    return m_scene;
}

void FacetedObject::setScene(const std::shared_ptr<threepp::Scene> &newScene)
{
    m_scene = newScene;
}

bool FacetedObject::castShadow() const
{
    return m_castShadow;
}

void FacetedObject::setCastShadow(bool newCastShadow)
{
    m_castShadow = newCastShadow;
}

bool FacetedObject::receiveShadow() const
{
    return m_receiveShadow;
}

void FacetedObject::setReceiveShadow(bool newReceiveShadow)
{
    m_receiveShadow = newReceiveShadow;
}

bool FacetedObject::wireframe() const
{
    return m_wireframe;
}

void FacetedObject::setWireframe(bool newWireframe)
{
    m_wireframe = newWireframe;
}

QColor FacetedObject::blendColour() const
{
    return m_blendColour;
}

double FacetedObject::blendFraction() const
{
    return m_blendFraction;
}

double FacetedObject::boundingBoxVolume() const
{
    return (m_upperBound[0] - m_lowerBound[0]) * (m_upperBound[1] - m_lowerBound[1]) * (m_upperBound[2] - m_lowerBound[2]);
}

pgd::Vector3 FacetedObject::boundingBoxSize() const
{
    return pgd::Vector3(m_upperBound[0] - m_lowerBound[0], m_upperBound[1] - m_lowerBound[1], m_upperBound[2] - m_lowerBound[2]);
}

