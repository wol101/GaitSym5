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

    virtual std::string *Initialise(Simulation *theSimulation);
    virtual std::string *Step();

    dWorldID worldID() const;

    dSpaceID spaceID() const;

    dJointGroupID contactGroup() const;

    std::vector<std::unique_ptr<dJointFeedback>> *contactFeedbackList();

    std::map<std::string, std::unique_ptr<dJointFeedback>> *jointFeedbackMap();

private:
    static void NearCallback(void *data, dGeomID o1, dGeomID o2);

    std::string *CreateBodies();
    std::string *CreateJoints();
    std::string *CreateGeoms();
    std::string *MoveBodies();

    // Simulation variables
    dWorldID m_worldID;
    dSpaceID m_spaceID;
    dJointGroupID m_contactGroup;

    // error message handling
    static void ODEMessageTrap(int num, const char *msg, va_list ap);
    static bool GetErrorMessage(int *messageNumber, std::string *messageText);
    static std::string m_messageText;
    static int m_messageNumber;
    static bool m_messageFlag;

    // ODE needs external handling of joint feedback memory
    std::map<std::string, std::unique_ptr<dJointFeedback>> m_jointFeedback;
    std::vector<std::unique_ptr<dJointFeedback>> m_contactFeedbackList;

};

#endif // ODEPHYSICSENGINE_H
