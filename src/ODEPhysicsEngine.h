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

    dWorldID worldID() const;

    dSpaceID spaceID() const;

    dJointGroupID contactGroup() const;

    std::vector<std::unique_ptr<Contact>> *contactList();

private:
    static void NearCallback(void *data, dGeomID o1, dGeomID o2);

    // Simulation variables
    dWorldID m_worldID;
    dSpaceID m_spaceID;
    dJointGroupID m_contactGroup;

    // this is a list of contacts that are active at the current time step
    std::vector<std::unique_ptr<Contact>> m_contactList;

    // ODE uses callbacks for errors so this needs wrapping
    ErrorHandler m_errorHandler;

    // ode needs external handling of joint feedback memory
    std::map<std::string, std::unique_ptr<dJointFeedback>> m_jointFeedback;
};

#endif // ODEPHYSICSENGINE_H
