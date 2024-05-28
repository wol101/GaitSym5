/*
 *  PlaneGeom.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 13/09/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#include "PlaneGeom.h"
#include "Marker.h"

#include <string>
#include <limits>
#include <cmath>

using namespace std::string_literals;
namespace GaitSym {

// The plane equation is:
// a * x + b * y + c * z = d
// The plane's normal vector is (a, b, c), and it must have length 1.
// d is then the distance of the plane along that normal vector

// Note: planes are non placeable in ODE and have to be attached to non-dynamic actors in PhysX
// the Z axis of the marker is used as the normal, which means the X and Y axes are in the plane

PlaneGeom::PlaneGeom(double a, double b, double c, double d)
{
    SetPlane(a, b, c, d);
}

void PlaneGeom::SetPlane(double a, double b, double c, double d)
{
    // create the geom
    double length = std::sqrt(a * a + b * b + c * c);
    if (length < std::numeric_limits<double>::min()) // standard fixup
    {
        a = 0;
        b = 0;
        c = 1;
        d = 0;
    }
    else
    {
        a = a / length;
        b = b / length;
        c = c / length;
    }
    m_a = a;
    m_b = b;
    m_c = c;
    m_d = d;
}

void PlaneGeom::GetPlane(double *a, double *b, double *c, double *d)
{
    *a = m_a;
    *b = m_b;
    *c = m_c;
    *d = m_d;
}

std::string *PlaneGeom::createFromAttributes()
{
    if (Geom::createFromAttributes()) return lastErrorPtr();
    std::string buf;

    if (geomMarker()->GetBody())
    {
        setLastError("GEOM ID=\""s + name() +"\" PlaneGeom must be attached to the World"s);
        return lastErrorPtr();
    }

    // because planes are non-placeable we need to calculate the plane equation directly from the marker
    //
    // we want the plane in its cartesian form a*x+b*y+c*z = d
    // the marker gives a point on the plane, 2 vectors in the plane (x & y axes)
    // and the plane normal (z axes)
    // the normal is (a,b,c)
    // d is the dot product of the normal at the point on the plane

    pgd::Vector3 normal = geomMarker()->GetWorldAxis(Marker::Axis::Z);
    pgd::Vector3 point = geomMarker()->GetWorldPosition();
    double a = normal.x;
    double b = normal.y;
    double c = normal.z;
    double d = normal.Dot(point);
    SetPlane(a, b, c, d);
    return nullptr;
}

void PlaneGeom::appendToAttributes()
{
    Geom::appendToAttributes();
    setAttribute("Type"s, "Plane"s);
    return;
}


} // namespace GaitSym


