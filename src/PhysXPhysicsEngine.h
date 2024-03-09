#ifndef PhysXPhysicsEngine_H
#define PhysXPhysicsEngine_H

#include "PhysicsEngine.h"

#include "PxPhysicsAPI.h"

class PhysXPhysicsEngine : public PhysicsEngine
{
public:
    PhysXPhysicsEngine();
    virtual ~PhysXPhysicsEngine();

    virtual int Initialise(Simulation *theSimulation);
    virtual int Step();

 private:

    void CreateBodies();
    void CreateJoints();
    void CreateGeoms();
    void MoveBodies();

    // Simulation variables
    physx::PxFoundation *m_foundation = nullptr;
    physx::PxPhysics *m_physics = nullptr;
    physx::PxDefaultCpuDispatcher *m_dispatcher = nullptr;
    physx::PxScene *m_scene = nullptr;
    physx::PxPvd *m_pvd = nullptr;

    float m_defaultLength = 1.0f;
    float m_defaultSpeed = 10.0f;
    bool m_recordMemoryAllocations = true;

    std::map<std::string, physx::PxRigidDynamic*> m_bodyMap;
    std::map<std::string, physx::PxJoint*> m_jointMap;

};

#endif // PhysXPhysicsEngine_H
