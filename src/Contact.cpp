/*
 *  Contact.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 09/02/2006.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#include "Contact.h"

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


