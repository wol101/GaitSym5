/*
 *  Contact.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 09/02/2006.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#include "Contact.h"

using namespace GaitSym;

Contact::Contact()
{
}

pgd::Vector3 Contact::position() const
{
    return m_position;
}

void Contact::setPosition(const pgd::Vector3 &newPosition)
{
    m_position = newPosition;
}

pgd::Vector3 Contact::force() const
{
    return m_force;
}

void Contact::setForce(const pgd::Vector3 &newForce)
{
    m_force = newForce;
}

pgd::Vector3 Contact::torque() const
{
    return m_torque;
}

void Contact::setTorque(const pgd::Vector3 &newTorque)
{
    m_torque = newTorque;
}

Body *Contact::body1() const
{
    return m_body1;
}

void Contact::setBody1(Body *newBody1)
{
    m_body1 = newBody1;
}

Body *Contact::body2() const
{
    return m_body2;
}

void Contact::setBody2(Body *newBody2)
{
    m_body2 = newBody2;
}


