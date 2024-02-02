#ifndef ODEPHYSICSENGINE_H
#define ODEPHYSICSENGINE_H

#include "PhysicsEngine.h"
#include "ErrorHandler.h"
#include "Contact.h"

#include "ode/ode.h"

#include <vector>

class ODEPhysicsEngine : public PhysicsEngine
{
public:
    ODEPhysicsEngine();
    virtual ~ODEPhysicsEngine();

    virtual int Initialise(Simulation *simulation);
    virtual int Step();

private:
    static void NearCallback(void *data, dGeomID o1, dGeomID o2);

    // Simulation variables
    dWorldID m_WorldID;
    dSpaceID m_SpaceID;
    dJointGroupID m_ContactGroup;
    int m_MaxContacts = 64;

    // this is a list of contacts that are active at the current time step
    std::vector<std::unique_ptr<Contact>> m_ContactList;

    // ODE uses callbacks for errors so this needs wrapping
    ErrorHandler m_errorHandler;
};

#endif // ODEPHYSICSENGINE_H
