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
#include "GSUtil.h"
#include "Simulation.h"

using namespace std::string_literals;

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
    return m_body1Marker->GetBody();
}

Body *Joint::body2() const
{
    return m_body2Marker->GetBody();
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

    if (findAttribute("Body1MarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto marker1Iterator = simulation()->GetMarkerList()->find(buf);
    if (marker1Iterator == simulation()->GetMarkerList()->end())
    {
        setLastError("Joint ID=\""s + name() +"\" Body1Marker not found"s);
        return lastErrorPtr();
    }
    if (findAttribute("Body2MarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto marker2Iterator = simulation()->GetMarkerList()->find(buf);
    if (marker2Iterator == simulation()->GetMarkerList()->end())
    {
        setLastError("Joint ID=\""s + name() +"\" Body2Marker not found"s);
        return lastErrorPtr();
    }
//    if (marker1Iterator->second->GetBody() == nullptr)
//    {
//        setLastError("Joint ID=\""s + name() +"\" Marker 1 cannot be attached to World"s);
//        return lastErrorPtr();
//    }
    if (marker1Iterator->second->GetBody() == nullptr && marker2Iterator->second->GetBody() == nullptr)
    {
        setLastError("Joint ID=\""s + name() +"\" both markers attached to World"s);
        return lastErrorPtr();
    }
    this->setBody1Marker(marker1Iterator->second.get());
    this->setBody2Marker(marker2Iterator->second.get());

    if (findAttribute("CFM"s, &buf)) m_CFM = GSUtil::Double(buf);
    if (findAttribute("ERP"s, &buf)) m_ERP = GSUtil::Double(buf);

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
    setAttribute("Body1MarkerID"s, body1Marker()->name());
    setAttribute("Body2MarkerID"s, body2Marker()->name());
    if (m_CFM >= 0) setAttribute("CFM"s, *GSUtil::ToString(m_CFM, &buf));
    if (m_ERP >= 0) setAttribute("ERP"s, *GSUtil::ToString(m_ERP, &buf));
}

double Joint::CFM() const
{
    return m_CFM;
}

void Joint::setCFM(double CFM)
{
    m_CFM = CFM;
}

double Joint::ERP() const
{
    return m_ERP;
}

void Joint::setERP(double ERP)
{
    m_ERP = ERP;
}

