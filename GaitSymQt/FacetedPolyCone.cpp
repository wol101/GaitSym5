/*
 *  FacetedPolyCone.h
 *  GaitSym5
 *
 *  Created by Bill Sellers on 10/06/2024
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#include "FacetedPolyCone.h"
#include "PolylineSweep.h"

#include <algorithm>

// this draws a fat polyline with varying radius. It mostly works very well but large radii and sharp turns will casue holes
// this might be fixable by shifting the circle so that the polyline is at an edge for steep turns rather than in the centre
// the problem is that the cut line can interfere with previous line segments and this is difficult to fix

FacetedPolyCone::FacetedPolyCone(const std::vector<pgd::Vector3> &polyVertexList, const std::vector<std::array<float, 3>> &vertexColours, const std::vector<double> &radiusList, size_t nSides, const QColor &blendColour, double blendFraction)
{
    setBlendColour(blendColour, blendFraction);

    std::vector<PolylineSweep::Vec3> nGon = PolylineSweep::makeNgon(int(nSides), 1.0);
    std::vector<PolylineSweep::Vec3> polyLine;
    std::vector<PolylineSweep::Color> colorList;
    polyLine.reserve(polyVertexList.size());
    for (auto &&vertex : polyVertexList) { polyLine.push_back({vertex.x, vertex.y, vertex.z}); }
    for (auto &&colour : vertexColours) { colorList.push_back({colour[0], colour[1], colour[2]}); }

    PolylineSweep::SweepOptions opts;
    opts.closeEnds = true;
    opts.bendSteps = std::max(4, int(nSides / 4));
    opts.generateNormals = true;
    opts.smoothNormals = true;
    opts.generateUVs = true;

    PolylineSweep::PolylineSweep polylineSweep;
    PolylineSweep::Mesh mesh = polylineSweep.sweep(polyLine, nGon, radiusList, opts, colorList);


    std::vector<double> vertexList;
    std::vector<double> normalList;
    std::vector<float> colourList;
    std::vector<double> uvList;
    for (size_t i = 0; i < mesh.triangles.size(); ++i)
    {
        vertexList.push_back(mesh.vertices[mesh.triangles[i].a].x);
        vertexList.push_back(mesh.vertices[mesh.triangles[i].a].y);
        vertexList.push_back(mesh.vertices[mesh.triangles[i].a].z);
        vertexList.push_back(mesh.vertices[mesh.triangles[i].b].x);
        vertexList.push_back(mesh.vertices[mesh.triangles[i].b].y);
        vertexList.push_back(mesh.vertices[mesh.triangles[i].b].z);
        vertexList.push_back(mesh.vertices[mesh.triangles[i].c].x);
        vertexList.push_back(mesh.vertices[mesh.triangles[i].c].y);
        vertexList.push_back(mesh.vertices[mesh.triangles[i].c].z);
        colourList.push_back(mesh.vertexColors[mesh.triangles[i].a].r);
        colourList.push_back(mesh.vertexColors[mesh.triangles[i].a].g);
        colourList.push_back(mesh.vertexColors[mesh.triangles[i].a].b);
        colourList.push_back(1.0f);
        colourList.push_back(mesh.vertexColors[mesh.triangles[i].b].r);
        colourList.push_back(mesh.vertexColors[mesh.triangles[i].b].g);
        colourList.push_back(mesh.vertexColors[mesh.triangles[i].b].b);
        colourList.push_back(1.0f);
        colourList.push_back(mesh.vertexColors[mesh.triangles[i].c].r);
        colourList.push_back(mesh.vertexColors[mesh.triangles[i].c].g);
        colourList.push_back(mesh.vertexColors[mesh.triangles[i].c].b);
        colourList.push_back(1.0f);
        uvList.push_back(mesh.uvs[mesh.uvTris[i].a].u);
        uvList.push_back(mesh.uvs[mesh.uvTris[i].a].v);
        uvList.push_back(mesh.uvs[mesh.uvTris[i].b].u);
        uvList.push_back(mesh.uvs[mesh.uvTris[i].b].v);
        uvList.push_back(mesh.uvs[mesh.uvTris[i].c].u);
        uvList.push_back(mesh.uvs[mesh.uvTris[i].c].v);
        normalList.push_back(mesh.normals[mesh.normalTris[i].a].x);
        normalList.push_back(mesh.normals[mesh.normalTris[i].a].y);
        normalList.push_back(mesh.normals[mesh.normalTris[i].a].z);
        normalList.push_back(mesh.normals[mesh.normalTris[i].b].x);
        normalList.push_back(mesh.normals[mesh.normalTris[i].b].y);
        normalList.push_back(mesh.normals[mesh.normalTris[i].b].z);
        normalList.push_back(mesh.normals[mesh.normalTris[i].c].x);
        normalList.push_back(mesh.normals[mesh.normalTris[i].c].y);
        normalList.push_back(mesh.normals[mesh.normalTris[i].c].z);
    }
    rawAppend(&vertexList, &normalList, &colourList, &uvList);

    // double tri[9];
    // double normal[9];
    // allocateMemory(mesh.triangles.size() * 9);
    // for (auto &&triangle : mesh.triangles)
    // {
    //     tri[0] = mesh.vertices[triangle.a].x;
    //     tri[1] = mesh.vertices[triangle.a].y;
    //     tri[2] = mesh.vertices[triangle.a].z;
    //     tri[3] = mesh.vertices[triangle.b].x;
    //     tri[4] = mesh.vertices[triangle.b].y;
    //     tri[5] = mesh.vertices[triangle.b].z;
    //     tri[6] = mesh.vertices[triangle.c].x;
    //     tri[7] = mesh.vertices[triangle.c].y;
    //     tri[8] = mesh.vertices[triangle.c].z;
    //     addTriangle(tri, normal);
    // }
}
