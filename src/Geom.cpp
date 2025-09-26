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
namespace GaitSym {

Geom::Geom()
{
}

// these functions set the geom position relative to its body
// these now use the geom offset functions
void Geom::setBody(Body *body)
{
    m_body = body;
}

Body *Geom::body()
{
    return m_body;
}

void Geom::setPosition (double x, double y, double z)
{
    m_position.set(x, y, z);
}

pgd::Vector3 Geom::position() const
{
    return m_position;
}

pgd::Vector3 Geom::worldPosition() const
{
    if (m_body)
    {
        // get the position in world coordinates
        //        pgd::Vector3 p;
        //        dBodyGetRelPointPos(m_body->GetBodyID(), m_position.x, m_position.y, m_position.z, p);
        //        return pgd::Vector3(p[0], p[1], p[2]);
        pgd::Vector3 bodyWorldPosition = pgd::qVRotate(m_body->quaternion(), m_position) + pgd::Vector3(m_body->position());
        return bodyWorldPosition;
    }
    else
    {
        return m_position;
    }
}

void Geom::setQuaternion(double n, double x, double y, double z)
{
    m_quaternion.set(n, x, y, z);
}

void Geom::setGeomMarker(Marker *geomMarker)
{
    m_geomMarker = geomMarker;
    if (m_geomMarker->body())
    {
        this->setGeomLocation(Geom::OnBody);
        this->setBody(m_geomMarker->body());
    }
    else
    {
        this->setBody(nullptr);
        this->setGeomLocation(Geom::OnEnvironment);

    }
    if (dynamic_cast<PlaneGeom *>(this)) return; // do not try to place non-placeable geoms

    pgd::Vector3 p = geomMarker->position();
    this->setPosition(p.x, p.y, p.z);
    pgd::Quaternion q = geomMarker->quaternion();
    this->setQuaternion(q.n, q.x, q.y, q.z);
}

pgd::Quaternion Geom::quaternion() const
{
    return m_quaternion;
}

pgd::Quaternion Geom::worldQuaternion() const
{
    if (m_body)
    {
        return m_body->quaternion() * m_quaternion;
    }
    else
    {
        return m_quaternion;
    }
}

void Geom::setSpringDamp(double springConstant, double dampingConstant, double integrationStep)
{
    m_erp = integrationStep * springConstant/(integrationStep * springConstant + dampingConstant);
    m_cfm = 1/(integrationStep * springConstant + dampingConstant);
    m_springConstant = springConstant;
    m_dampingConstant = springConstant;
}

void Geom::setSpringERP(double springConstant, double ERP, double integrationStep)
{
    m_erp = ERP;
    m_cfm = ERP / (integrationStep * springConstant);
    m_springConstant = springConstant;
    m_dampingConstant = (1.0 - m_erp) / m_cfm;
}

void Geom::setSpringCFM(double springConstant, double CFM, double integrationStep)
{
    m_cfm = CFM;
    m_springConstant = springConstant;
    m_dampingConstant = 1.0 / m_cfm - integrationStep * m_springConstant;
    m_erp = integrationStep * m_springConstant/(integrationStep * m_springConstant + m_dampingConstant);
}

void Geom::setCFMERP(double CFM, double ERP, double integrationStep)
{
    m_erp = ERP;
    m_cfm = CFM;
    m_springConstant = m_erp / (m_cfm * integrationStep);
    m_dampingConstant = (1.0 - m_erp) / m_cfm;
}

void Geom::setCFMDamp(double CFM, double dampingConstant, double integrationStep)
{
    m_cfm = CFM;
    m_dampingConstant = dampingConstant;
    m_springConstant = (1.0 / m_cfm - m_dampingConstant) / integrationStep;
    m_erp = integrationStep * m_springConstant/(integrationStep * m_springConstant + m_dampingConstant);
}

void Geom::setERPDamp(double ERP, double dampingConstant, double integrationStep)
{
    m_erp = ERP;
    m_dampingConstant = dampingConstant;
    m_springConstant = m_dampingConstant / (integrationStep / m_erp - integrationStep);
    m_cfm = 1.0/(integrationStep * m_springConstant + m_dampingConstant);
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

    pgd::Vector3 p = m_geomMarker->worldPosition();
    pgd::Quaternion q = m_geomMarker->worldQuaternion();
    ss << simulation()->GetTime() << "\t" << p[0] << "\t" << p[1] << "\t" << p[2] << "\t" << q[0] << "\t" << q[1] << "\t" << q[2] << "\t" << q[3] << "\t" << m_contactList.size();
    std::string body1, body2;
    for (auto &&iter : m_contactList)
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

std::string Geom::type() const
{
    return m_type;
}

// this function initialises the data in the object based on the contents
// of an xml_node node. It uses information from the simulation as required
// to satisfy dependencies
// it returns nullptr on success and a pointer to lastError() on failure
std::string *Geom::createFromAttributes()
{
    if (NamedObject::createFromAttributes()) return lastErrorPtr();
    std::string buf, buf2;

    if (findAttribute("Type"s, &buf) == nullptr) return lastErrorPtr();
    m_type = buf;

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
            m_erp = GSUtil::toDouble(buf);
            m_cfm = GSUtil::toDouble(buf2);
            m_springConstant = m_erp / (m_cfm * stepSize);
            m_dampingConstant = (1.0 - m_erp) / m_cfm;
            break;
        }
        if (findAttribute("ERP", &buf) && findAttribute("SpringConstant", &buf2))
        {
            m_erp = GSUtil::toDouble(buf);
            m_springConstant = GSUtil::toDouble(buf2);
            m_dampingConstant = stepSize * (m_springConstant / m_erp - m_springConstant);
            m_cfm = 1.0/(stepSize * m_springConstant + m_dampingConstant);
            break;
        }
        if (findAttribute("ERP", &buf) && findAttribute("DampingConstant", &buf2))
        {
            m_erp = GSUtil::toDouble(buf);
            m_dampingConstant = GSUtil::toDouble(buf2);
            m_springConstant = m_dampingConstant / (stepSize / m_erp - stepSize);
            m_cfm = 1.0/(stepSize * m_springConstant + m_dampingConstant);
            break;
        }
        if (findAttribute("CFM", &buf) && findAttribute("DampingConstant", &buf2))
        {
            m_cfm = GSUtil::toDouble(buf);
            m_dampingConstant = GSUtil::toDouble(buf2);
            m_springConstant = (1.0 / m_cfm - m_dampingConstant) / stepSize;
            m_erp = stepSize * m_springConstant/(stepSize * m_springConstant + m_dampingConstant);
            break;
        }
        if (findAttribute("CFM", &buf) && findAttribute("SpringConstant", &buf2))
        {
            m_cfm = GSUtil::toDouble(buf);
            m_springConstant = GSUtil::toDouble(buf2);
            m_dampingConstant = 1.0 / m_cfm - stepSize * m_springConstant;
            m_erp = stepSize * m_springConstant/(stepSize * m_springConstant + m_dampingConstant);
            break;
        }
        if (findAttribute("DampingConstant", &buf) && findAttribute("SpringConstant", &buf2))
        {
            m_dampingConstant = GSUtil::toDouble(buf);
            m_springConstant = GSUtil::toDouble(buf2);
            m_cfm = 1.0/(stepSize * m_springConstant + m_dampingConstant);
            m_erp = stepSize * m_springConstant/(stepSize * m_springConstant + m_dampingConstant);
            break;
        }
        setLastError("GEOM ID=\""s + name() +"\" 2 of DampingConstant, SpringConstant, CFM, or ERP must be provided"s);
        return lastErrorPtr();
    }

    if (findAttribute("Bounce"s, &buf) == nullptr) return lastErrorPtr();
    this->setContactBounce(GSUtil::toDouble(buf));
    if (findAttribute("Mu"s, &buf) == nullptr) return lastErrorPtr();
    this->setContactMu(GSUtil::toDouble(buf));
    if (findAttribute("Abort"s, &buf) == nullptr) return lastErrorPtr();
    this->setAbort(GSUtil::toBool(buf));
    if (findAttribute("Adhesion"s, &buf) == nullptr) return lastErrorPtr();
    this->setAdhesion(GSUtil::toBool(buf));

    if (findAttribute("Rho"s, &buf))
    {
        this->setContactRho(GSUtil::toDouble(buf));
    }

    m_excludeList.clear();
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
            m_excludeList.push_back(geom);
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
    setAttribute("Type", type());
    setAttribute("MarkerID"s, m_geomMarker->name());
    setAttribute("SpringConstant"s, *GSUtil::toString(m_springConstant, &buf));
    setAttribute("DampingConstant"s, *GSUtil::toString(m_dampingConstant, &buf));
    setAttribute("Bounce"s, *GSUtil::toString(m_bounce, &buf));
    setAttribute("Mu"s, *GSUtil::toString(m_mu, &buf));
    setAttribute("Rho"s, *GSUtil::toString(m_rho, &buf));
    setAttribute("Abort"s, *GSUtil::toString(m_abort, &buf));
    setAttribute("Adhesion"s, *GSUtil::toString(m_adhesion, &buf));
    std::vector<std::string> geomNames;
    for (size_t i = 0; i < m_excludeList.size(); i++) geomNames.push_back(m_excludeList[i]->name());
    setAttribute("ExcludeIDList"s, pystring::join(" "s, geomNames));
}

void Geom::setGeomLocation(GeomLocation geomLocation)
{
    m_GeomLocation = geomLocation;
}

Geom::GeomLocation Geom::geomLocation() const
{
    return m_GeomLocation;
}

double Geom::contactSoftCFM() const
{
    return m_cfm;
}

double Geom::contactSoftERP() const
{
    return m_erp;
}

void Geom::setContactMu(double mu)
{
    m_mu = mu;
}

double Geom::contactMu() const
{
    return m_mu;
}

void Geom::setContactBounce(double bounce)
{
    m_bounce = bounce;
}

double Geom::contactBounce() const
{
    return m_bounce;
}

void Geom::setContactRho(double rho)
{
    m_rho = rho;
}

double Geom::contactRho() const
{
    return m_rho;
}

double Geom::contactSpringConstant() const
{
    return m_springConstant;
}

double Geom::contactDampingConstant() const
{
    return m_dampingConstant;
}

void Geom::setAbort(bool abort)
{
    m_abort = abort;
}

bool Geom::abort() const
{
    return m_abort;
}

void Geom::setAdhesion(bool adhesion)
{
    m_adhesion = adhesion;
}

bool Geom::adhesion() const
{
    return m_adhesion;
}

void Geom::addContact(Contact *contact)
{
    m_contactList.push_back(contact);
}

std::vector<Contact *> *Geom::contactList()
{
    return &m_contactList;
}

void Geom::clearContacts()
{
    m_contactList.clear();
}

std::vector<Geom *> *Geom::excludeList()
{
    return &m_excludeList;
}


} // namespace GaitSym


