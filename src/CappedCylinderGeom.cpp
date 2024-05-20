/*
 *  CappedCylinderGeom.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 28/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#include "CappedCylinderGeom.h"
#include "GSUtil.h"

#include <string>

using namespace std::string_literals;
namespace GaitSym {

CappedCylinderGeom::CappedCylinderGeom(double radius, double length)
{
    m_radius = radius;
    m_length = length;
}

std::string *CappedCylinderGeom::createFromAttributes()
{
    if (Geom::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("Radius"s, &buf) == nullptr) return lastErrorPtr();
    double radius = GSUtil::Double(buf);
    if (findAttribute("Length"s, &buf) == nullptr) return lastErrorPtr();
    double length = GSUtil::Double(buf);
    setLengthRadius(radius, length);

    return nullptr;
}

void CappedCylinderGeom::appendToAttributes()
{
    Geom::appendToAttributes();
    std::string buf;

    setAttribute("Type"s, "CappedCylinder"s);
    double radius, length;
    getLengthRadius(&radius, &length);
    setAttribute("Radius"s, *GSUtil::ToString(radius, &buf));
    setAttribute("Length"s, *GSUtil::ToString(length, &buf));
    return;
}

void CappedCylinderGeom::setLengthRadius(double length, double radius)
{
    m_radius = radius;
    m_length = length;
}

void CappedCylinderGeom::getLengthRadius(double *length, double *radius) const
{
    *radius = m_radius;
    *length = m_length;
}



} // namespace GaitSym


