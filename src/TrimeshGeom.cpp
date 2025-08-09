/*
 *  TrimeshGeom.cpp
 *  GaitSym2019
 *
 *  Created by Bill Sellers on 27/02/2021.
 *  Copyright 2021 Bill Sellers. All rights reserved.
 *
 */

#include "TrimeshGeom.h"

#include "Simulation.h"
#include "GSUtil.h"


#include <string>

using namespace std::string_literals;
namespace GaitSym {

TrimeshGeom::TrimeshGeom()
{
}

std::string *TrimeshGeom::createFromAttributes()
{
    if (Geom::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    buf.reserve(1000000);
    if (findAttribute("IndexStart"s, &buf) == nullptr) return lastErrorPtr();
    m_indexStart = GSUtil::Int(buf);
    if (findAttribute("Vertices"s, &buf) == nullptr) return lastErrorPtr();
    GSUtil::Double(buf, &m_vertices);
    if (findAttribute("Triangles"s, &buf) == nullptr) return lastErrorPtr();
    GSUtil::Int(buf, &m_triangles);
    if (findAttribute("ReverseWinding"s, &buf)) m_reverseWinding = GSUtil::Bool(buf);
    if (m_indexStart) { for (size_t i = 0; i < m_triangles.size(); i++) { m_triangles[i] -= m_indexStart; } }
    if (m_reverseWinding) { for (size_t i = 0; i < m_triangles.size(); i += 3) { std::swap(m_triangles[i], m_triangles[i + 2]); } }
    return nullptr;
}

void TrimeshGeom::appendToAttributes()
{
    Geom::appendToAttributes();
    std::string buf;
    buf.reserve(1000000);
    setAttribute("Type"s, "Trimesh"s);
    setAttribute("IndexStart"s, *GSUtil::ToString(m_indexStart, &buf));
    setAttribute("ReverseWinding"s, *GSUtil::ToString(m_reverseWinding, &buf));
    setAttribute("Vertices"s, *GSUtil::ToString(m_vertices.data(), m_vertices.size(), &buf));
    if (m_indexStart) { for (size_t i = 0; i < m_triangles.size(); i++) { m_triangles[i] += m_indexStart; } }
    if (m_reverseWinding) { for (size_t i = 0; i < m_triangles.size(); i += 3) { std::swap(m_triangles[i], m_triangles[i + 2]); } }
    setAttribute("Triangles"s, *GSUtil::ToString(m_triangles.data(), m_triangles.size(), &buf));
    if (m_reverseWinding) { for (size_t i = 0; i < m_triangles.size(); i += 3) { std::swap(m_triangles[i], m_triangles[i + 2]); } }
    if (m_indexStart) { for (size_t i = 0; i < m_triangles.size(); i++) { m_triangles[i] -= m_indexStart; } }
    return;
}


std::vector<int> *TrimeshGeom::triangles()
{
    return &m_triangles;
}

bool TrimeshGeom::reverseWinding() const
{
    return m_reverseWinding;
}

void TrimeshGeom::setReverseWinding(bool newReverseWinding)
{
    m_reverseWinding = newReverseWinding;
}

int TrimeshGeom::indexStart() const
{
    return m_indexStart;
}

void TrimeshGeom::setIndexStart(int newIndexStart)
{
    m_indexStart = newIndexStart;
}

std::vector<double> *TrimeshGeom::vertices()
{
    return &m_vertices;
}

} // namespace GaitSym


