/*
 *  LMotorJoint.h
 *  GaitSym2019
 *
 *  Created by Bill Sellers on 23/10/2019.
 *  Copyright 2019 Bill Sellers. All rights reserved.
 *
 */

#include "LMotorJoint.h"
#include "Simulation.h"
#include "Body.h"
#include "PGDMath.h"
#include "Marker.h"
#include "GSUtil.h"

#include <sstream>

namespace GaitSym {

LMotorJoint::LMotorJoint() : Joint()
{
}

void LMotorJoint::setNumAxes(int numAxes)
{
    m_numAxes = numAxes;
}

int LMotorJoint::numAxes(void)
{
    return m_numAxes;
}

double LMotorJoint::position(int anum)
{
    pgd::Vector3 relativePositionWorld = pgd::Vector3(body2Marker()->GetWorldPosition()) - pgd::Vector3(body1Marker()->GetWorldPosition());
    pgd::Vector3 relativePositionBody1Marker(relativePositionWorld);
    // not 100% sure this should be the inverse of the body rotation here
    if (body1Marker()->GetBody()) relativePositionBody1Marker = pgd::QVRotate(~pgd::Quaternion(body1Marker()->GetBody()->GetQuaternion()), relativePositionWorld);
    switch (anum)
    {
    case 0:
        return relativePositionBody1Marker.x;
    case 1:
        return relativePositionBody1Marker.y;
    case 2:
        return relativePositionBody1Marker.z;
    default:
#ifndef NDEBUG
        std::cerr << "Warning: LMotorJoint::position anum out of range, setting to 0\n";
#endif
        return relativePositionBody1Marker.x;
    }
}

void LMotorJoint::getPositions(double *x, double *y, double *z)
{
    pgd::Vector3 relativePositionWorld = pgd::Vector3(body2Marker()->GetWorldPosition()) - pgd::Vector3(body1Marker()->GetWorldPosition());
    pgd::Vector3 relativePositionBody1Marker(relativePositionWorld);
    // not 100% sure this should be the inverse of the body rotation here
    if (body1Marker()->GetBody()) relativePositionBody1Marker = pgd::QVRotate(~pgd::Quaternion(body1Marker()->GetBody()->GetQuaternion()), relativePositionWorld);
    *x = relativePositionBody1Marker.x;
    *y = relativePositionBody1Marker.y;
    *z = relativePositionBody1Marker.z;
}

double LMotorJoint::positionRate(int anum)
{
    switch (anum)
    {
    case 0:
        return m_lastPositionRate0;
    case 1:
        return m_lastPositionRate1;
    case 2:
        return m_lastPositionRate2;
    default:
#ifndef NDEBUG
        std::cerr << "Warning: LMotorJoint::positionRate anum out of range, setting to 0\n";
#endif
        return m_lastPositionRate0;
    }
}

double LMotorJoint::targetPosition(int anum)
{
    switch (anum)
    {
    case 0:
        return m_targetPosition0;
    case 1:
        return m_targetPosition1;
    case 2:
        return m_targetPosition2;
    default:
#ifndef NDEBUG
        std::cerr << "Warning: LMotorJoint::targetPosition anum out of range, setting to 0\n";
#endif
        return m_targetPosition0;
    }
}

void LMotorJoint::setStops(int anum, double low, double high)
{
    // note there is safety feature that stops setting incompatible low and high
    // stops which can cause difficulties. The safe option is to set them twice.

    switch (anum)
    {
    case 0:
        m_stops0.Set(low, high);
        m_stopsSet0 = true;
        break;
    case 1:
        m_stops1.Set(low, high);
        m_stopsSet1 = true;
        break;
    case 2:
        m_stops2.Set(low, high);
        m_stopsSet2 = true;
        break;
    default:
#ifndef NDEBUG
        std::cerr << "Warning: LMotorJoint::setStops anum out of range, setting to 0\n";
#endif
        m_stops0.Set(low, high);
        m_stopsSet0 = true;
        break;
    }
}

// this is the target velocity for the motor
void LMotorJoint::setTargetVelocity(int anum, double targetVelocity)
{
    switch (anum)
    {
    case 0:
        m_targetVelocity0 = targetVelocity;
        break;
    case 1:
        m_targetVelocity1 = targetVelocity;
        break;
    case 2:
        m_targetVelocity2 = targetVelocity;
        break;
    default:
#ifndef NDEBUG
        std::cerr << "Warning: LMotorJoint::setTargetVelocity anum out of range, setting to 0\n";
#endif
        m_targetVelocity0 = targetVelocity;
        break;
    }
}

// this is the maximum force the motor can generate to achieve the
// desired velocity. Note this isn't used for stops which can generate
// a *lot* of force.
// set to zero to turn off the motor bit (again stops still work if set)
void LMotorJoint::setMaxForce(int anum, double maximumForce)
{
    switch (anum)
    {
    case 0:
        m_maxForce0 = maximumForce;
        break;
    case 1:
        m_maxForce1 = maximumForce;
        break;
    case 2:
        m_maxForce2 = maximumForce;
        break;
    default:
#ifndef NDEBUG
        std::cerr << "Warning: LMotorJoint::setMaxForce anum out of range, setting to 0\n";
#endif
        m_maxForce0 = maximumForce;
        break;
    }
}

void LMotorJoint::setDynamicFriction(double dynamicFrictionIntercept, double dynamicFrictionSlope)
{
    m_dynamicFrictionIntercept = dynamicFrictionIntercept;
    m_dynamicFrictionSlope = dynamicFrictionSlope;
    m_dynamicFrictionFlag = true;

    setTargetVelocity(0, 0);
    setTargetVelocity(1, 0);
    setTargetVelocity(2, 0);
    SetDynamicFriction();
}

void LMotorJoint::SetPosition(int anum, double position, double time)
{
    switch (anum)
    {
    case 0:
        if (!m_lastTimeValid0) m_lastTimeValid0 = true;
        else m_lastPositionRate0 = (position - m_lastPosition0) / (time - m_lastTime0);
        m_lastTime0 = time;
        m_lastPosition0 = position;
        break;
    case 1:
        if (!m_lastTimeValid1) m_lastTimeValid1 = true;
        else m_lastPositionRate1 = (position - m_lastPosition1) / (time - m_lastTime1);
        m_lastTime1 = time;
        m_lastPosition1 = position;
        break;
    case 2:
        if (!m_lastTimeValid2) m_lastTimeValid2 = true;
        else m_lastPositionRate2 = (position - m_lastPosition2) / (time - m_lastTime2);
        m_lastTime2 = time;
        m_lastPosition2 = position;
        break;
    default:
#ifndef NDEBUG
        std::cerr << "Warning: LMotorJoint::setPosition anum out of range, setting to 0\n";
#endif
        if (!m_lastTimeValid0) m_lastTimeValid0 = true;
        else m_lastPositionRate0 = (position - m_lastPosition0) / (time - m_lastTime0);
        m_lastTime0 = time;
        m_lastPosition0 = position;
        break;
    }
}

void LMotorJoint::setTargetPosition(int anum, double targetPosition)
{
    switch(anum)
    {
    case 0:
        m_targetPosition0 = targetPosition;
        m_targetPositionSet0 = true;
        break;
    case 1:
        m_targetPosition1 = targetPosition;
        m_targetPositionSet1 = true;
        break;
    case 2:
        m_targetPosition2 = targetPosition;
        m_targetPositionSet2 = true;
        break;
    default:
#ifndef NDEBUG
        std::cerr << "Warning: LMotorJoint::setTargetPosition anum out of range, setting to 0\n";
#endif
        m_targetPosition0 = targetPosition;
        m_targetPositionSet0 = true;
        break;
    }
}

void LMotorJoint::setTargetPositionGain(int anum, double targetPositionGain)
{
    switch(anum)
    {
    case 0:
        m_targetPositionGain0 = targetPositionGain;
        break;
    case 1:
        m_targetPositionGain1 = targetPositionGain;
        break;
    case 2:
        m_targetPositionGain2 = targetPositionGain;
        break;
    default:
#ifndef NDEBUG
        std::cerr << "Warning: LMotorJoint::setTargetPositionGain anum out of range, setting to 0\n";
#endif
        m_targetPositionGain0 = targetPositionGain;
        break;
    }
}

void LMotorJoint::SetDynamicFriction()
{
    double maximumForce;
    maximumForce = m_dynamicFrictionIntercept + m_dynamicFrictionSlope * std::fabs(positionRate(0));
    setMaxForce(0, maximumForce);
    maximumForce = m_dynamicFrictionIntercept + m_dynamicFrictionSlope * std::fabs(positionRate(1));
    setMaxForce(1, maximumForce);
    maximumForce = m_dynamicFrictionIntercept + m_dynamicFrictionSlope * std::fabs(positionRate(2));
    setMaxForce(2, maximumForce);
}

void LMotorJoint::Update()
{
    double position0, position1, position2, delPosition;
    getPositions(&position0, &position1, &position2);
    double time = simulation()->GetTime();
    switch (m_numAxes)
    {
    case 1:
        if (m_targetPositionSet0)
        {
            delPosition = m_targetPosition0 - position0;
            setTargetVelocity(0, delPosition * m_targetPositionGain0);
        }
        SetPosition(0, position0, time);
        break;
    case 2:
        if (m_targetPositionSet0)
        {
            delPosition = m_targetPosition0 - position0;
            setTargetVelocity(0, delPosition * m_targetPositionGain0);
        }
        if (m_targetPositionSet1)
        {
            delPosition = m_targetPosition1 - position1;
            setTargetVelocity(1, delPosition * m_targetPositionGain1);
        }
        SetPosition(0, position0, time);
        SetPosition(1, position1, time);
        break;
    case 3:
        if (m_targetPositionSet0)
        {
            delPosition = m_targetPosition0 - position0;
            setTargetVelocity(0, delPosition * m_targetPositionGain0);
        }
        if (m_targetPositionSet1)
        {
            delPosition = m_targetPosition1 - position1;
            setTargetVelocity(1, delPosition * m_targetPositionGain1);
        }
        if (m_targetPositionSet2)
        {
            delPosition = m_targetPosition2 - position2;
            setTargetVelocity(2, delPosition * m_targetPositionGain2);
        }
        SetPosition(0, position0, time);
        SetPosition(1, position1, time);
        SetPosition(2, position2, time);
        break;
    default:
#ifndef NDEBUG
        std::cerr << "Warning: LMotorJoint::setStops anum out of range, setting to 0\n";
#endif
        if (m_targetPositionSet0)
        {
            delPosition = m_targetPosition0 - position0;
            setTargetVelocity(0, delPosition * m_targetPositionGain0);
        }
        SetPosition(0, position0, time);
        break;
    }
}

std::string *LMotorJoint::createFromAttributes()
{
    if (Joint::createFromAttributes()) return lastErrorPtr();
    std::string buf, buf2, buf3;
    // if (CFM() >= 0) dJointSetLMotorParam (JointID(), dParamCFM, CFM());
    // if (ERP() >= 0) dJointSetLMotorParam (JointID(), dParamERP, ERP());

    if (findAttribute("NumAxes"s, &buf) == nullptr) return lastErrorPtr();
    int numAxes = GSUtil::Int(buf);
    if (numAxes < 1 || numAxes > 3) { setLastError("Joint ID=\""s + name() +"\" NumAxes out of range"s); return lastErrorPtr(); }
    this->setNumAxes(numAxes);
    switch (numAxes)
    {
    case 1:
        if (findAttribute("MaxForce0"s, &buf) == nullptr) return lastErrorPtr();
        this->setMaxForce(0, GSUtil::Double(buf));
        findAttribute("LowStop0"s, &buf);
        findAttribute("HighStop0"s, &buf2);
        if (buf.size() || buf2.size())
        {
            if (!(buf.size() && buf2.size())) { setLastError("Joint ID=\""s + name() +"\" both LowStop0 and HighStop0 required"s); return lastErrorPtr(); }
            this->setStops(0, GSUtil::Double(buf), GSUtil::Double(buf2));
        }
        if (findAttribute("TargetPosition0"s, &buf)) this->setTargetPosition(0, GSUtil::Double(buf));
        if (findAttribute("TargetPositionGain0"s, &buf)) this->setTargetPositionGain(0, GSUtil::Double(buf));
        break;
    case 2:
        if (findAttribute("MaxForce0"s, &buf) == nullptr) return lastErrorPtr();
        this->setMaxForce(0, GSUtil::Double(buf));
        if (findAttribute("MaxForce1"s, &buf) == nullptr) return lastErrorPtr();
        this->setMaxForce(1, GSUtil::Double(buf));
        findAttribute("LowStop0"s, &buf);
        findAttribute("HighStop0"s, &buf2);
        if (buf.size() || buf2.size())
        {
            if (!(buf.size() && buf2.size())) { setLastError("Joint ID=\""s + name() +"\" both LowStop0 and HighStop0 required"s); return lastErrorPtr(); }
            this->setStops(0, GSUtil::Double(buf), GSUtil::Double(buf2));
        }
        findAttribute("LowStop1"s, &buf);
        findAttribute("HighStop1"s, &buf2);
        if (buf.size() || buf2.size())
        {
            if (!(buf.size() && buf2.size())) { setLastError("Joint ID=\""s + name() +"\" both LowStop1 and HighStop1 required"s); return lastErrorPtr(); }
            this->setStops(1, GSUtil::Double(buf), GSUtil::Double(buf2));
        }
        if (findAttribute("TargetPosition0"s, &buf)) this->setTargetPosition(0, GSUtil::Double(buf));
        if (findAttribute("TargetPosition1"s, &buf)) this->setTargetPosition(1, GSUtil::Double(buf));
        if (findAttribute("TargetPositionGain0"s, &buf)) this->setTargetPositionGain(0, GSUtil::Double(buf));
        if (findAttribute("TargetPositionGain1"s, &buf)) this->setTargetPositionGain(1, GSUtil::Double(buf));
        break;
    case 3:
        if (findAttribute("MaxForce0"s, &buf) == nullptr) return lastErrorPtr();
        this->setMaxForce(0, GSUtil::Double(buf));
        if (findAttribute("MaxForce1"s, &buf) == nullptr) return lastErrorPtr();
        this->setMaxForce(1, GSUtil::Double(buf));
        if (findAttribute("MaxForce2"s, &buf) == nullptr) return lastErrorPtr();
        this->setMaxForce(2, GSUtil::Double(buf));
        findAttribute("LowStop0"s, &buf);
        findAttribute("HighStop0"s, &buf2);
        if (buf.size() || buf2.size())
        {
            if (!(buf.size() && buf2.size())) { setLastError("Joint ID=\""s + name() +"\" both LowStop0 and HighStop0 required"s); return lastErrorPtr(); }
            this->setStops(0, GSUtil::Double(buf), GSUtil::Double(buf2));
        }
        findAttribute("LowStop1"s, &buf);
        findAttribute("HighStop1"s, &buf2);
        if (buf.size() || buf2.size())
        {
            if (!(buf.size() && buf2.size())) { setLastError("Joint ID=\""s + name() +"\" both LowStop1 and HighStop1 required"s); return lastErrorPtr(); }
            this->setStops(1, GSUtil::Double(buf), GSUtil::Double(buf2));
        }
        findAttribute("LowStop2"s, &buf);
        findAttribute("HighStop2"s, &buf2);
        if (buf.size() || buf2.size())
        {
            if (!(buf.size() && buf2.size())) { setLastError("Joint ID=\""s + name() +"\" both LowStop2 and HighStop2 required"s); return lastErrorPtr(); }
            this->setStops(2, GSUtil::Double(buf), GSUtil::Double(buf2));
        }
        if (findAttribute("TargetPosition0"s, &buf)) this->setTargetPosition(0, GSUtil::Double(buf));
        if (findAttribute("TargetPosition1"s, &buf)) this->setTargetPosition(1, GSUtil::Double(buf));
        if (findAttribute("TargetPosition2"s, &buf)) this->setTargetPosition(2, GSUtil::Double(buf));
        if (findAttribute("TargetPositionGain0"s, &buf)) this->setTargetPositionGain(0, GSUtil::Double(buf));
        if (findAttribute("TargetPositionGain1"s, &buf)) this->setTargetPositionGain(1, GSUtil::Double(buf));
        if (findAttribute("TargetPositionGain2"s, &buf)) this->setTargetPositionGain(2, GSUtil::Double(buf));
        break;
    }

    return nullptr;
}

void LMotorJoint::appendToAttributes()
{
    Joint::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "LMotor"s);
    setAttribute("Body1MarkerID"s, body1Marker()->name());
    setAttribute("Body2MarkerID"s, body2Marker()->name());
    setAttribute("NumAxes"s, *GSUtil::ToString(this->numAxes(), &buf));
    switch (this->numAxes())
    {
    case 1:
        setAttribute("MaxForce0"s, *GSUtil::ToString(m_maxForce0, &buf));
        if (m_stopsSet0)
        {
            setAttribute("LowStop0"s, *GSUtil::ToString(m_stops0[0], &buf));
            setAttribute("HighStop0"s, *GSUtil::ToString(m_stops0[1], &buf));
        }
        if (m_targetPositionSet0)
        {
            setAttribute("TargetPosition0"s, *GSUtil::ToString(m_targetPosition0, &buf));
            setAttribute("TargetPositionGain0"s, *GSUtil::ToString(m_targetPositionGain0, &buf));
        }
        break;
    case 2:
        setAttribute("MaxForce0"s, *GSUtil::ToString(m_maxForce0, &buf));
        setAttribute("MaxForce1"s, *GSUtil::ToString(m_maxForce1, &buf));
        if (m_stopsSet0)
        {
            setAttribute("LowStop0"s, *GSUtil::ToString(m_stops0[0], &buf));
            setAttribute("HighStop0"s, *GSUtil::ToString(m_stops0[1], &buf));
        }
        if (m_stopsSet1)
        {
            setAttribute("LowStop1"s, *GSUtil::ToString(m_stops1[0], &buf));
            setAttribute("HighStop1"s, *GSUtil::ToString(m_stops1[1], &buf));
        }
        if (m_targetPositionSet0)
        {
            setAttribute("TargetPosition0"s, *GSUtil::ToString(m_targetPosition0, &buf));
            setAttribute("TargetPositionGain0"s, *GSUtil::ToString(m_targetPositionGain0, &buf));
        }
        if (m_targetPositionSet1)
        {
            setAttribute("TargetPosition1"s, *GSUtil::ToString(m_targetPosition1, &buf));
            setAttribute("TargetPositionGain1"s, *GSUtil::ToString(m_targetPositionGain1, &buf));
        }
        break;
    case 3:
        setAttribute("MaxForce0"s, *GSUtil::ToString(m_maxForce0, &buf));
        setAttribute("MaxForce1"s, *GSUtil::ToString(m_maxForce1, &buf));
        setAttribute("MaxForce2"s, *GSUtil::ToString(m_maxForce2, &buf));
        if (m_stopsSet0)
        {
            setAttribute("LowStop0"s, *GSUtil::ToString(m_stops0[0], &buf));
            setAttribute("HighStop0"s, *GSUtil::ToString(m_stops0[1], &buf));
        }
        if (m_stopsSet1)
        {
            setAttribute("LowStop1"s, *GSUtil::ToString(m_stops1[0], &buf));
            setAttribute("HighStop1"s, *GSUtil::ToString(m_stops1[1], &buf));
        }
        if (m_stopsSet2)
        {
            setAttribute("LowStop2"s, *GSUtil::ToString(m_stops2[0], &buf));
            setAttribute("HighStop2"s, *GSUtil::ToString(m_stops2[1], &buf));
        }
        if (m_targetPositionSet0)
        {
            setAttribute("TargetPosition0"s, *GSUtil::ToString(m_targetPosition0, &buf));
            setAttribute("TargetPositionGain0"s, *GSUtil::ToString(m_targetPositionGain0, &buf));
        }
        if (m_targetPositionSet1)
        {
            setAttribute("TargetPosition1"s, *GSUtil::ToString(m_targetPosition1, &buf));
            setAttribute("TargetPositionGain1"s, *GSUtil::ToString(m_targetPositionGain1, &buf));
        }
        if (m_targetPositionSet2)
        {
            setAttribute("TargetPosition2"s, *GSUtil::ToString(m_targetPosition2, &buf));
            setAttribute("TargetPositionGain2"s, *GSUtil::ToString(m_targetPositionGain2, &buf));
        }
        break;
    }
}

std::string LMotorJoint::dumpToString()
{
    std::stringstream ss;
#ifdef FIX_ME
    ss.precision(17);
    ss.setf(std::ios::scientific);
    if (firstDump())
    {
        setFirstDump(false);
        ss << "Time\tFX1\tFY1\tFZ1\tTX1\tTY1\tTZ1\tFX2\tFY2\tFZ2\tTX2\tTY2\tTZ2\n";
    }
    ss << simulation()->GetTime() << "\t" <<
          JointFeedback()->f1[0] << "\t" << JointFeedback()->f1[1] << "\t" << JointFeedback()->f1[2] << "\t" <<
          JointFeedback()->t1[0] << "\t" << JointFeedback()->t1[1] << "\t" << JointFeedback()->t1[2] << "\t" <<
          JointFeedback()->f2[0] << "\t" << JointFeedback()->f2[1] << "\t" << JointFeedback()->f2[2] << "\t" <<
          JointFeedback()->t2[0] << "\t" << JointFeedback()->t2[1] << "\t" << JointFeedback()->t2[2] <<
          "\n";
#endif // FIX_ME
return ss.str();
}


} // namespace GaitSym


