/*
 *  UniversalJoint.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 21/12/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#ifndef UniversalJoint_h
#define UniversalJoint_h

#include "Joint.h"

class Marker;

class UniversalJoint: public Joint
{
public:

    UniversalJoint();

    virtual std::string dumpToString();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    pgd::Vector3 axis0() const;
    void setAxis0(const pgd::Vector3 &newAxis0);

    pgd::Vector3 axis1() const;
    void setAxis1(const pgd::Vector3 &newAxis1);

    pgd::Vector3 anchor() const;
    void setAnchor(const pgd::Vector3 &newAnchor);

    pgd::Vector2 stops0() const;
    void setStops0(const pgd::Vector2 &newStops0);

    double stop0Spring() const;
    void setStop0Spring(double newStop0Spring);

    double stop0Damp() const;
    void setStop0Damp(double newStop0Damp);

    double stop0Bounce() const;
    void setStop0Bounce(double newStop0Bounce);

    pgd::Vector2 stops1() const;
    void setStops1(const pgd::Vector2 &newStops1);

    double stop1Spring() const;
    void setStop1Spring(double newStop1Spring);

    double stop1Damp() const;
    void setStop1Damp(double newStop1Damp);

    double stop1Bounce() const;
    void setStop1Bounce(double newStop1Bounce);

private:

    pgd::Vector3 m_axis0;
    pgd::Vector3 m_axis1;
    pgd::Vector3 m_anchor;
    pgd::Vector2 m_stops0 = {-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    double m_stop0Spring = 0;
    double m_stop0Damp = 0;
    double m_stop0Bounce = 0;
    pgd::Vector2 m_stops1 = {-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    double m_stop1Spring = 0;
    double m_stop1Damp = 0;
    double m_stop1Bounce = 0;

};



#endif
