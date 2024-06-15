/*
 *  BallJoint.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 29/12/2008.
 *  Copyright 2008 Bill Sellers. All rights reserved.
 *
 */

#include "BallJoint.h"
#include "Body.h"
#include "Simulation.h"
#include "GSUtil.h"
#include "Marker.h"

#include <sstream>

using namespace std::string_literals;
namespace GaitSym {

BallJoint::BallJoint() : Joint()
{
}

// get the quaternion that rotates from body1 to body2
pgd::Quaternion BallJoint::CalculateQuaternion()
{
    pgd::Quaternion q;
    body2Marker()->GetBody()->GetRelativeQuaternion(body1Marker()->GetBody(), &q);
    return q;
}


std::string *BallJoint::createFromAttributes()
{
    if (Joint::createFromAttributes()) return lastErrorPtr();
    std::string buf;

    pgd::Vector3 position = body1Marker()->GetWorldPosition();
    this->setAnchor(position);
    pgd::Vector3 x, y, z;

    if (!(findAttribute("LowStops"s, &buf) == nullptr && findAttribute("HighStops"s, &buf) == nullptr)) // if one of these is undefined, they both need to be undefined
    {
        if (findAttribute("LowStops"s, &buf) == nullptr)
        {
            setLastError("Ball ID=\""s + name() +"\" LowStops missing"s);
            return lastErrorPtr();
        }
        std::vector<double> d1;
        GSUtil::Double(buf, &d1);
        if (d1.size() != 3)
        {
            setLastError("Ball ID=\""s + name() +"\" LowStops needs 3 values"s);
            return lastErrorPtr();
        }
        if (findAttribute("HighStops"s, &buf) == nullptr)
        {
            setLastError("Ball ID=\""s + name() +"\" HighStops missing"s);
            return lastErrorPtr();
        }
        std::vector<double> d2;
        GSUtil::Double(buf, &d2);
        if (d2.size() != 3)
        {
            setLastError("Ball ID=\""s + name() +"\" HighStops needs 3 values"s);
            return lastErrorPtr();
        }
        std::array<pgd::Vector2, 3> stops;
        for (size_t i = 0; i < 3; i++)
        {
            stops[i].x = d1[i];
            stops[i].y = d2[i];
            if (stops[i].y <= stops[i].x)
            {
                setLastError("Ball ID=\""s + name() +"\" HighStops must be larger than LowStops"s);
                return lastErrorPtr();
            }
        }
        m_stops.emplace(std::move(stops));
    }
    return nullptr;
}

void BallJoint::appendToAttributes()
{
    Joint::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Ball"s);
    setAttribute("Body1MarkerID"s, body1Marker()->name());
    setAttribute("Body2MarkerID"s, body2Marker()->name());
    if (m_stops)
    {
        setAttribute("LowStops"s, GSUtil::ToString("%.17g %.17g %.17g", (*m_stops)[0].x, (*m_stops)[1].x, (*m_stops)[2].x));
        setAttribute("HighStops"s, GSUtil::ToString("%.17g %.17g %.17g", (*m_stops)[0].y, (*m_stops)[1].y, (*m_stops)[2].y));
    }
}

pgd::Vector3 BallJoint::anchor() const
{
    return m_anchor;
}

void BallJoint::setAnchor(const pgd::Vector3 &newAnchor)
{
    m_anchor = newAnchor;
}

std::optional<std::array<pgd::Vector2, 3> > BallJoint::stops() const
{
    return m_stops;
}

void BallJoint::setStops(std::optional<std::array<pgd::Vector2, 3> > newStops)
{
    m_stops = newStops;
}

std::string BallJoint::dumpToString()
{
    std::stringstream ss;
    // ss.precision(17);
    // ss.setf(std::ios::scientific);
    // if (firstDump())
    // {
    //     setFirstDump(false);
    //     ss << "Time\tXP\tYP\tZP\ttheta0\ttheta1\ttheta2\tFX1\tFY1\tFZ1\tTX1\tTY1\tTZ1\tFX2\tFY2\tFZ2\tTX2\tTY2\tTZ2\tMotorFX1\tMotorFY1\tMotorFZ1\tMotorTX1\tMotorTY1\tMotorTZ1\tMotorFX2\tMotorFY2\tMotorFZ2\tMotorTX2\tMotorTY2\tMotorTZ2\n";
    // }
    // pgd::Vector3 p;
    // GetBallAnchor(p);
    // double theta0 = dJointGetAMotorAngle(m_MotorJointID, 0);
    // double theta1 = dJointGetAMotorAngle(m_MotorJointID, 1);
    // double theta2 = dJointGetAMotorAngle(m_MotorJointID, 2);

    // ss << simulation()->GetTime() << "\t" << p[0] << "\t" << p[1] << "\t" << p[2] << "\t" <<
    //       theta0 << "\t" << theta1 << "\t" << theta2 << "\t" <<
    //       JointFeedback()->f1[0] << "\t" << JointFeedback()->f1[1] << "\t" << JointFeedback()->f1[2] << "\t" <<
    //       JointFeedback()->t1[0] << "\t" << JointFeedback()->t1[1] << "\t" << JointFeedback()->t1[2] << "\t" <<
    //       JointFeedback()->f2[0] << "\t" << JointFeedback()->f2[1] << "\t" << JointFeedback()->f2[2] << "\t" <<
    //       JointFeedback()->t2[0] << "\t" << JointFeedback()->t2[1] << "\t" << JointFeedback()->t2[2] << "\t" <<
    //       m_MotorJointFeedback.f1[0] << "\t" << m_MotorJointFeedback.f1[1] << "\t" << m_MotorJointFeedback.f1[2] << "\t" <<
    //       m_MotorJointFeedback.t1[0] << "\t" << m_MotorJointFeedback.t1[1] << "\t" << m_MotorJointFeedback.t1[2] << "\t" <<
    //       m_MotorJointFeedback.f2[0] << "\t" << m_MotorJointFeedback.f2[1] << "\t" << m_MotorJointFeedback.f2[2] << "\t" <<
    //       m_MotorJointFeedback.t2[0] << "\t" << m_MotorJointFeedback.t2[1] << "\t" << m_MotorJointFeedback.t2[2] << "\t" <<
    //       "\n";
    return ss.str();
}



} // namespace GaitSym


