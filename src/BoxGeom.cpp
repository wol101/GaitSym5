/*
 *  BoxGeom.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 30/05/2012.
 *  Copyright 2012 Bill Sellers. All rights reserved.
 *
 */

#include "BoxGeom.h"
#include "Simulation.h"
#include "Marker.h"
#include "GSUtil.h"


#include "ode/ode.h"

#include <string>

using namespace std::string_literals;

BoxGeom::BoxGeom(double lx, double ly, double lz)
{
    m_lx = lx;
    m_ly = ly;
    m_lz = lz;
}

void BoxGeom::GetDimensions(double *lx, double *ly, double *lz)
{
    *lx = m_lx;
    *ly = m_lx;
    *lz = m_lx;
}

void BoxGeom::SetDimensions(double lx, double ly, double lz)
{
    m_lx = lx;
    m_ly = ly;
    m_lz = lz;
}

std::string *BoxGeom::createFromAttributes()
{
    if (Geom::createFromAttributes()) return lastErrorPtr();
    std::string buf;

    if (findAttribute("LengthX"s, &buf) == nullptr) return lastErrorPtr();
    double lengthX = GSUtil::Double(buf);
    if (findAttribute("LengthY"s, &buf) == nullptr) return lastErrorPtr();
    double lengthY = GSUtil::Double(buf);
    if (findAttribute("LengthZ"s, &buf) == nullptr) return lastErrorPtr();
    double lengthZ = GSUtil::Double(buf);
    SetDimensions(lengthX, lengthY, lengthZ);

    return nullptr;
}

void BoxGeom::appendToAttributes()
{
    Geom::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Box"s);
    double lx, ly, lz;
    GetDimensions(&lx, &ly, &lz);
    setAttribute("LengthX"s, *GSUtil::ToString(lx, &buf));
    setAttribute("LengthY"s, *GSUtil::ToString(ly, &buf));
    setAttribute("LengthZ"s, *GSUtil::ToString(lz, &buf));

    return;
}

