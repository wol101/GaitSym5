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

class Contact:public NamedObject
{
public:
    Contact();

    void GetFeedback(pgd::Vector3 *position, pgd::Vector3 *force, pgd::Vector3 *torque);

private:

    pgd::Vector3 m_position;
    pgd::Vector3 m_force;
    pgd::Vector3 m_torque;
};


#endif

