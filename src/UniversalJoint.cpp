/*
 *  UniversalJoint.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 21/12/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#include "UniversalJoint.h"
#include "Simulation.h"
#include "Marker.h"
#include "GSUtil.h"

#include <sstream>

using namespace std::string_literals;

UniversalJoint::UniversalJoint() : Joint()
{
}

std::string *UniversalJoint::createFromAttributes()
{
    if (Joint::createFromAttributes()) return lastErrorPtr();
    std::string buf;

    pgd::Vector3 position = body1Marker()->GetWorldPosition();
    this->setAnchor(position);
    pgd::Vector3 x, y, z;
    body1Marker()->GetWorldBasis(&x, &y, &z);
    this->setAxis0(x);
    this->setAxis1(y);
    // if (CFM() >= 0) dJointSetUniversalParam (JointID(), dParamCFM, CFM());
    // if (ERP() >= 0) dJointSetUniversalParam (JointID(), dParamERP, ERP());

    if (findAttribute("LowStop1"s, &buf) == nullptr) return lastErrorPtr();
    double loStop1 = GSUtil::GetAngle(buf);
    if (findAttribute("HighStop1"s, &buf) == nullptr) return lastErrorPtr();
    double hiStop1 = GSUtil::GetAngle(buf);
    if (loStop1 >= hiStop1)
    {
        setLastError("Universal ID=\""s + name() +"\" LowStop1 >= HighStop1"s);
        return lastErrorPtr();
    }
    this->setStops0(pgd::Vector2(loStop1, hiStop1));

    if (findAttribute("LowStop2"s, &buf) == nullptr) return lastErrorPtr();
    double loStop2 = GSUtil::GetAngle(buf);
    if (findAttribute("HighStop2"s, &buf) == nullptr) return lastErrorPtr();
    double hiStop2 = GSUtil::GetAngle(buf);
    if (loStop2 >= hiStop2)
    {
        setLastError("Universal ID=\""s + name() +"\" LowStop2 >= HighStop2"s);
        return lastErrorPtr();
    }
    this->setStops1(pgd::Vector2(loStop2, hiStop2));

    if (findAttribute("StopSpring1"s, &buf))
    {
        this->setStop0Spring(GSUtil::Double(buf));
        if (findAttribute("StopDamp1"s, &buf) == nullptr) return lastErrorPtr();
        this->setStop0Damp(GSUtil::Double(buf));
    }
    if (findAttribute("StopSpring2"s, &buf))
    {
        this->setStop1Spring(GSUtil::Double(buf));
        if (findAttribute("StopDamp2"s, &buf) == nullptr) return lastErrorPtr();
        this->setStop1Damp(GSUtil::Double(buf));
    }

    if (findAttribute("StopBounce1"s, &buf)) this->setStop0Bounce(GSUtil::Double(buf));
    if (findAttribute("StopBounce2"s, &buf)) this->setStop1Bounce(GSUtil::Double(buf));

    return nullptr;
}

void UniversalJoint::appendToAttributes()
{
    Joint::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Universal"s);
    setAttribute("LowStop1"s, *GSUtil::ToString(m_stops0[0], &buf));
    setAttribute("HighStop1"s, *GSUtil::ToString(m_stops0[1], &buf));
    setAttribute("LowStop2"s, *GSUtil::ToString(m_stops1[0], &buf));
    setAttribute("HighStop2"s, *GSUtil::ToString(m_stops1[1], &buf));
    if (m_stop0Spring > 0) setAttribute("StopSpring1"s, *GSUtil::ToString(m_stop0Spring, &buf));
    if (m_stop0Damp > 0) setAttribute("StopDamp1"s, *GSUtil::ToString(m_stop0Damp, &buf));
    setAttribute("StopBounce1"s, *GSUtil::ToString(m_stop0Bounce, &buf));
    if (m_stop1Spring > 0) setAttribute("StopSpring2"s, *GSUtil::ToString(m_stop0Spring, &buf));
    if (m_stop1Damp > 0) setAttribute("StopDamp2"s, *GSUtil::ToString(m_stop0Damp, &buf));
    setAttribute("StopBounce2"s, *GSUtil::ToString(m_stop1Bounce, &buf));
}

pgd::Vector3 UniversalJoint::axis0() const
{
    return m_axis0;
}

void UniversalJoint::setAxis0(const pgd::Vector3 &newAxis0)
{
    m_axis0 = newAxis0;
}

pgd::Vector3 UniversalJoint::axis1() const
{
    return m_axis1;
}

void UniversalJoint::setAxis1(const pgd::Vector3 &newAxis1)
{
    m_axis1 = newAxis1;
}

pgd::Vector3 UniversalJoint::anchor() const
{
    return m_anchor;
}

void UniversalJoint::setAnchor(const pgd::Vector3 &newAnchor)
{
    m_anchor = newAnchor;
}

pgd::Vector2 UniversalJoint::stops0() const
{
    return m_stops0;
}

void UniversalJoint::setStops0(const pgd::Vector2 &newStops0)
{
    m_stops0 = newStops0;
}

double UniversalJoint::stop0Spring() const
{
    return m_stop0Spring;
}

void UniversalJoint::setStop0Spring(double newStop0Spring)
{
    m_stop0Spring = newStop0Spring;
}

double UniversalJoint::stop0Damp() const
{
    return m_stop0Damp;
}

void UniversalJoint::setStop0Damp(double newStop0Damp)
{
    m_stop0Damp = newStop0Damp;
}

double UniversalJoint::stop0Bounce() const
{
    return m_stop0Bounce;
}

void UniversalJoint::setStop0Bounce(double newStop0Bounce)
{
    m_stop0Bounce = newStop0Bounce;
}

pgd::Vector2 UniversalJoint::stops1() const
{
    return m_stops1;
}

void UniversalJoint::setStops1(const pgd::Vector2 &newStops1)
{
    m_stops1 = newStops1;
}

double UniversalJoint::stop1Spring() const
{
    return m_stop1Spring;
}

void UniversalJoint::setStop1Spring(double newStop1Spring)
{
    m_stop1Spring = newStop1Spring;
}

double UniversalJoint::stop1Damp() const
{
    return m_stop1Damp;
}

void UniversalJoint::setStop1Damp(double newStop1Damp)
{
    m_stop1Damp = newStop1Damp;
}

double UniversalJoint::stop1Bounce() const
{
    return m_stop1Bounce;
}

void UniversalJoint::setStop1Bounce(double newStop1Bounce)
{
    m_stop1Bounce = newStop1Bounce;
}

std::string UniversalJoint::dumpToString()
{
    std::stringstream ss;
#ifdef FIX_ME
    ss.precision(17);
    ss.setf(std::ios::scientific);
    if (firstDump())
    {
        setFirstDump(false);
        ss << "Time\tXP\tYP\tZP\tXA1\tYA1\tZA1\tAngle1\tAngleRate1\tXA2\tYA2\tZA2\tAngle2\tAngleRate2\tFX1\tFY1\tFZ1\tTX1\tTY1\tTZ1\tFX2\tFY2\tFZ2\tTX2\tTY2\tTZ2\n";
    }
    pgd::Vector3 p, a1, a2;
    GetUniversalAnchor(p);
    GetUniversalAxis1(a1);
    GetUniversalAxis2(a2);

    ss << simulation()->GetTime() << "\t" << p[0] << "\t" << p[1] << "\t" << p[2] << "\t" <<
          a1[0] << "\t" << a1[1] << "\t" << a1[2] << "\t" << GetUniversalAngle1() << "\t" << GetUniversalAngle1Rate() << "\t" <<
          a2[0] << "\t" << a2[1] << "\t" << a2[2] << "\t" << GetUniversalAngle2() << "\t" << GetUniversalAngle2Rate() << "\t" <<
          JointFeedback()->f1[0] << "\t" << JointFeedback()->f1[1] << "\t" << JointFeedback()->f1[2] << "\t" <<
          JointFeedback()->t1[0] << "\t" << JointFeedback()->t1[1] << "\t" << JointFeedback()->t1[2] << "\t" <<
          JointFeedback()->f2[0] << "\t" << JointFeedback()->f2[1] << "\t" << JointFeedback()->f2[2] << "\t" <<
          JointFeedback()->t2[0] << "\t" << JointFeedback()->t2[1] << "\t" << JointFeedback()->t2[2] <<
          "\n";
#endif
    return ss.str();
}



