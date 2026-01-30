/*
 *  Muscle.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 29/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */


#include "Muscle.h"

#include <string>
#include <iostream>
#include <cassert>

using namespace std::string_literals;
namespace GaitSym {

Muscle::Muscle()
{
}

Muscle::~Muscle()
{
}

std::string *Muscle::createFromAttributes()
{
    if (NamedObject::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("StrapID"s, &buf) == nullptr) return lastErrorPtr();
    auto strapList = simulation()->strapList();
    auto it = strapList->find(buf);
    if (it == strapList->end())
    {
        setLastError("MUSCLE ID=\""s + name() +"\" StrapID=\""s + buf + "\" not found"s);
        return lastErrorPtr();
    }
    this->setStrap(it->second.get());
#ifdef SAVE_CUSTOM_STRAP_COLOUR_CONTROL
    if (findAttribute("StrapColourControl"s, &buf))
    {
        if (buf == "FixedColour"s) setStrapColourControl(fixedColour);
        else if (buf == "ActivationMap"s) setStrapColourControl(activationMap);
        else if (buf == "StrainMap"s) setStrapColourControl(strainMap);
        else if (buf == "ForceMap"s) setStrapColourControl(forceMap);
        else
        {
            setLastError("MUSCLE ID=\""s + name() +"\" StrapColourControl=\""s + buf + "\" not recognised: "s);
            return lastErrorPtr();
        }
    }
#endif
    setUpstreamObjects({m_Strap});
    return nullptr;
}

void Muscle::saveToAttributes()
{
    this->setTag("MUSCLE"s);
    this->clearAttributeMap();
    this->appendToAttributes();
}

void Muscle::appendToAttributes()
{
    NamedObject::appendToAttributes();
    setAttribute("StrapID"s, this->strap()->name());

#ifdef SAVE_CUSTOM_STRAP_COLOUR_CONTROL
    switch (m_strapColourControl)
    {
    case fixedColour:
        setAttribute("StrapColourControl"s, "FixedColour"s);
        break;
    case activationMap:
        setAttribute("StrapColourControl"s, "ActivationMap"s);
        break;
    case strainMap:
        setAttribute("StrapColourControl"s, "StrainMap"s);
        break;
    case forceMap:
        setAttribute("StrapColourControl"s, "ForceMap"s);
        break;
    }
#endif
    return;
}

Muscle::StrapColourControl Muscle::strapColourControl() const
{
    return m_strapColourControl;
}

void Muscle::setStrapColourControl(const Muscle::StrapColourControl &strapColourControl)
{
    m_strapColourControl = strapColourControl;
}

double Muscle::length() const
{
    return m_Strap->length();
}

double Muscle::velocity() const
{
    return m_Strap->velocity();
}

double Muscle::tension() const
{
    return m_Strap->tension();
}

double Muscle::power() const
{
    return -(m_Strap->tension() * m_Strap->velocity());
}

void Muscle::calculateStrap()
{
    m_Strap->calculate();
}

std::vector<std::unique_ptr<PointForce >> *Muscle::pointForceList() const
{
    return m_Strap->pointForceList();
}

Strap *Muscle::strap() const
{
    return m_Strap;
}

void Muscle::setStrap(Strap *strap)
{
    m_Strap = strap;
}

//int Muscle::sanityCheck(Muscle *otherMuscle, Simulation::AxisType axis, const std::string &sanityCheckLeft, const std::string &sanityCheckRight)
//{
//    return m_Strap->sanityCheck(otherMuscle->m_Strap, axis, sanityCheckLeft, sanityCheckRight);
//}

void Muscle::lateInitialisation()
{
    calculateStrap();
}



} // namespace GaitSym


