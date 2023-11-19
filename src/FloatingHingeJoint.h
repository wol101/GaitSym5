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

    FloatingHingeJoint(dWorldID worldID);

    void SetFloatingHingeAxis(double x, double y, double z);

    void SetJointStops(double loStop, double hiStop);

    void GetFloatingHingeAnchor(pgd::Vector3 result);
    void GetFloatingHingeAnchor2(pgd::Vector3 result);
    void GetFloatingHingeAxis(pgd::Vector3 result);

    double GetFloatingHingeAngle();
    double GetFloatingHingeAngleRate();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

};



#endif
