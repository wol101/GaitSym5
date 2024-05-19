/*
 *  Contact.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 09/02/2002.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef Contact_h
#define Contact_h

#include "NamedObject.h"
#include "PGDMath.h"

namespace GaitSym
{

class Body;

class Contact: public NamedObject
{
public:
    Contact();

    pgd::Vector3 position() const;
    void setPosition(const pgd::Vector3 &newPosition);

    pgd::Vector3 force() const;
    void setForce(const pgd::Vector3 &newForce);

    pgd::Vector3 torque() const;
    void setTorque(const pgd::Vector3 &newTorque);

    Body *body1() const;
    void setBody1(Body *newBody1);

    Body *body2() const;
    void setBody2(Body *newBody2);

private:

    Body *m_body1;
    Body *m_body2;
    pgd::Vector3 m_position;
    pgd::Vector3 m_force;
    pgd::Vector3 m_torque;
};

}

#endif

