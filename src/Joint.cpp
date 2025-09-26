/*
 *  Joint.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 19/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#include "Joint.h"
#include "Marker.h"
#include "Body.h"
#include "GSUtil.h"
#include "Simulation.h"

using namespace std::string_literals;
namespace GaitSym {

Joint::Joint()
{
}

Marker *Joint::body1Marker() const
{
    return m_body1Marker;
}

void Joint::setBody1Marker(Marker *body1Marker)
{
    m_body1Marker = body1Marker;
}

Marker *Joint::body2Marker() const
{
    return m_body2Marker;
}

Body *Joint::body1() const
{
    return m_body1Marker->body();
}

Body *Joint::body2() const
{
    return m_body2Marker->body();
}

void Joint::setBody2Marker(Marker *body2Marker)
{
    m_body2Marker = body2Marker;
}

// this function initialises the data in the object based on the contents
// of an xml_node node. It uses information from the simulation as required
// to satisfy dependencies
// it returns nullptr on success and a pointer to lastError() on failure
std::string *Joint::createFromAttributes()
{
    if (NamedObject::createFromAttributes()) return lastErrorPtr();
    std::string buf;

    if (findAttribute("Type"s, &buf) == nullptr) return lastErrorPtr();
    m_type = buf;

    if (findAttribute("Body1MarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto marker1Iterator = simulation()->markerList()->find(buf);
    if (marker1Iterator == simulation()->markerList()->end())
    {
        setLastError("Joint ID=\""s + name() +"\" Body1Marker not found"s);
        return lastErrorPtr();
    }
    if (findAttribute("Body2MarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto marker2Iterator = simulation()->markerList()->find(buf);
    if (marker2Iterator == simulation()->markerList()->end())
    {
        setLastError("Joint ID=\""s + name() +"\" Body2Marker not found"s);
        return lastErrorPtr();
    }
    if (marker1Iterator->second->body() ==  marker2Iterator->second->body())
    {
        setLastError("Joint ID=\""s + name() +"\" both markers attached to the same body ID=\""s + marker1Iterator->second->body()->name() + "\""s);
        return lastErrorPtr();
    }

    // these checks use the construction positions and rotations (body rotations are always zero at construction)
    pgd::Vector3 distanceVector = marker2Iterator->second->constructionPosition() - marker1Iterator->second->constructionPosition();
    pgd::Quaternion rotationQuaternion = pgd::findRotation(marker1Iterator->second->quaternion(), marker2Iterator->second->quaternion());
    double testEpsilon = std::numeric_limits<double>::epsilon() * 100.0;
    if (distanceVector.magnitude2() > testEpsilon)
    {
        setLastError(GSUtil::toString("Joint ID=\"%s\" marker distance is too large: Magnitude2() = %g limit = %g", name().c_str(), distanceVector.magnitude2(), testEpsilon));
        return lastErrorPtr();
    }
    if (pgd::qGetAngle(rotationQuaternion) > testEpsilon)
    {
        setLastError(GSUtil::toString("Joint ID=\"%s\" marker rotation is too large: QGetAngle() = %g limit = %g", name().c_str(), pgd::qGetAngle(rotationQuaternion), testEpsilon));
        return lastErrorPtr();
    }

    this->setBody1Marker(marker1Iterator->second.get());
    this->setBody2Marker(marker2Iterator->second.get());

    if (findAttribute("CFM"s, &buf)) m_cfm = GSUtil::toDouble(buf);
    if (findAttribute("ERP"s, &buf)) m_erp = GSUtil::toDouble(buf);

    setUpstreamObjects({m_body1Marker, m_body2Marker});
    return nullptr;
}

// this function copies the data in the object to an xml_node node that it creates internally.
// doc is used to allocate the memory so deletion should be automatic
void Joint::saveToAttributes()
{
    this->setTag("JOINT"s);
    this->clearAttributeMap();
    this->appendToAttributes();
}

// this function appends data to a pre-existing xml_node - often created by XMLSave
void Joint::appendToAttributes()
{
    NamedObject::appendToAttributes();
    std::string buf;
    setAttribute("Type", type());
    setAttribute("Body1MarkerID"s, body1Marker()->name());
    setAttribute("Body2MarkerID"s, body2Marker()->name());
    if (m_cfm >= 0) setAttribute("CFM"s, *GSUtil::toString(m_cfm, &buf));
    if (m_erp >= 0) setAttribute("ERP"s, *GSUtil::toString(m_erp, &buf));
}

double Joint::CFM() const
{
    return m_cfm;
}

void Joint::setCFM(double CFM)
{
    m_cfm = CFM;
}

double Joint::ERP() const
{
    return m_erp;
}

void Joint::setERP(double ERP)
{
    m_erp = ERP;
}

pgd::Vector3 Joint::worldDistance() const
{
    pgd::Vector3 result = m_body2Marker->worldPosition() - m_body1Marker->worldPosition();
    return result;
}

pgd::Quaternion Joint::worldRotation() const
{
    pgd::Quaternion result = pgd::findRotation(m_body1Marker->worldQuaternion(), m_body2Marker->worldQuaternion());
    // pgd::Quaternion result2 = pgd::FindRotation(m_body1Marker->GetBody()->GetQuaternion(), m_body2Marker->GetBody()->GetQuaternion()); // just checking that using the marker gives the same result as usning the body
    return result;
}

pgd::Vector3 Joint::worldLinearVelocity() const
{
    pgd::Vector3 result = m_body2Marker->worldLinearVelocity() - m_body1Marker->worldLinearVelocity();
    return result;
}

pgd::Vector3 Joint::worldAngularVelocity() const
{
    pgd::Vector3 result = m_body2Marker->worldAngularVelocity() - m_body1Marker->worldAngularVelocity();
    return result;
}



pgd::Vector3 Joint::force() const
{
    return m_force;
}

void Joint::setForce(const pgd::Vector3 &newForce)
{
    m_force = newForce;
}

pgd::Vector3 Joint::torque() const
{
    return m_torque;
}

void Joint::setTorque(const pgd::Vector3 &newTorque)
{
    m_torque = newTorque;
}

std::string Joint::type() const
{
    return m_type;
}



} // namespace GaitSym


