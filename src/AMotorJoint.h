/*
 *  AMotorJoint.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 07/01/2011.
 *  Copyright 2011 Bill Sellers. All rights reserved.
 *
 */

#ifndef AMOTORJOINT_H
#define AMOTORJOINT_H

#include "Joint.h"
#include "PGDMath.h"

#include <limits>

namespace GaitSym
{

class AMotorJoint: public Joint
{
public:

    AMotorJoint();

    void getAxisAngle(double *xa, double *ya, double *za, double *angle) const;
    pgd::Quaternion quaternion() const;
    pgd::Vector3 eulerAngles() const;
    pgd::Vector3 eulerAngles(const Marker &basisMarker) const;

    void setTargetAngles(double angle0);
    void setTargetAngles(double angle0, double angle1);
    void setTargetAngles(double angle0, double angle1, double angle2);
    void setTargetAngleGain(double targetAngleGain);
    void setMaxTorque(double maxTorque);

    pgd::Vector3 targetAxis() const;
    double targetAngle() const;
    double targetAngleGain() const;
    double maxTorque() const;


    void setDynamicFriction(double dynamicFrictionIntercept, double dynamicFrictionSlope);

    virtual std::string dumpToString();
    virtual void update();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    const std::vector<double> &targetAnglesList() const;

    bool reverseBodyOrderInCalculations() const;
    void setReverseBodyOrderInCalculations(bool reverseBodyOrderInCalculations);

private:

    pgd::Vector3 m_targetAxis;
    double m_targetAngle = 0;
    double m_targetAngleGain = 1;
    std::vector<double> m_targetAnglesList;
    pgd::Vector3 m_lastDeltaAxis;
    pgd::Vector3 m_deltaAxis;
    double m_deltaAngle = 0;
    pgd::Quaternion m_currentQuaternion;
    pgd::Quaternion m_lastQuaternion;
    pgd::Quaternion m_lastToCurrent;
    bool m_firstTime = true;
    bool m_reverseBodyOrderInCalculations = false;
    double m_maxTorque = std::numeric_limits<double>::infinity();
    double m_targetVelocity = 0;

    void updateDynamicFriction();
    double m_dynamicFrictionIntercept = 0;
    double m_dynamicFrictionSlope = 0;
    bool m_dynamicFrictionFlag = false;
};

}

#endif // AMOTORJOINT_H
