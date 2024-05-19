/*
 *  TorqueReporter.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 06/01/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#include "TorqueReporter.h"
#include "Simulation.h"
#include "Muscle.h"
#include "Marker.h"

#include <iostream>
#include <vector>
#include <sstream>

using namespace GaitSym;

TorqueReporter::TorqueReporter()
{
    mBody = nullptr;
    mAxis = pgd::Vector3(1, 0, 0);
}


void TorqueReporter::SetAxis(double x, double y, double z)
{
    mAxis = pgd::Vector3(x, y, z);
    mAxis.Normalize();
}

//Theory:
//Point = point of force effect
//Center = center of gravity/rotation
//Direction = direction of force effect (unit)
//Force_Scalar = amount of force effect
//Torque = cross(Point - Center, Direction * Force_Scalar)
//Axis = normalize(cross(Point - Center, Direction));
//Torque_Scalar = dot(Point - Center, Cross(Direction, Axis)) * Force_Scalar;

std::string TorqueReporter::dumpToString()
{
    std::stringstream ss;
    ss.precision(17);
    ss.setf(std::ios::scientific);
    if (firstDump())
    {
        setFirstDump(false);
        ss << "Time\tTension\tWorldTorqueX\tWorldTorqueY\tWorldTorqueZ\tBodyTorqueX\tBodyTorqueY\tBodyTorqueZ\tAxisTorqueX\tAxisTorqueY\tAxisTorqueZ\tWorldMAX\tWorldMAY\tWorldMAZ\tBodyMAX\tBodyMAY\tBodyMAZ\tAxisMAX\tAxisMAY\tAxisMAZ\n";
    }

    // sum the torques acting on body 0 of the joint
    std::vector<std::unique_ptr<PointForce >> *pointForceList = mMuscle->GetPointForceList();
    double tension = mMuscle->GetTension();
    pgd::Vector3 torque, point, force, centre;
    pgd::Vector3 forcePoint, forceDirection;
    pgd::Vector3 totalTorque, momentArm;
    Marker marker(mBody);
    marker.SetPosition(mPivotPoint.x, mPivotPoint.y, mPivotPoint.z);
    centre = marker.GetWorldPosition();

// These are the same but the second option works even when tension is zero
//        if (tension > 0)
//        {
//            for (unsigned int i = 0; i < pointForceList->size(); i++)
//            {
//                if ((*pointForceList)[i]->body == mBody)
//                {
//                    //Torque = cross(Point - Center, Force)
//                    forcePoint = (*pointForceList)[i]->point;
//                    point = pgd::Vector3(forcePoint[0], forcePoint[1], forcePoint[2]);
//                    forceDirection = (*pointForceList)[i]->vector;
//                    force = pgd::Vector3(forceDirection[0], forceDirection[1], forceDirection[2]) * tension;
//                    torque = (point - centre) ^ force;
//                    totalTorque += torque;
//                }
//            }
//            momentArm = totalTorque / tension;
//        }
//        else
    {
        for (unsigned int i = 0; i < pointForceList->size(); i++)
        {
            if ((*pointForceList)[i]->body == mBody)
            {
                //Torque = cross(Point - Center, Force)
                forcePoint = (*pointForceList)[i]->point;
                point = pgd::Vector3(forcePoint[0], forcePoint[1], forcePoint[2]);
                forceDirection = (*pointForceList)[i]->vector;
                force = pgd::Vector3(forceDirection[0], forceDirection[1], forceDirection[2]);
                torque = (point - centre) ^ force;
                momentArm += torque;
            }
        }
        totalTorque = momentArm * tension;
    }

    // convert to body local coordinates
    pgd::Vector3 bodyTorque, bodyMomentArm;
    bodyTorque = marker.GetVector(totalTorque);
    bodyMomentArm = marker.GetVector(momentArm);

    // now find the rotation axis specific values
    pgd::Matrix3x3 R;
    CalculateRotationFromAxis(mAxis.x, mAxis.y, mAxis.z, &R);
    pgd::Vector3 axisBasedTorque = R * totalTorque;
    pgd::Vector3 axisBasedMomentArm = R * momentArm;

    ss << simulation()->GetTime() << "\t" << tension << "\t"
       << totalTorque.x << "\t" << totalTorque.y << "\t" << totalTorque.z << "\t"
       << bodyTorque[0] << "\t" << bodyTorque[1] << "\t" << bodyTorque[2] << "\t"
       << axisBasedTorque.x << "\t" << axisBasedTorque.y << "\t" << axisBasedTorque.z << "\t"
       << momentArm.x << "\t" << momentArm.y << "\t" << momentArm.z << "\t"
       << bodyMomentArm[0] << "\t" << bodyMomentArm[1] << "\t" << bodyMomentArm[2] << "\t"
       << axisBasedMomentArm.x << "\t" << axisBasedMomentArm.y << "\t" << axisBasedMomentArm.z
       << "\n";
    return ss.str();
}

void TorqueReporter::CalculateRotationFromAxis(double x, double y, double z, pgd::Matrix3x3 *R)
{
    // calculate the rotation needed to get the axis pointing the right way
    // axis is assumed to already be normalised
    pgd::Vector3 n(x, y, z);
    pgd::Vector3 p, q;
    // from odemath dxPlaneSpace
    // calculate 2 perpendicular vectors
    if (std::fabs(n[2]) > M_SQRT1_2) {
        // choose p in y-z plane
        double a = n[1]*n[1] + n[2]*n[2];
        double k = 1.0/std::sqrt(a);
        p[0] = 0;
        p[1] = -n[2]*k;
        p[2] = n[1]*k;
        // set q = n x p
        q[0] = a*k;
        q[1] = -n[0]*p[2];
        q[2] = n[0]*p[1];
    }
    else {
        // choose p in x-y plane
        double a = n[0]*n[0] + n[1]*n[1];
        double k = 1.0/std::sqrt(a);
        p[0] = -n[1]*k;
        p[1] = n[0]*k;
        p[2] = 0;
        // set q = n x p
        q[0] = -n[2]*p[1];
        q[1] = n[2]*p[0];
        q[2] = a*k;
    }
    // assemble the matrix
    *R = pgd::Matrix3x3( n[0], n[1], n[2],
                         p[0], p[1], p[2],
                         q[0], q[1], q[2] );
}
