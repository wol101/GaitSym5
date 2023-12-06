/*
 *  SphereGeom.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 05/12/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#include "SphereGeom.h"
#include "GSUtil.h"

#include <string>

using namespace std::string_literals;

SphereGeom::SphereGeom(double radius)
{
    m_radius = radius;
}

std::string *SphereGeom::createFromAttributes()
{
    if (Geom::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("Radius"s, &buf) == nullptr) return lastErrorPtr();
    setRadius(GSUtil::Double(buf));
    return nullptr;
}

void SphereGeom::appendToAttributes()
{
    Geom::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Sphere"s);
    setAttribute("Radius"s, *GSUtil::ToString(radius(), &buf));
    return;
}

double SphereGeom::radius() const
{
    return m_radius;
}

void SphereGeom::setRadius(double radius)
{
    m_radius = radius;
}


