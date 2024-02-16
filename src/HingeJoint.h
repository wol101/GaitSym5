/*
 *  HingeJoint.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 24/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef HingeJoint_h
#define HingeJoint_h

#include "Joint.h"

class Marker;

class HingeJoint: public Joint
{
public:

    HingeJoint();

    int TestLimits();

    virtual void Update();
    virtual std::string dumpToString();
    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    double stopSpring() const;
    void setStopSpring(double newStopSpring);

    double stopDamp() const;
    void setStopDamp(double newStopDamp);

    double stopBounce() const;
    void setStopBounce(double newStopBounce);

    pgd::Vector2 axisTorqueLimits() const;
    void setAxisTorqueLimits(const pgd::Vector2 &newAxisTorqueLimits);

    int axisTorqueWindow() const;
    void setAxisTorqueWindow(int newAxisTorqueWindow);

    double axisTorque() const;

    double axisTorqueMean() const;

    pgd::Vector3 axis() const;
    void setAxis(const pgd::Vector3 &newAxis);

    pgd::Vector3 anchor() const;
    void setAnchor(const pgd::Vector3 &newAnchor);

    pgd::Vector2 stops() const;
    void setStops(const pgd::Vector2 &newStops);

    double angle() const;
    void setAngle(double newAngle);

    double angleRate() const;
    void setAngleRate(double newAngleRate);

private:

    void CalculateStopTorque();

    pgd::Vector2 m_axisTorqueLimits = {-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    double m_axisTorque = 0;
    std::vector<double> m_axisTorqueList;
    double m_axisTorqueTotal = 0;
    double m_axisTorqueMean = 0;
    int m_axisTorqueIndex = 0;
    int m_axisTorqueWindow = 0;

    pgd::Vector3 m_axis;
    pgd::Vector3 m_anchor;
    pgd::Vector2 m_stops = {-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    double m_stopSpring = 0;
    double m_stopDamp = 0;
    double m_stopBounce = 0;

    double m_angle = 0;
    double m_angleRate = 0;
};



#endif
