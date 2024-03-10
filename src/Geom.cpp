/*
 *  Geom.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 28/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

// Wrapper class to hold ODE geom

#include "Geom.h"
#include "PGDMath.h"
#include "DataFile.h"
#include "Simulation.h"
#include "Body.h"
#include "GSUtil.h"
#include "Contact.h"
#include "Marker.h"
#include "GSUtil.h"
#include "PlaneGeom.h"

#include "pystring.h"

#include <iostream>
#include <sstream>

using namespace std::string_literals;


Geom::Geom()
{
}

// these functions set the geom position relative to its body
// these now use the geom offset functions
void Geom::SetBody(Body *body)
{
    m_body = body;
}

Body *Geom::GetBody()
{
    return m_body;
}

void Geom::SetPosition (double x, double y, double z)
{
    m_position.Set(x, y, z);
}

pgd::Vector3 Geom::GetPosition() const
{
    return m_position;
}

pgd::Vector3 Geom::GetWorldPosition() const
{
    if (m_body)
    {
        // get the position in world coordinates
        //        pgd::Vector3 p;
        //        dBodyGetRelPointPos(m_body->GetBodyID(), m_position.x, m_position.y, m_position.z, p);
        //        return pgd::Vector3(p[0], p[1], p[2]);
        pgd::Vector3 bodyWorldPosition = pgd::QVRotate(m_body->GetQuaternion(), m_position) + pgd::Vector3(m_body->GetPosition());
        return bodyWorldPosition;
    }
    else
    {
        return m_position;
    }
}

void Geom::SetQuaternion(double n, double x, double y, double z)
{
    m_quaternion.Set(n, x, y, z);
}

void Geom::setGeomMarker(Marker *geomMarker)
{
    m_geomMarker = geomMarker;
    if (m_geomMarker->GetBody())
    {
        this->SetGeomLocation(Geom::body);
        this->SetBody(m_geomMarker->GetBody());
    }
    else
    {
        this->SetBody(nullptr);
        this->SetGeomLocation(Geom::environment);

    }
    if (dynamic_cast<PlaneGeom *>(this)) return; // do not try to place non-placeable geoms

    pgd::Vector3 p = geomMarker->GetPosition();
    this->SetPosition(p.x, p.y, p.z);
    pgd::Quaternion q = geomMarker->GetQuaternion();
    this->SetQuaternion(q.n, q.x, q.y, q.z);
}

pgd::Quaternion Geom::GetQuaternion() const
{
    return m_quaternion;
}

pgd::Quaternion Geom::GetWorldQuaternion() const
{
    if (m_body)
    {
        return m_body->GetQuaternion() * m_quaternion;
    }
    else
    {
        return m_quaternion;
    }
}

void Geom::SetSpringDamp(double springConstant, double dampingConstant, double integrationStep)
{
    m_ERP = integrationStep * springConstant/(integrationStep * springConstant + dampingConstant);
    m_CFM = 1/(integrationStep * springConstant + dampingConstant);
    m_SpringConstant = springConstant;
    m_DampingConstant = springConstant;
}

void Geom::SetSpringERP(double springConstant, double ERP, double integrationStep)
{
    m_ERP = ERP;
    m_CFM = ERP / (integrationStep * springConstant);
    m_SpringConstant = springConstant;
    m_DampingConstant = (1.0 - m_ERP) / m_CFM;
}

void Geom::SetSpringCFM(double springConstant, double CFM, double integrationStep)
{
    m_CFM = CFM;
    m_SpringConstant = springConstant;
    m_DampingConstant = 1.0 / m_CFM - integrationStep * m_SpringConstant;
    m_ERP = integrationStep * m_SpringConstant/(integrationStep * m_SpringConstant + m_DampingConstant);
}

void Geom::SetCFMERP(double CFM, double ERP, double integrationStep)
{
    m_ERP = ERP;
    m_CFM = CFM;
    m_SpringConstant = m_ERP / (m_CFM * integrationStep);
    m_DampingConstant = (1.0 - m_ERP) / m_CFM;
}

void Geom::SetCFMDamp(double CFM, double dampingConstant, double integrationStep)
{
    m_CFM = CFM;
    m_DampingConstant = dampingConstant;
    m_SpringConstant = (1.0 / m_CFM - m_DampingConstant) / integrationStep;
    m_ERP = integrationStep * m_SpringConstant/(integrationStep * m_SpringConstant + m_DampingConstant);
}

void Geom::SetERPDamp(double ERP, double dampingConstant, double integrationStep)
{
    m_ERP = ERP;
    m_DampingConstant = dampingConstant;
    m_SpringConstant = m_DampingConstant / (integrationStep / m_ERP - integrationStep);
    m_CFM = 1.0/(integrationStep * m_SpringConstant + m_DampingConstant);
}

std::string Geom::dumpToString()
{
    std::stringstream ss;
    ss.precision(17);
    ss.setf(std::ios::scientific);
    if (firstDump())
    {
        setFirstDump(false);
        ss << "Time\tXP\tYP\tZP\tQW\tQX\tQY\tQZ\tNContacts\tBody1\tBody2\tXC\tYC\tZC\tFX\tFY\tFZ\tTX\tTY\tTZ\n";
    }

    pgd::Vector3 p = m_geomMarker->GetWorldPosition();
    pgd::Quaternion q = m_geomMarker->GetWorldQuaternion();
    ss << simulation()->GetTime() << "\t" << p[0] << "\t" << p[1] << "\t" << p[2] << "\t" << q[0] << "\t" << q[1] << "\t" << q[2] << "\t" << q[3] << "\t" << m_ContactList.size();
    std::string body1, body2;
    for (auto &&iter : m_ContactList)
    {
        if (iter->body1()) body1 = iter->body1()->name();
        else body1 = "World"s;
        if (iter->body2()) body2 = iter->body2()->name();
        else body2 = "World"s;
        ss << "\t" << body1 << "\t" << body2 << "\t";
        pgd::Vector3 p = iter->position();
        ss << p[0] << "\t" << p[1] << "\t" <<p[2] << "\t";
        pgd::Vector3 f = iter->force();
        ss << f[0] << "\t" << f[1] << "\t" <<f[2] << "\t";
        pgd::Vector3 t = iter->torque();
        ss << t[0] << "\t" << t[1] << "\t" <<t[2];
    }
    ss << "\n";
    return ss.str();
}

Marker *Geom::geomMarker() const
{
    return m_geomMarker;
}

// this function initialises the data in the object based on the contents
// of an xml_node node. It uses information from the simulation as required
// to satisfy dependencies
// it returns nullptr on success and a pointer to lastError() on failure
std::string *Geom::createFromAttributes()
{
    if (NamedObject::createFromAttributes()) return lastErrorPtr();
    std::string buf, buf2;
    if (findAttribute("MarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto it = simulation()->GetMarkerList()->find(buf);
    if (it == simulation()->GetMarkerList()->end())
    {
        setLastError("GEOM ID=\""s + name() +"\" Marker not found"s);
        return lastErrorPtr();
    }
    this->setGeomMarker(it->second.get());

    // can specify ERP & CFM; SpringConstant & DampingConstant; SpringConstant & ERP; SpringConstant & CFM; DampingConstant & ERP; DampingConstant & CFM
    double stepSize = simulation()->GetTimeIncrement();
    while (true)
    {
        if (findAttribute("ERP", &buf) && findAttribute("CFM", &buf2))
        {
            m_ERP = GSUtil::Double(buf);
            m_CFM = GSUtil::Double(buf2);
            m_SpringConstant = m_ERP / (m_CFM * stepSize);
            m_DampingConstant = (1.0 - m_ERP) / m_CFM;
            break;
        }
        if (findAttribute("ERP", &buf) && findAttribute("SpringConstant", &buf2))
        {
            m_ERP = GSUtil::Double(buf);
            m_SpringConstant = GSUtil::Double(buf2);
            m_DampingConstant = stepSize * (m_SpringConstant / m_ERP - m_SpringConstant);
            m_CFM = 1.0/(stepSize * m_SpringConstant + m_DampingConstant);
            break;
        }
        if (findAttribute("ERP", &buf) && findAttribute("DampingConstant", &buf2))
        {
            m_ERP = GSUtil::Double(buf);
            m_DampingConstant = GSUtil::Double(buf2);
            m_SpringConstant = m_DampingConstant / (stepSize / m_ERP - stepSize);
            m_CFM = 1.0/(stepSize * m_SpringConstant + m_DampingConstant);
            break;
        }
        if (findAttribute("CFM", &buf) && findAttribute("DampingConstant", &buf2))
        {
            m_CFM = GSUtil::Double(buf);
            m_DampingConstant = GSUtil::Double(buf2);
            m_SpringConstant = (1.0 / m_CFM - m_DampingConstant) / stepSize;
            m_ERP = stepSize * m_SpringConstant/(stepSize * m_SpringConstant + m_DampingConstant);
            break;
        }
        if (findAttribute("CFM", &buf) && findAttribute("SpringConstant", &buf2))
        {
            m_CFM = GSUtil::Double(buf);
            m_SpringConstant = GSUtil::Double(buf2);
            m_DampingConstant = 1.0 / m_CFM - stepSize * m_SpringConstant;
            m_ERP = stepSize * m_SpringConstant/(stepSize * m_SpringConstant + m_DampingConstant);
            break;
        }
        if (findAttribute("DampingConstant", &buf) && findAttribute("SpringConstant", &buf2))
        {
            m_DampingConstant = GSUtil::Double(buf);
            m_SpringConstant = GSUtil::Double(buf2);
            m_CFM = 1.0/(stepSize * m_SpringConstant + m_DampingConstant);
            m_ERP = stepSize * m_SpringConstant/(stepSize * m_SpringConstant + m_DampingConstant);
            break;
        }
        setLastError("GEOM ID=\""s + name() +"\" 2 of DampingConstant, SpringConstant, CFM, or ERP must be provided"s);
        return lastErrorPtr();
    }

    if (findAttribute("Bounce"s, &buf) == nullptr) return lastErrorPtr();
    this->SetContactBounce(GSUtil::Double(buf));
    if (findAttribute("Mu"s, &buf) == nullptr) return lastErrorPtr();
    this->SetContactMu(GSUtil::Double(buf));
    if (findAttribute("Abort"s, &buf) == nullptr) return lastErrorPtr();
    this->SetAbort(GSUtil::Bool(buf));
    if (findAttribute("Adhesion"s, &buf) == nullptr) return lastErrorPtr();
    this->SetAdhesion(GSUtil::Bool(buf));

    if (findAttribute("Rho"s, &buf))
    {
        this->SetRho(GSUtil::Double(buf));
    }

    m_ExcludeList.clear();
    std::vector<NamedObject *> upstreamObjects;
    if (findAttribute("ExcludeIDList"s, &buf))
    {
        std::vector<std::string> geomNames;
        pystring::split(buf, geomNames);
        for (size_t i = 0; i < geomNames.size(); i++)
        {
            Geom *geom = simulation()->GetGeom(geomNames[i]);
            if (!geom)
            {
                setLastError("GEOM ID=\""s + name() + "ExcludeList geom "s + geomNames[i] + " missing"s);
                return lastErrorPtr();
            }
            m_ExcludeList.push_back(geom);
            upstreamObjects.push_back(geom);
        }
    }

    upstreamObjects.push_back(m_geomMarker);
    setUpstreamObjects(std::move(upstreamObjects));
    return nullptr;
}

// this function copies the data in the object to an xml_node node that it creates internally.
// doc is used to allocate the memory so deletion should be automatic
void Geom::saveToAttributes()
{
    this->setTag("GEOM"s);
    this->clearAttributeMap();
    this->appendToAttributes();
}

// this function appends data to a pre-existing xml_node - often created by XMLSave
void Geom::appendToAttributes()
{
    NamedObject::appendToAttributes();
    std::string buf;
    setAttribute("MarkerID"s, m_geomMarker->name());
    setAttribute("SpringConstant"s, *GSUtil::ToString(m_SpringConstant, &buf));
    setAttribute("DampingConstant"s, *GSUtil::ToString(m_DampingConstant, &buf));
    setAttribute("Bounce"s, *GSUtil::ToString(m_Bounce, &buf));
    setAttribute("Mu"s, *GSUtil::ToString(m_Mu, &buf));
    setAttribute("Rho"s, *GSUtil::ToString(m_Rho, &buf));
    setAttribute("Abort"s, *GSUtil::ToString(m_Abort, &buf));
    setAttribute("Adhesion"s, *GSUtil::ToString(m_Adhesion, &buf));
    std::vector<std::string> geomNames;
    for (size_t i = 0; i < m_ExcludeList.size(); i++) geomNames.push_back(m_ExcludeList[i]->name());
    setAttribute("ExcludeIDList"s, pystring::join(" "s, geomNames));
}

void Geom::SetGeomLocation(GeomLocation geomLocation)
{
    m_GeomLocation = geomLocation;
}

Geom::GeomLocation Geom::GetGeomLocation() const
{
    return m_GeomLocation;
}

double Geom::GetContactSoftCFM() const
{
    return m_CFM;
}

double Geom::GetContactSoftERP() const
{
    return m_ERP;
}

void Geom::SetContactMu(double mu)
{
    m_Mu = mu;
}

double Geom::GetContactMu() const
{
    return m_Mu;
}

void Geom::SetContactBounce(double bounce)
{
    m_Bounce = bounce;
}

double Geom::GetContactBounce() const
{
    return m_Bounce;
}

void Geom::SetRho(double rho)
{
    m_Rho = rho;
}

double Geom::GetRho() const
{
    return m_Rho;
}

void Geom::SetAbort(bool abort)
{
    m_Abort = abort;
}

bool Geom::GetAbort() const
{
    return m_Abort;
}

void Geom::SetAdhesion(bool adhesion)
{
    m_Adhesion = adhesion;
}

bool Geom::GetAdhesion() const
{
    return m_Adhesion;
}

void Geom::AddContact(Contact *contact)
{
    m_ContactList.push_back(contact);
}

std::vector<Contact *> *Geom::GetContactList()
{
    return &m_ContactList;
}

void Geom::ClearContacts()
{
    m_ContactList.clear();
}

std::vector<Geom *> *Geom::GetExcludeList()
{
    return &m_ExcludeList;
}


