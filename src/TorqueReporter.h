/*
 *  TorqueReporter.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 06/01/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#ifndef TORQUEREPORTER_H
#define TORQUEREPORTER_H

#include "Reporter.h"
#include "PGDMath.h"

namespace GaitSym
{

class Muscle;
class Joint;
class Body;

class TorqueReporter : public Reporter
{
public:
    TorqueReporter();

    void setMuscle(Muscle *muscle) { m_muscle = muscle; }
    void setPivotPoint(double x, double y, double z) { m_pivotPoint = pgd::Vector3(x, y, z); }
    void setAxis(double x, double y, double z);
    void setBody(Body *body) { m_body = body; }

    Muscle *muscle() { return m_muscle; }
    pgd::Vector3 pivotPoint() { return m_pivotPoint; }
    pgd::Vector3 axis() { return m_axis; }
    Body *body() { return m_body; }

    virtual std::string dumpToString();

    // Utility to function to calculate a matrix that lines the x axis up with a supplied vector
    static void calculateRotationFromAxis(double x, double y, double z, pgd::Matrix3x3 *R);

private:

    Body *m_body;
    pgd::Vector3 m_pivotPoint;
    pgd::Vector3 m_axis;
    Muscle *m_muscle;


};

}

#endif // TORQUEREPORTER_H
