#ifndef ODEPHYSICSENGINE_H
#define ODEPHYSICSENGINE_H

#include "PhysicsEngine.h"
#include "Contact.h"

#include "ode/ode.h"

#include <vector>

class ODEPhysicsEngine : public PhysicsEngine
{
public:
    ODEPhysicsEngine();
    virtual ~ODEPhysicsEngine();

    virtual int Initialise(Simulation *theSimulation);
    virtual int Step();

    dWorldID worldID() const;

    dSpaceID spaceID() const;

    dJointGroupID contactGroup() const;

    std::vector<std::unique_ptr<Contact>> *contactList();

    std::vector<std::unique_ptr<dJointFeedback> > *contactFeedbackList();

private:
    static void NearCallback(void *data, dGeomID o1, dGeomID o2);

    void CreateBodies();
    void CreateJoints();
    void CreateGeoms();
    void MoveBodies();

    // Simulation variables
    dWorldID m_worldID;
    dSpaceID m_spaceID;
    dJointGroupID m_contactGroup;

    // this is a list of contacts that are active at the current time step
    std::vector<std::unique_ptr<Contact>> m_contactList;
    std::vector< std::unique_ptr<dJointFeedback>> m_contactFeedbackList;

    // error message handling
    static void ODEMessageTrap(int num, const char *msg, va_list ap);
    static bool GetErrorMessage(int *messageNumber, std::string *messageText);
    static std::string m_messageText;
    static int m_messageNumber;
    static bool m_messageFlag;

    //ODE needs external handling of joint feedback memory
    std::map<std::string, std::unique_ptr<dJointFeedback>> m_jointFeedback;
};

#endif // ODEPHYSICSENGINE_H
