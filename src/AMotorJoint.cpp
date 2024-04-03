/*
 *  AMotorJoint.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 07/01/2011.
 *  Copyright 2011 Bill Sellers. All rights reserved.
 *
 */

#include "AMotorJoint.h"
#include "Simulation.h"
#include "Body.h"
#include "PGDMath.h"
#include "Marker.h"
#include "GSUtil.h"

#include "pystring.h"

#include <sstream>
#include <limits>

AMotorJoint::AMotorJoint() : Joint()
{
    // setJointID(dJointCreateAMotor(worldID, nullptr));
    // dJointSetData(JointID(), this);
    // dJointSetAMotorMode(JointID(), dAMotorUser);

    // dJointSetFeedback(JointID(), JointFeedback());

    // // this is always implmented as a single axis motor
    // // with the axis set globally and updated each time step
    // dJointSetAMotorNumAxes(JointID(), 1);

    // // rel value controls how the axis is anchored
    // // 0: The axis is anchored to the global frame.
    // // 1: The axis is anchored to the first body.
    // // 2: The axis is anchored to the second body.
    // // x, y, z are always set globally
    // // NB. If attached to World then must use rel = 0
    // int rel = 0;
    // // this is just a placeholder since it will generally get recalculated
    // pgd::Vector3 axis(1, 0, 0);
    // dJointSetAMotorAxis(JointID(), 0, rel, axis.x, axis.y, axis.z);
}

void AMotorJoint::GetAxisAngle(double *xa, double *ya, double *za, double *angle) const
{
    pgd::Quaternion body1MarkerWorld = body1Marker()->GetWorldQuaternion();
    pgd::Quaternion body2MarkerWorld = body2Marker()->GetWorldQuaternion();
    if (m_reverseBodyOrderInCalculations)
    {
        // angle is calculated with respect to body 1
        pgd::Quaternion body1ToBody2 = body2MarkerWorld * (~body1MarkerWorld);
        pgd::MakeAxisAngleFromQ(body1ToBody2, xa, ya, za, angle);
    }
    else
    {
        // angle is calculated with respect to body 2
        pgd::Quaternion body2ToBody1 = body1MarkerWorld * (~body2MarkerWorld);
        pgd::MakeAxisAngleFromQ(body2ToBody1, xa, ya, za, angle);
    }
}

pgd::Quaternion AMotorJoint::GetQuaternion() const
{
    pgd::Quaternion body1MarkerWorld = body1Marker()->GetWorldQuaternion();
    pgd::Quaternion body2MarkerWorld = body2Marker()->GetWorldQuaternion();
    if (m_reverseBodyOrderInCalculations)
    {
        // angle is calculated with respect to body 1
        pgd::Quaternion body1ToBody2 = body2MarkerWorld * (~body1MarkerWorld);
        return body1ToBody2;
    }
    else
    {
        // angle is calculated with respect to body 2
        pgd::Quaternion body2ToBody1 = body1MarkerWorld * (~body2MarkerWorld);
        return body2ToBody1;
    }
}

pgd::Vector3 AMotorJoint::GetEulerAngles() const
{
    pgd::Quaternion body1MarkerWorld = body1Marker()->GetWorldQuaternion();
    pgd::Quaternion body2MarkerWorld = body2Marker()->GetWorldQuaternion();
    if (m_reverseBodyOrderInCalculations)
    {
        // angle is calculated with respect to body 1
        pgd::Quaternion body1ToBody2 = body2MarkerWorld * (~body1MarkerWorld);
        pgd::Vector3 euler = pgd::MakeEulerAnglesFromQRadian(body1ToBody2);
        return euler;
    }
    else
    {
        // angle is calculated with respect to body 2
        pgd::Quaternion body2ToBody1 = body1MarkerWorld * (~body2MarkerWorld);
        pgd::Vector3 euler = pgd::MakeEulerAnglesFromQRadian(body2ToBody1);
        return euler;
    }
}

pgd::Vector3 AMotorJoint::GetEulerAngles(const Marker &basisMarker) const
{
    // returns the Euler angles using marker axes as the basis
    pgd::Vector3 euler;
    pgd::Quaternion body1MarkerWorld = body1Marker()->GetWorldQuaternion();
    pgd::Quaternion body2MarkerWorld = body2Marker()->GetWorldQuaternion();
    if (m_reverseBodyOrderInCalculations)
    {
        // angle is calculated with respect to body 1
        pgd::Quaternion body1ToBody2 = body2MarkerWorld * (~body1MarkerWorld);
        euler = pgd::MakeEulerAnglesFromQRadian(body1ToBody2, basisMarker.GetWorldBasis());
    }
    else
    {
        // angle is calculated with respect to body 2
        pgd::Quaternion body2ToBody1 = body1MarkerWorld * (~body2MarkerWorld);
        euler = pgd::MakeEulerAnglesFromQRadian(body2ToBody1, basisMarker.GetWorldBasis());
    }
    return euler;
}

void AMotorJoint::SetDynamicFriction(double dynamicFrictionIntercept, double dynamicFrictionSlope)
{
    m_dynamicFrictionIntercept = dynamicFrictionIntercept;
    m_dynamicFrictionSlope = dynamicFrictionSlope;
    m_dynamicFrictionFlag = true;

    UpdateDynamicFriction();
}

void AMotorJoint::UpdateDynamicFriction()
{
    pgd::Vector3 axis;
    double deltaAngle;
    pgd::MakeAxisAngleFromQ(m_lastToCurrent, &axis.x, &axis.y, &axis.z, &deltaAngle);
    double angularVelocity = deltaAngle / simulation()->GetTimeIncrement();  // note this value will not necessarily have the correct sign
    double maxTorque = m_dynamicFrictionIntercept + m_dynamicFrictionSlope * std::fabs(angularVelocity);
    SetMaxTorque(maxTorque);
}

const std::vector<double> &AMotorJoint::targetAnglesList() const
{
    return m_targetAnglesList;
}

bool AMotorJoint::reverseBodyOrderInCalculations() const
{
    return m_reverseBodyOrderInCalculations;
}

void AMotorJoint::setReverseBodyOrderInCalculations(bool reverseBodyOrderInCalculations)
{
    m_reverseBodyOrderInCalculations = reverseBodyOrderInCalculations;
}

void AMotorJoint::SetTargetAngles(double angle0)
{
    m_targetAxis = body1Marker()->GetWorldAxis(Marker::X);
    m_targetAngle = angle0;
    m_targetAnglesList.clear();
    m_targetAnglesList.push_back(angle0);
}

void AMotorJoint::SetTargetAngles(double angle0, double angle1)
{
    pgd::Vector3 ax,ay,az;
    body1Marker()->GetWorldBasis(&ax, &ay, &az);
    pgd::Quaternion r1 = pgd::MakeQFromAxisAngle(ax, angle0);
    pgd::Quaternion r2 = pgd::MakeQFromAxisAngle(ay, angle1);
    pgd::MakeAxisAngleFromQ(r2 * r1, &m_targetAxis.x, &m_targetAxis.y, &m_targetAxis.z, &m_targetAngle);
    m_targetAnglesList.clear();
    m_targetAnglesList.push_back(angle0);
    m_targetAnglesList.push_back(angle1);
}

void AMotorJoint::SetTargetAngles(double angle0, double angle1, double angle2)
{
    pgd::Vector3 ax,ay,az;
    body1Marker()->GetWorldBasis(&ax, &ay, &az);
    pgd::Quaternion r1 = pgd::MakeQFromAxisAngle(ax, angle0);
    pgd::Quaternion r2 = pgd::MakeQFromAxisAngle(ay, angle1);
    pgd::Quaternion r3 = pgd::MakeQFromAxisAngle(az, angle2);
    pgd::MakeAxisAngleFromQ(r3 * r2 * r1, &m_targetAxis.x, &m_targetAxis.y, &m_targetAxis.z, &m_targetAngle);
    m_targetAnglesList.clear();
    m_targetAnglesList.push_back(angle0);
    m_targetAnglesList.push_back(angle1);
    m_targetAnglesList.push_back(angle2);
}

void AMotorJoint::SetTargetAngleGain(double targetAngleGain)
{
    m_targetAngleGain = targetAngleGain;
}

void AMotorJoint::SetMaxTorque(double maxTorque)
{
    m_maxTorque = maxTorque;
}

pgd::Vector3 AMotorJoint::GetTargetAxis() const
{
    return m_targetAxis;
}

double AMotorJoint::GetTargetAngle() const
{
    return m_targetAngle;
}

double AMotorJoint::GetTargetAngleGain() const
{
    return m_targetAngleGain;
}

double AMotorJoint::GetMaxTorque() const
{
    return m_maxTorque;
}

void AMotorJoint::Update()
{
    // handle the angular change code
    m_currentQuaternion = GetQuaternion();
    if (m_firstTime) m_lastQuaternion = m_currentQuaternion;
    m_lastToCurrent = m_currentQuaternion * (~m_lastQuaternion);
    m_lastQuaternion = m_currentQuaternion;

    pgd::Quaternion targetQuaternion = pgd::MakeQFromAxisAngle(m_targetAxis, m_targetAngle);
    pgd::Quaternion currentToTarget = targetQuaternion * (~m_currentQuaternion);
    pgd::MakeAxisAngleFromQ(currentToTarget, &m_deltaAxis.x, &m_deltaAxis.y, &m_deltaAxis.z, &m_deltaAngle);
    if (body1())
    {
        pgd::Quaternion bodyRotation(body1()->GetQuaternion());
        m_deltaAxis = pgd::QVRotate(bodyRotation, m_deltaAxis);
    }
    if (m_firstTime) m_lastDeltaAxis = m_deltaAxis;
    if (pgd::Dot(m_lastDeltaAxis, m_deltaAxis) < 0)
    {
        m_deltaAxis = -m_deltaAxis;
        m_deltaAngle = -m_deltaAngle;
    }
    m_targetVelocity = m_deltaAngle * m_targetAngleGain;
    if (m_targetVelocity * simulation()->GetTimeIncrement() > m_deltaAngle) m_targetVelocity = 0.5 * m_deltaAngle / simulation()->GetTimeIncrement();

    m_firstTime = false;
}

std::string *AMotorJoint::createFromAttributes()
{
    if (Joint::createFromAttributes()) return lastErrorPtr();
    std::string buf, buf2, buf3;

    if (findAttribute("MaxTorque"s, &buf) == nullptr) return lastErrorPtr();
    this->SetMaxTorque(GSUtil::Double(buf));
    if (findAttribute("TargetAngleGain"s, &buf) == nullptr) return lastErrorPtr();
    this->SetTargetAngleGain(GSUtil::Double(buf));

    if (findAttribute("TargetAngles"s, &buf) == nullptr) return lastErrorPtr();
    std::vector<std::string> tokens;
    pystring::split(buf, tokens);
    if (tokens.size() < 1 || tokens.size() > 3) { setLastError("Joint ID=\""s + name() +"\" TargetAngles size out of range"s); return lastErrorPtr(); }
    switch (tokens.size())
    {
    case 1:
        this->SetTargetAngles(GSUtil::Double(tokens[0]));
        break;
    case 2:
        this->SetTargetAngles(GSUtil::Double(tokens[0]), GSUtil::Double(tokens[1]));
        break;
    case 3:
        this->SetTargetAngles(GSUtil::Double(tokens[0]), GSUtil::Double(tokens[1]), GSUtil::Double(tokens[2]));
        break;
    }

    if (findAttribute("ReverseBodyOrderInCalculations"s, &buf)) this->setReverseBodyOrderInCalculations(GSUtil::Bool(buf));

    return nullptr;
}

void AMotorJoint::appendToAttributes()
{
    Joint::appendToAttributes();
    std::string buf;
    buf.reserve(256);
    setAttribute("Type"s, "AMotor"s);
    setAttribute("MaxTorque"s, *GSUtil::ToString(m_maxTorque, &buf));
    setAttribute("TargetAngleGain"s, *GSUtil::ToString(m_targetAngleGain, &buf));
    setAttribute("TargetAngles"s, *GSUtil::ToString(m_targetAnglesList.data(), m_targetAnglesList.size(), &buf));
    setAttribute("ReverseBodyOrderInCalculations"s, *GSUtil::ToString(m_reverseBodyOrderInCalculations, &buf));
}

std::string AMotorJoint::dumpToString()
{
    std::stringstream ss;
    ss.precision(17);
    ss.setf(std::ios::scientific);
    if (firstDump())
    {
        setFirstDump(false);
        ss << "Time\tFX1\tFY1\tFZ1\tTX1\tTY1\tTZ1\tFX2\tFY2\tFZ2\tTX2\tTY2\tTZ2\t"
              "angularVelocity\taxisTorque\taxisPower\ttargetVelocity\t"
              "currentAngle\tcurrentAxis.x\tcurrentAxis.y\tcurrentAxis.z\t"
              "deltaAngle\tdeltaAxis.x\tdeltaAxis.y\tdeltaAxis.z\t"
              "\n";
    }

    pgd::Vector3 currentAxis;
    double currentAngle;
    pgd::MakeAxisAngleFromQ(m_currentQuaternion, &currentAxis.x, &currentAxis.y, &currentAxis.z, &currentAngle);

    pgd::Vector3 lastToCurrentAxis;
    double deltaAngle;
    pgd::MakeAxisAngleFromQ(m_lastToCurrent, &lastToCurrentAxis.x, &lastToCurrentAxis.y, &lastToCurrentAxis.z, &deltaAngle);
    double angularVelocity = deltaAngle / simulation()->GetTimeIncrement(); // note this value will not necessarily have the correct sign

    pgd::Vector3 axis;
    // dJointGetAMotorAxis(JointID(), 0, axis); // get the world axis orientation
    // double targetVelocity = dJointGetAMotorParam(JointID(), dParamVel1);
    // double axisTorque = dDOT(axis, JointFeedback()->t1);

    // ss << simulation()->GetTime() << "\t" <<
    //       JointFeedback()->f1[0] << "\t" << JointFeedback()->f1[1] << "\t" << JointFeedback()->f1[2] << "\t" <<
    //       JointFeedback()->t1[0] << "\t" << JointFeedback()->t1[1] << "\t" << JointFeedback()->t1[2] << "\t" <<
    //       JointFeedback()->f2[0] << "\t" << JointFeedback()->f2[1] << "\t" << JointFeedback()->f2[2] << "\t" <<
    //       JointFeedback()->t2[0] << "\t" << JointFeedback()->t2[1] << "\t" << JointFeedback()->t2[2] << "\t" <<
    //       angularVelocity << "\t" << axisTorque << "\t" << targetVelocity * axisTorque << "\t" << targetVelocity << "\t" <<
    //       currentAngle << "\t" << currentAxis.x << "\t" << currentAxis.y << "\t" << currentAxis.z << "\t" <<
    //       m_deltaAngle << "\t" << m_deltaAxis.x << "\t" << m_deltaAxis.y << "\t" << m_deltaAxis.z << "\t" <<
    //       "\n";
    return ss.str();
}



