/*
 *  HingeJoint.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 24/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#include "HingeJoint.h"
#include "Simulation.h"
#include "PGDMath.h"
#include "Marker.h"
#include "GSUtil.h"

// #include <iostream>
#include <cstdlib>
#include <sstream>

using namespace std::string_literals;

HingeJoint::HingeJoint() : Joint()
{
}



void HingeJoint::CalculateStopTorque()
{
#ifdef USE_JOINT_ANGLES_FOR_STOP_TORQUE
    // this is the approximate stop torque from the amount past the limit that the joint is
    // it doesn't take into account damping and probably isn't actualy the value used anyway

    double angle = dJointGetHingeAngle(JointID());
    double loStop = dJointGetHingeParam (JointID(), dParamLoStop);
    double hiStop = dJointGetHingeParam (JointID(), dParamHiStop);
    double t = 0; // calculated torque from error
    double ERP, CFM;
    double kp = 0;
    // double kd = 0; // decided not to use damping in these calculations
    if (angle > hiStop)
    {
        ERP = dJointGetHingeParam (JointID(), dParamStopERP);
        CFM = dJointGetHingeParam (JointID(), dParamStopCFM);
        kp = ERP / (CFM * simulation()->GetTimeIncrement());
        //kd = (1 - ERP) / CFM;
        //t = (hiStop - angle) * kp - GetHingeAngleRate() * kd;
        t = (hiStop - angle) * kp;
    }
    else if (angle < loStop)
    {
        ERP = dJointGetHingeParam (JointID(), dParamStopERP);
        CFM = dJointGetHingeParam (JointID(), dParamStopCFM);
        kp = ERP / (CFM * simulation()->GetTimeIncrement());
        //kd = (1 - ERP) / CFM;
        //t = (loStop - angle) * kp - GetHingeAngleRate() * kd;
        t = (loStop - angle) * kp;
    }
#endif
#ifdef FIX_ME
    if (simulation()->GetTime() <= 0)
    {
        m_axisTorque = 0;
        return;
    }


    // now do it properly
    // first of all we need to convert the forces and torques into the joint local coordinate system

    // the force feedback is at the CM for fixed joints
    // first we need to move it to the joint position

    // calculate the offset of the joint anchor from the CM
    pgd::Vector3 jointAnchor;
    dJointGetHingeAnchor(JointID(), jointAnchor);
    dBodyID bodyID = dJointGetBody(JointID(), 0);
    pgd::Vector3 worldForceOffset;
    if (bodyID)
    {
        const double *bodyPosition = dBodyGetPosition(bodyID);
        worldForceOffset = pgd::Vector3(jointAnchor[0] - bodyPosition[0], jointAnchor[1] - bodyPosition[1], jointAnchor[2] - bodyPosition[2]);
    }
    else
    {
        worldForceOffset = pgd::Vector3(jointAnchor[0], jointAnchor[1], jointAnchor[2]);
    }

    // now the linear components of JointFeedback() will generate a torque if applied at this position
    // torque = r x f
    pgd::Vector3 forceCM(JointFeedback()->f1[0], JointFeedback()->f1[1], JointFeedback()->f1[2]);
    pgd::Vector3 addedTorque = worldForceOffset ^ forceCM;

    pgd::Vector3 torqueCM(JointFeedback()->t1[0], JointFeedback()->t1[1], JointFeedback()->t1[2]);
    pgd::Vector3 torqueJointAnchor = torqueCM - addedTorque;

    double torqueScalar = torqueJointAnchor.Magnitude();
    if (torqueScalar == 0)
    {
        m_axisTorque = 0;
        return;
    }

    pgd::Vector3 torqueAxis = torqueJointAnchor / torqueScalar;

    // so the torque around the hinge axis should be: torqueScalar * (hingeAxis .dot. torqueAxis)
    pgd::Vector3 result;
    dJointGetHingeAxis(JointID(), result);
    pgd::Vector3 hingeAxis(result[0], result[1], result[2]);
    m_axisTorque = torqueScalar * (hingeAxis * torqueAxis);

    if (m_axisTorqueWindow < 2)
    {
        m_axisTorqueMean = m_axisTorque;
    }
    else
    {
        m_axisTorqueIndex++;
        if (m_axisTorqueIndex >= m_axisTorqueWindow) m_axisTorqueIndex = 0;

        m_axisTorqueTotal -= m_axisTorqueList[size_t(m_axisTorqueIndex)];
        m_axisTorqueTotal += m_axisTorque;
        m_axisTorqueList[size_t(m_axisTorqueIndex)] = m_axisTorque;
        m_axisTorqueMean = m_axisTorqueTotal / m_axisTorqueWindow;
    }
#endif // FIX_ME
}

double HingeJoint::angle() const
{
    return m_angle;
}

void HingeJoint::setAngle(double newAngle)
{
    m_angle = newAngle;
}

pgd::Vector2 HingeJoint::stops() const
{
    return m_stops;
}

void HingeJoint::setStops(const pgd::Vector2 &newStops)
{
    m_stops = newStops;
}

pgd::Vector3 HingeJoint::anchor() const
{
    return m_anchor;
}

void HingeJoint::setAnchor(const pgd::Vector3 &newAnchor)
{
    m_anchor = newAnchor;
}

pgd::Vector3 HingeJoint::axis() const
{
    return m_axis;
}

void HingeJoint::setAxis(const pgd::Vector3 &newAxis)
{
    m_axis = newAxis;
}

double HingeJoint::axisTorqueMean() const
{
    return m_axisTorqueMean;
}

double HingeJoint::axisTorque() const
{
    return m_axisTorque;
}

int HingeJoint::axisTorqueWindow() const
{
    return m_axisTorqueWindow;
}

void HingeJoint::setAxisTorqueWindow(int newAxisTorqueWindow)
{
    m_axisTorqueWindow = newAxisTorqueWindow;
    if (m_axisTorqueWindow > 1)
    {
        m_axisTorqueList.resize(size_t(m_axisTorqueWindow)); // () initialises the array to zero
    }
    else
    {
        m_axisTorqueList.clear();
        m_axisTorqueWindow = 0;
    }
    m_axisTorqueTotal = 0;
    m_axisTorqueMean = 0;
    m_axisTorqueIndex = 0;
}

pgd::Vector2 HingeJoint::axisTorqueLimits() const
{
    return m_axisTorqueLimits;
}

void HingeJoint::setAxisTorqueLimits(const pgd::Vector2 &newAxisTorqueLimits)
{
    m_axisTorqueLimits = newAxisTorqueLimits;
}

double HingeJoint::stopBounce() const
{
    return m_stopBounce;
}

void HingeJoint::setStopBounce(double newStopBounce)
{
    m_stopBounce = newStopBounce;
}

double HingeJoint::stopDamp() const
{
    return m_stopDamp;
}

void HingeJoint::setStopDamp(double newStopDamp)
{
    m_stopDamp = newStopDamp;
}

double HingeJoint::stopSpring() const
{
    return m_stopSpring;
}

void HingeJoint::setStopSpring(double newStopSpring)
{
    m_stopSpring = newStopSpring;
}

int HingeJoint::TestLimits()
{
    if (m_axisTorqueMean < m_axisTorqueLimits[0]) return -1;
    if (m_axisTorqueMean > m_axisTorqueLimits[1]) return 1;
    return 0;
}

void HingeJoint::Update()
{
    CalculateStopTorque();
}

std::string *HingeJoint::createFromAttributes()
{
    if (Joint::createFromAttributes()) return lastErrorPtr();
    std::string buf;

    pgd::Vector3 axis = body1Marker()->GetWorldAxis(Marker::Axis::X);
    this->setAxis(axis);
    pgd::Vector3 position = body1Marker()->GetWorldPosition();
    this->setAnchor(position);

    if (findAttribute("LowStop"s, &buf) == nullptr) return lastErrorPtr();
    double loStop = GSUtil::GetAngle(buf);
    if (findAttribute("HighStop"s, &buf) == nullptr) return lastErrorPtr();
    double hiStop = GSUtil::GetAngle(buf);
    if (loStop >= hiStop)
    {
        setLastError("Hinge ID=\""s + name() +"\" LowStop >= HighStop"s);
        return lastErrorPtr();
    }
    this->setStops(pgd::Vector2(loStop, hiStop));

    if (findAttribute("HighStopTorqueLimit"s, &buf))
    {
        double hiStopTorqueLimit = GSUtil::Double(buf);
        if (findAttribute("LowStopTorqueLimit"s, &buf) == nullptr) return lastErrorPtr();
        double loStopTorqueLimit = GSUtil::Double(buf);
        this->setAxisTorqueLimits(pgd::Vector2(loStopTorqueLimit, hiStopTorqueLimit));
        if (findAttribute("StopTorqueWindow"s, &buf) == nullptr) return lastErrorPtr();
        this->setAxisTorqueWindow(GSUtil::Int(buf));
    }

    if (findAttribute("StopSpring"s, &buf)) this->setStopSpring(GSUtil::Double(buf));
    if (findAttribute("StopDamp"s, &buf)) this->setStopDamp(GSUtil::Double(buf));
    if (findAttribute("StopBounce"s, &buf)) this->setStopBounce(GSUtil::Double(buf));

    return nullptr;
}

void HingeJoint::appendToAttributes()
{
    Joint::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Hinge"s);
    setAttribute("LowStop"s, *GSUtil::ToString(m_stops[0], &buf));
    setAttribute("HighStop"s, *GSUtil::ToString(m_stops[1], &buf));
    if (m_axisTorqueLimits[1] != std::numeric_limits<double>::infinity())
    {
        setAttribute("HighStopTorqueLimit"s, *GSUtil::ToString(m_axisTorqueLimits[1], &buf));
        setAttribute("LowStopTorqueLimit"s, *GSUtil::ToString(m_axisTorqueLimits[0], &buf));
        setAttribute("StopTorqueWindow"s, *GSUtil::ToString(m_axisTorqueWindow, &buf));
    }
    if (m_stopSpring >= 0) setAttribute("StopSpring"s, *GSUtil::ToString(m_stopSpring, &buf));
    if (m_stopDamp >= 0) setAttribute("StopDamp"s, *GSUtil::ToString(m_stopDamp, &buf));
    setAttribute("StopBounce"s, *GSUtil::ToString(m_stopBounce, &buf));
}

std::string HingeJoint::dumpToString()
{
    std::stringstream ss;
    // ss.precision(17);
    // ss.setf(std::ios::scientific);
    // if (firstDump())
    // {
    //     setFirstDump(false);
    //     ss << "Time\tXP\tYP\tZP\tXP2\tYP2\tZP2\tXA\tYA\tZA\tAngle\tAngleRate\tFX1\tFY1\tFZ1\tTX1\tTY1\tTZ1\tFX2\tFY2\tFZ2\tTX2\tTY2\tTZ2\tStopTorque\n";
    // }
    // pgd::Vector3 p, p2, a;
    // GetHingeAnchor(p);
    // GetHingeAnchor2(p2);
    // GetHingeAxis(a);

    // ss << simulation()->GetTime() << "\t" << p[0] << "\t" << p[1] << "\t" << p[2] << "\t" <<
    //       p2[0] << "\t" << p2[1] << "\t" << p2[2] << "\t" <<
    //       a[0] << "\t" << a[1] << "\t" << a[2] << "\t" << GetHingeAngle() << "\t" << GetHingeAngleRate() << "\t" <<
    //       JointFeedback()->f1[0] << "\t" << JointFeedback()->f1[1] << "\t" << JointFeedback()->f1[2] << "\t" <<
    //       JointFeedback()->t1[0] << "\t" << JointFeedback()->t1[1] << "\t" << JointFeedback()->t1[2] << "\t" <<
    //       JointFeedback()->f2[0] << "\t" << JointFeedback()->f2[1] << "\t" << JointFeedback()->f2[2] << "\t" <<
    //       JointFeedback()->t2[0] << "\t" << JointFeedback()->t2[1] << "\t" << JointFeedback()->t2[2] << "\t" <<
    //       m_axisTorque <<
    //       "\n";
    return ss.str();
}



