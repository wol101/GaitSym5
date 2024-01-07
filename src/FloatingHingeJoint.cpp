/*
 *  FloatingHingeJoint.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 30/12/2006.
 *  Copyright 2006 Bill Sellers. All rights reserved.
 *
 */

#include "FloatingHingeJoint.h"

#include "PGDMath.h"
#include "Simulation.h"
#include "Marker.h"
#include "GSUtil.h"


using namespace std::string_literals;

FloatingHingeJoint::FloatingHingeJoint() : Joint()
{
}

std::string *FloatingHingeJoint::createFromAttributes()
{
    if (Joint::createFromAttributes()) return lastErrorPtr();
    pgd::Vector3 axis = body1Marker()->GetWorldAxis(Marker::Axis::X);
    this->setAxis(axis);

    std::string buf;
    if (findAttribute("LowStop"s, &buf) == nullptr) return lastErrorPtr();
    double loStop = GSUtil::GetAngle(buf);
    if (findAttribute("HighStop"s, &buf) == nullptr) return lastErrorPtr();
    double hiStop = GSUtil::GetAngle(buf);
    if (loStop >= hiStop)
    {
        setLastError("FloatingHinge ID=\""s + name() +"\" LowStop >= HighStop"s);
        return lastErrorPtr();
    }
    this->setStops(pgd::Vector2(loStop, hiStop));
    return nullptr;
}

void FloatingHingeJoint::appendToAttributes()
{
    Joint::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "FloatingHinge"s);
    setAttribute("LowStop"s, *GSUtil::ToString(m_stops[0], &buf));
    setAttribute("HighStop"s, *GSUtil::ToString(m_stops[1], &buf));
}

pgd::Vector3 FloatingHingeJoint::axis() const
{
    return m_axis;
}

void FloatingHingeJoint::setAxis(const pgd::Vector3 &newAxis)
{
    m_axis = newAxis;
}

pgd::Vector2 FloatingHingeJoint::stops() const
{
    return m_stops;
}

void FloatingHingeJoint::setStops(const pgd::Vector2 &newStops)
{
    m_stops = newStops;
}


