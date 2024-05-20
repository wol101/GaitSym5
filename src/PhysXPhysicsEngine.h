/*
 *  PhysXPhysicsEngine.h
 *  GaitSym5
 *
 *  Created by Bill Sellers on 20/03/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#ifndef PhysXPhysicsEngine_H
#define PhysXPhysicsEngine_H

#include "PhysicsEngine.h"

#include "PxPhysicsAPI.h"

namespace GaitSym
{

class ContactReportCallback: public physx::PxSimulationEventCallback
{
public:
    struct ContactData
    {
        physx::PxActor *actors[2];
        std::vector<physx::PxShape *> shapes;
        std::vector<physx::PxVec3> positions;
        std::vector<physx::PxVec3> impulses;
    };

    void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)	{ PX_UNUSED(constraints); PX_UNUSED(count); }
    void onWake(physx::PxActor** actors, physx::PxU32 count)							{ PX_UNUSED(actors); PX_UNUSED(count); }
    void onSleep(physx::PxActor** actors, physx::PxU32 count)							{ PX_UNUSED(actors); PX_UNUSED(count); }
    void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)					{ PX_UNUSED(pairs); PX_UNUSED(count); }
    void onAdvance(const physx::PxRigidBody*const*, const physx::PxTransform*, const physx::PxU32) {}
    void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
    {
        m_contactData.push_back(ContactData());
        m_contactData.back().actors[0] = pairHeader.actors[0];
        m_contactData.back().actors[1] = pairHeader.actors[1];
        std::vector<physx::PxContactPairPoint> contactPoints;
        for (physx::PxU32 i = 0; i < nbPairs; i++)
        {
            physx::PxU32 contactCount = pairs[i].contactCount;
            if(contactCount)
            {
                contactPoints.resize(contactCount);
                pairs[i].extractContacts(&contactPoints[0], contactCount);

                for(physx::PxU32 j=0;j<contactCount;j++)
                {
                    m_contactData.back().positions.push_back(contactPoints[j].position);
                    m_contactData.back().impulses.push_back(contactPoints[j].impulse);
                    m_contactData.back().shapes.push_back(pairs[i].shapes[0]);
                    m_contactData.back().shapes.push_back(pairs[i].shapes[1]);
                }
            }
        }
    }

    std::vector<ContactData> *contactData() { return &m_contactData; }

private:
    std::vector<ContactData> m_contactData;
};

class PhysXPhysicsEngine : public PhysicsEngine
{
public:
    PhysXPhysicsEngine();
    virtual ~PhysXPhysicsEngine();

    virtual std::string *Initialise(Simulation *theSimulation);
    virtual std::string *Step();

 private:

    std::string *CreateBodies();
    std::string *CreateJoints();
    std::string *CreateGeoms();
    std::string *MoveBodies();

    // Simulation variables
    physx::PxFoundation *m_foundation = nullptr;
    physx::PxPhysics *m_physics = nullptr;
    physx::PxDefaultCpuDispatcher *m_dispatcher = nullptr;
    physx::PxScene *m_scene = nullptr;
    physx::PxPvd *m_pvd = nullptr;
    physx::PxRigidStatic *m_world = nullptr;

    float m_defaultLength = 1.0f;
    float m_defaultSpeed = 10.0f;
    bool m_recordMemoryAllocations = true;

    std::map<std::string, physx::PxRigidDynamic*> m_bodyMap;
    std::map<std::string, physx::PxJoint*> m_jointMap;
};

}

#endif // PhysXPhysicsEngine_H
