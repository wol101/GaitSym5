/*
 *  Joint.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 19/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

// wrapper class for a joint

#ifndef Joint_h
#define Joint_h

#include "NamedObject.h"
#include "PGDMath.h"

class Body;
class SimulationWindow;
class Marker;

class Joint: public NamedObject
{
public:

    Joint();

    void GetFeedback(pgd::Vector3 *position, pgd::Vector3 *force, pgd::Vector3 *torque);

    // some joints (particularly those with motors) need to do something before the simulation step
    virtual void Update() {}

    // this might be needed at some point in the future
    virtual void LateInitialisation() {}

    Marker *body1Marker() const;
    void setBody1Marker(Marker *body1Marker);

    Marker *body2Marker() const;
    void setBody2Marker(Marker *body2Marker);

    Body *body1() const;
    Body *body2() const;

    virtual std::string *createFromAttributes();
    virtual void saveToAttributes();
    virtual void appendToAttributes();

    double CFM() const;
    void setCFM(double CFM);

    double ERP() const;
    void setERP(double ERP);

    pgd::Vector3 GetWorldDistance() const; //distance vector from body1 to body2 in world coordinates
    pgd::Quaternion GetWorldRotation() const; //rotation quaternion from body1 to body2 in world coordinates
    pgd::Vector3 GetBody1Distance() const; //distance vector from body1 to body2 in body1 coordinates
    pgd::Quaternion GetBody1Rotation() const; //rotation quaternion from body1 to body2 in body1 coordinates
    pgd::Vector3 GetBody2Distance() const; //distance vector from body1 to body2 in body2 coordinates
    pgd::Quaternion GetBody2Rotation() const; //rotation quaternion from body1 to body2 in body2 coordinates

private:

    Marker *m_body1Marker = nullptr;
    Marker *m_body2Marker = nullptr;
    double m_CFM = -1;
    double m_ERP = -1;
};

#endif
