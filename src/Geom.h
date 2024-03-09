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

class Contact;
class SimulationWindow;
class Marker;
class Body;

class Geom: public NamedObject
{
public:

    Geom();

    SMART_ENUM(GeomLocation, GeomLocationStrings, GeomLocationCount, environment, body);
//    enum GeomLocation { environment, body };

    void SetBody(Body *body);
    Body *GetBody();

    // these functions set the geom position relative to its body
    void SetPosition (double x, double y, double z);
    void SetQuaternion(double n, double x, double y, double z);

    // return body local values
    pgd::Vector3 GetPosition() const;
    pgd::Quaternion GetQuaternion() const;
    // return world values
    pgd::Vector3 GetWorldPosition() const;
    pgd::Quaternion GetWorldQuaternion() const;


    void SetGeomLocation(GeomLocation l);
    GeomLocation GetGeomLocation() const;
    double GetContactSoftCFM() const;
    double GetContactSoftERP() const;
    void SetContactMu(double mu);
    double GetContactMu() const;
    void SetContactBounce(double bounce);
    double GetContactBounce() const;
    void SetRho(double rho);
    double GetRho() const;

    void SetSpringDamp(double springConstant, double dampingConstant, double integrationStep);
    void SetSpringERP(double springConstant, double ERP, double integrationStep);
    void SetSpringCFM(double springConstant, double CFM, double integrationStep);
    void SetCFMERP(double CFM, double ERP, double integrationStep);
    void SetCFMDamp(double CFM, double dampingConstant, double integrationStep);
    void SetERPDamp(double ERP, double dampingConstant, double integrationStep);

    void SetAbort(bool abort);
    bool GetAbort() const;

    void SetAdhesion(bool adhesion);
    bool GetAdhesion() const;

    void AddContact(Contact *contact);
    std::vector<Contact *> *GetContactList();
    void ClearContacts();

    std::vector<Geom *> *GetExcludeList();

    virtual std::string dumpToString();
    virtual std::string *createFromAttributes();
    virtual void saveToAttributes();
    virtual void appendToAttributes();

    void setGeomMarker(Marker *geomMarker); // virtual because this is specialised for some geoms e.g. plane
    Marker *geomMarker() const;

private:

    Body *m_body = nullptr;
    GeomLocation m_GeomLocation = {GeomLocation::environment};
    pgd::Vector3 m_position;
    pgd::Quaternion m_quaternion;

    double m_CFM = -1;
    double m_ERP = -1;
    double m_Mu = -1;
    double m_Bounce = -1;
    double m_Rho = -1;

    bool m_Abort = false;
    bool m_Adhesion = false;

    std::vector<Contact *> m_ContactList;

    Marker *m_geomMarker = nullptr;

    std::vector<Geom *> m_ExcludeList;

    // used for XMLSave
    double m_SpringConstant = 0;
    double m_DampingConstant = 0;
};


#endif
