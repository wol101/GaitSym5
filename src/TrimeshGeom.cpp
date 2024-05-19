/*
 *  TrimeshGeom.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 11/08/2009.
 *  Copyright 2009 Bill Sellers. All rights reserved.
 *
 */


#include "TrimeshGeom.h"
#include "GSUtil.h"

#include "pystring.h"

using namespace std::string_literals;
using namespace GaitSym;

// create the trimesh object
// m_vertexList is a list of traingular vertices in FacetedObject format
// i.e x1y1z1x2y2z2x3y3z3 for each triangle packed as a single list
TrimeshGeom::TrimeshGeom(const std::vector<double> &vertexList)
{
    m_vertexList = vertexList;
}

std::string *TrimeshGeom::createFromAttributes()
{
    if (Geom::createFromAttributes()) return lastErrorPtr();
    std::string buf;

    if (findAttribute("VertexList"s, &buf) == nullptr) return lastErrorPtr();
    std::vector<std::string> vertexList = pystring::split(buf);
    m_vertexList.clear();
    m_vertexList.reserve(vertexList.size());
    for (auto &&vertex : vertexList) { m_vertexList.push_back(std::stod(vertex)); }
    return nullptr;
}

void TrimeshGeom::appendToAttributes()
{
    Geom::appendToAttributes();
    std::vector<std::string> stringList;
    stringList.reserve(m_vertexList.size());
    for (auto &&vertex : m_vertexList) { stringList.push_back(GSUtil::ToString(vertex)); }
    setAttribute("VertexList"s, pystring::join(" "s, stringList));

    return;
}


