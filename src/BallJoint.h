/*
 *  BallJoint.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 29/12/2008.
 *  Copyright 2008 Bill Sellers. All rights reserved.
 *
 */

#ifndef BallJoint_h
#define BallJoint_h

#include "Joint.h"
#include "PGDMath.h"
// #include "SmartEnum.h"

class Marker;

class BallJoint: public Joint
{
public:

    // SMART_ENUM(Mode, modeStrings, modeCount, AMotorUser, AMotorEuler, NoStops);

    BallJoint();


    // void SetBallAnchor (double x, double y, double z);
    // void SetStops(double a0Low, double a0High, double a1Low, double a1High, double a2Low, double a2High);
    // void SetAxes(double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2, int axisMode);

    // pgd::Vector3 GetBallAnchor() const;
    // void GetBallAnchor2(pgd::Vector3 result);
    // void GetEulerReferenceVectors(pgd::Vector3 reference1, pgd::Vector3 reference2);
    pgd::Quaternion CalculateQuaternion();


//    virtual void Update();
    virtual std::string dumpToString();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    // Mode GetMode() const;

    pgd::Vector3 anchor() const;
    void setAnchor(const pgd::Vector3 &newAnchor);

private:

    pgd::Vector3 m_anchor;
};



#endif
