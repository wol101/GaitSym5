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
#include "GSUtil.h"

#include <string>

using namespace std::string_literals;
namespace GaitSym {

BoxGeom::BoxGeom(double lx, double ly, double lz)
{
    m_lx = lx;
    m_ly = ly;
    m_lz = lz;
}

void BoxGeom::getDimensions(double *lx, double *ly, double *lz)
{
    *lx = m_lx;
    *ly = m_lx;
    *lz = m_lx;
}

void BoxGeom::setDimensions(double lx, double ly, double lz)
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
    double lengthX = GSUtil::toDouble(buf);
    if (findAttribute("LengthY"s, &buf) == nullptr) return lastErrorPtr();
    double lengthY = GSUtil::toDouble(buf);
    if (findAttribute("LengthZ"s, &buf) == nullptr) return lastErrorPtr();
    double lengthZ = GSUtil::toDouble(buf);
    setDimensions(lengthX, lengthY, lengthZ);

    return nullptr;
}

void BoxGeom::appendToAttributes()
{
    Geom::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Box"s);
    double lx, ly, lz;
    getDimensions(&lx, &ly, &lz);
    setAttribute("LengthX"s, *GSUtil::toString(lx, &buf));
    setAttribute("LengthY"s, *GSUtil::toString(ly, &buf));
    setAttribute("LengthZ"s, *GSUtil::toString(lz, &buf));

    return;
}

} // namespace GaitSym


