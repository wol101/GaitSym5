/*
 *  Geom.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 28/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

// Wrapper class to hold ODE geom

#ifndef Geom_h
#define Geom_h

#include "NamedObject.h"
#include "SmartEnum.h"
#include "PGDMath.h"

#include <vector>

namespace GaitSym
{

class Contact;
class SimulationWindow;
class Marker;
class Body;

class Geom: public NamedObject
{
public:

    Geom();

    SMART_ENUM(GeomLocation, GeomLocationStrings, GeomLocationCount, OnEnvironment, OnBody);

    void setBody(Body *body);
    Body *body();

    // these functions set the geom position relative to its body
    void setPosition (double x, double y, double z);
    void setQuaternion(double n, double x, double y, double z);

    // return body local values
    pgd::Vector3 position() const;
    pgd::Quaternion quaternion() const;
    // return world values
    pgd::Vector3 worldPosition() const;
    pgd::Quaternion worldQuaternion() const;


    void setGeomLocation(GeomLocation l);
    GeomLocation geomLocation() const;
    double contactSoftCFM() const;
    double contactSoftERP() const;
    void setContactMu(double mu);
    double contactMu() const;
    void setContactBounce(double bounce);
    double contactBounce() const;
    void setContactRho(double rho);
    double contactRho() const;
    double contactSpringConstant() const;
    double contactDampingConstant() const;

    void setSpringDamp(double springConstant, double dampingConstant, double integrationStep);
    void setSpringERP(double springConstant, double ERP, double integrationStep);
    void setSpringCFM(double springConstant, double CFM, double integrationStep);
    void setCFMERP(double CFM, double ERP, double integrationStep);
    void setCFMDamp(double CFM, double dampingConstant, double integrationStep);
    void setERPDamp(double ERP, double dampingConstant, double integrationStep);

    void setAbort(bool abort);
    bool abort() const;

    void setAdhesion(bool adhesion);
    bool adhesion() const;

    void addContact(Contact *contact);
    std::vector<Contact *> *contactList();
    void clearContacts();

    std::vector<Geom *> *excludeList();

    virtual std::string dumpToString();
    virtual std::string *createFromAttributes();
    virtual void saveToAttributes();
    virtual void appendToAttributes();

    void setGeomMarker(Marker *geomMarker); // virtual because this is specialised for some geoms e.g. plane
    Marker *geomMarker() const;

    std::string type() const;

private:

    Body *m_body = nullptr;
    GeomLocation m_GeomLocation = {GeomLocation::OnEnvironment};
    pgd::Vector3 m_position;
    pgd::Quaternion m_quaternion;

    double m_cfm = -1;
    double m_erp = -1;
    double m_mu = -1;
    double m_bounce = -1;
    double m_rho = -1;

    bool m_abort = false;
    bool m_adhesion = false;

    std::vector<Contact *> m_contactList;

    Marker *m_geomMarker = nullptr;

    std::vector<Geom *> m_excludeList;

    // used for XMLSave
    double m_springConstant = 0;
    double m_dampingConstant = 0;

    std::string m_type;
};

}

#endif
