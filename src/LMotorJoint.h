/*
 *  LMotorJoint.h
 *  GaitSym2019
 *
 *  Created by Bill Sellers on 23/10/2019.
 *  Copyright 2019 Bill Sellers. All rights reserved.
 *
 */

#ifndef LMOTORJOINT_H
#define LMOTORJOINT_H

#include "Joint.h"
#include "PGDMath.h"

#include <limits>

namespace GaitSym
{

class LMotorJoint: public Joint
{
public:

    LMotorJoint();

    void setNumAxes(int numAxes);
    void setStops(int anum, double low, double high);
    void setTargetVelocity(int anum, double targetVelocity);
    void setTargetPosition(int anum, double targetPosition);
    void setTargetPositionGain(int anum, double targetPositionGain);
    void setMaxForce(int anum, double maximumForce);
    void setDynamicFriction(double dynamicFrictionIntercept, double dynamicFrictionSlope);

    int numAxes();
    double position(int anum);
    double positionRate(int anum);
    void getPositions(double *x, double *y, double *z);
    double targetPosition(int anum);

    virtual std::string dumpToString();
    virtual void Update();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

private:

    void SetPosition(int anum, double position, double time);
    void SetDynamicFriction();

    double m_lastPosition0 = 0;
    double m_lastPosition1 = 0;
    double m_lastPosition2 = 0;
    double m_lastPositionRate0 = 0;
    double m_lastPositionRate1 = 0;
    double m_lastPositionRate2 = 0;
    double m_lastTime0 =0;
    double m_lastTime1 = 0;
    double m_lastTime2 = 0;
    bool m_lastTimeValid0 = false;
    bool m_lastTimeValid1 = false;
    bool m_lastTimeValid2 = false;
    bool m_stopsSet0 = false;
    bool m_stopsSet1 = false;
    bool m_stopsSet2 = false;
    pgd::Vector2 m_stops0 = {-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    pgd::Vector2 m_stops1 = {-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    pgd::Vector2 m_stops2 = {-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    double m_targetPosition0 = 0;
    double m_targetPosition1 = 0;
    double m_targetPosition2 = 0;
    bool m_targetPositionSet0 = false;
    bool m_targetPositionSet1 = false;
    bool m_targetPositionSet2 = false;
    double m_targetPositionGain0 = 1;
    double m_targetPositionGain1 = 1;
    double m_targetPositionGain2 = 1;
    double m_targetVelocity0 = 0;
    double m_targetVelocity1 = 0;
    double m_targetVelocity2 = 0;
    double m_maxForce0 = 0;
    double m_maxForce1 = 0;
    double m_maxForce2 = 0;

    int m_numAxes = 0;
    double m_dynamicFrictionIntercept = 0;
    double m_dynamicFrictionSlope = 0;
    bool m_dynamicFrictionFlag = false;
};

}

#endif // LMOTORJOINT_H
