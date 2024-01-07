/*
 *  FloatingHingeJoint.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 300/12/2006.
 *  Copyright 2006 Bill Sellers. All rights reserved.
 *
 */

#ifndef FloatingHingeJoint_h
#define FloatingHingeJoint_h

#include "Joint.h"

class FloatingHingeJoint: public Joint
{
public:

    FloatingHingeJoint();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    pgd::Vector3 axis() const;
    void setAxis(const pgd::Vector3 &newAxis);

    pgd::Vector2 stops() const;
    void setStops(const pgd::Vector2 &newStops);

private:
    pgd::Vector3 m_axis;
    pgd::Vector2 m_stops = {-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
};



#endif
