/*
 *  PhysXPhysicsEngine.cpp
 *  GaitSym5
 *
 *  Created by Bill Sellers on 20/03/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#include "PhysXPhysicsEngine.h"

#include "PGDMath.h"
#include "Simulation.h"
#include "Body.h"
#include "Muscle.h"
#include "FluidSac.h"
#include "Geom.h"
#include "HingeJoint.h"
#include "SphereGeom.h"
#include "PlaneGeom.h"
#include "Marker.h"
#include "Contact.h"

using namespace std::string_literals;
namespace GaitSym {

static physx::PxDefaultAllocator g_allocator;
static physx::PxDefaultErrorCallback g_errorCallback;
ContactReportCallback g_contactReportCallback;
static physx::PxFilterFlags contactReportFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
                                                      physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
                                                      physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);


#define PVD_HOST "127.0.0.1"

PhysXPhysicsEngine::PhysXPhysicsEngine()
{
}

PhysXPhysicsEngine::~PhysXPhysicsEngine()
{
    PX_RELEASE(m_scene);
    PX_RELEASE(m_dispatcher);
    PxCloseExtensions();
    PX_RELEASE(m_physics);
    if(m_pvd)
    {
        physx::PxPvdTransport* transport = m_pvd->getTransport();
        m_pvd->release();
        m_pvd = nullptr;
        PX_RELEASE(transport);
    }
    PX_RELEASE(m_foundation);
}

std::string *PhysXPhysicsEngine::Initialise(Simulation *theSimulation)
{
    std::string *err = PhysicsEngine::Initialise(theSimulation);
    if (err) { return err; }

    m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_allocator, g_errorCallback);
    if (!m_foundation)
    {
        setLastError("Error: PhysXPhysicsEngine error in PxCreateFoundation"s);
        return lastErrorPtr();
    }

    m_pvd = PxCreatePvd(*m_foundation);
    physx::PxPvdTransport *transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    m_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, physx::PxTolerancesScale(m_defaultLength, m_defaultSpeed), m_recordMemoryAllocations, m_pvd);
    if (!m_physics)
    {
        setLastError("Error: PhysXPhysicsEngine error in PxCreatePhysics"s);
        return lastErrorPtr();
    }

    PxInitExtensions(*m_physics, m_pvd);

    physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
    pgd::Vector3 gravity = simulation()->GetGlobal()->Gravity();
    sceneDesc.gravity = physx::PxVec3(gravity.x, gravity.y, gravity.z);
    m_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    physx::PxU32 numCores = physx::PxThread::getNbPhysicalCores();
    m_dispatcher = physx::PxDefaultCpuDispatcherCreate(numCores == 0 ? 0 : numCores - 1);
    sceneDesc.cpuDispatcher	= m_dispatcher;
    sceneDesc.filterShader	= contactReportFilterShader;
    sceneDesc.simulationEventCallback = &g_contactReportCallback;
    m_scene = m_physics->createScene(sceneDesc);
    m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
    m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LIMITS, 1.0f);

    physx::PxPvdSceneClient* pvdClient = m_scene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    // create the fixed world body
    physx::PxTransform transform(physx::PxVec3(0, 0, 0));
    m_world = m_physics->createRigidStatic(transform);
    m_world->userData = nullptr;
    m_scene->addActor(*m_world);

    // create the PhysX versions of the main elements
    CreateBodies();
    CreateJoints();
    CreateGeoms();

    // And PhysX requires that bodies be moved to their starting positions after joints have been created
    MoveBodies();

    return nullptr;
}

std::string *PhysXPhysicsEngine::CreateBodies()
{
    const pgd::Quaternion zeroRotation( 1, 0, 0, 0);
    for (auto &&iter : *simulation()->GetBodyList())
    {
        Body *body = iter.second.get();
        double mass, ixx, iyy, izz, ixy, izx, iyz;
        body->GetMass(&mass, &ixx, &iyy, &izz, &ixy, &izx, &iyz);
        pgd::Vector3 position = body->GetConstructionPosition();
        pgd::Vector3 linearVelocity = body->GetLinearVelocity();
        pgd::Vector3 angularVelocity = body->GetAngularVelocity();
        physx::PxTransform transform(physx::PxVec3(position.x, position.y, position.z), physx::PxQuat(zeroRotation.x, zeroRotation.y, zeroRotation.z, zeroRotation.n));
        physx::PxRigidDynamic* rigidDynamic = m_physics->createRigidDynamic(transform);
        physx::PxMat33 inertialTensor(physx::PxVec3(ixx, ixy, izx), physx::PxVec3(ixy, iyy, iyz), physx::PxVec3(izx, iyz, izz)); // construct from 3 column vectors
        physx::PxQuat massFrame;
        physx::PxVec3 diagonal = physx::PxMassProperties::getMassSpaceInertia(inertialTensor, massFrame);
        rigidDynamic->setMass(mass);
        rigidDynamic->setMassSpaceInertiaTensor(diagonal);
        rigidDynamic->setCMassLocalPose(physx::PxTransform(physx::PxVec3(0, 0, 0), massFrame));
        rigidDynamic->setLinearVelocity(physx::PxVec3(linearVelocity.x, linearVelocity.y, linearVelocity.z));
        rigidDynamic->setAngularVelocity(physx::PxVec3(angularVelocity.x, angularVelocity.y, angularVelocity.z));
        rigidDynamic->userData = body;
        m_scene->addActor(*rigidDynamic);
        m_bodyMap[iter.first] = rigidDynamic;
    }
    return nullptr;
}

std::string *PhysXPhysicsEngine::CreateJoints()
{
    for (auto &&iter : *simulation()->GetJointList())
    {
        while (true)
        {
            if (HingeJoint *hingeJoint = dynamic_cast<HingeJoint *>(iter.second.get()))
            {
                Marker *marker1 = hingeJoint->body1Marker();
                Marker *marker2 = hingeJoint->body2Marker();
                pgd::Vector3 p1 = marker1->GetPosition();
                pgd::Vector3 p2 = marker2->GetPosition();
                pgd::Quaternion q1 = marker1->GetQuaternion();
                pgd::Quaternion q2 = marker2->GetQuaternion();
                physx::PxTransform localFrame0(physx::PxVec3(p1.x, p1.y, p1.z), physx::PxQuat(q1.x, q1.y, q1.z, q1.n));
                physx::PxTransform localFrame1(physx::PxVec3(p2.x, p2.y, p2.z), physx::PxQuat(q2.x, q2.y, q2.z, q2.n));
                physx::PxRigidActor *actor0 = m_bodyMap[hingeJoint->body1()->name()];
                physx::PxRigidActor *actor1 = m_bodyMap[hingeJoint->body2()->name()];
                physx::PxRevoluteJoint *revolute = PxRevoluteJointCreate(*m_physics, actor0, localFrame0, actor1, localFrame1);
                revolute->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, true);

                pgd::Vector2 stops = hingeJoint->stops();
                double springConstant = hingeJoint->stopSpring();
                double dampingConstant = hingeJoint->stopDamp();
                pgd::Vector2 reversedStops(-stops[1], -stops[0]);
                revolute->setLimit(physx::PxJointAngularLimitPair(reversedStops[0], reversedStops[1], physx::PxSpring(springConstant, dampingConstant))); // note the stops are reversed from the ODE values
                revolute->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eLIMIT_ENABLED, true);
                revolute->userData = hingeJoint;
                m_jointMap[iter.first] = revolute;
                break;
            }
            break;
        }
    }
    return nullptr;
}


std::string *PhysXPhysicsEngine::CreateGeoms()
{
    for (auto &&iter : *simulation()->GetGeomList())
    {
        while (true)
        {
            if (SphereGeom *sphereGeom = dynamic_cast<SphereGeom *>(iter.second.get()))
            {
                double radius = sphereGeom->radius();
                pgd::Vector3 position = sphereGeom->GetPosition();
                pgd::Quaternion quaternion = sphereGeom->GetQuaternion();
                physx::PxReal staticFriction = sphereGeom->GetContactMu();
                physx::PxReal dynamicFriction = staticFriction; // FIX ME - need to implement dynamic friction
                physx::PxMaterial *material;
                if (sphereGeom->GetContactBounce() > 0)
                {
                    physx::PxReal restitution = sphereGeom->GetContactBounce();
                    material = m_physics->createMaterial(staticFriction, dynamicFriction, restitution);
                }
                else
                {
                    physx::PxReal restitution = -1 * sphereGeom->GetContactSpringConstant();
                    physx::PxReal damping = sphereGeom->GetContactDampingConstant();
                    material = m_physics->createMaterial(staticFriction, dynamicFriction, restitution);
                    material->setFlag(physx::PxMaterialFlag::eCOMPLIANT_CONTACT, true);
                    material->setDamping(damping);
                }
                bool isExclusive = true;
                physx::PxShapeFlags shapeFlags = physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE;
                physx::PxShape *shape = m_physics->createShape(physx::PxSphereGeometry(radius), *material, isExclusive, shapeFlags);
                physx::PxTransform transform(physx::PxVec3(position.x, position.y, position.z), physx::PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.n));
                shape->setLocalPose(transform);
                shape->userData = sphereGeom;
                m_bodyMap[sphereGeom->GetBody()->name()]->attachShape(*shape);
                material->release();
                shape->release();
                break;
            }
            if (PlaneGeom *planeGeom = dynamic_cast<PlaneGeom *>(iter.second.get()))
            {
                double a, b, c, d;
                planeGeom->GetPlane(&a, &b, &c, &d);
                physx::PxReal staticFriction = planeGeom->GetContactMu();
                physx::PxReal dynamicFriction = staticFriction; // FIX ME - need to implement dynamic friction
                physx::PxMaterial *material;
                if (planeGeom->GetContactBounce() > 0)
                {
                    physx::PxReal restitution = planeGeom->GetContactBounce();
                    material = m_physics->createMaterial(staticFriction, dynamicFriction, restitution);
                }
                else
                {
                    physx::PxReal restitution = -1 * planeGeom->GetContactSpringConstant();
                    physx::PxReal damping = planeGeom->GetContactDampingConstant();
                    material = m_physics->createMaterial(staticFriction, dynamicFriction, restitution);
                    material->setFlag(physx::PxMaterialFlag::eCOMPLIANT_CONTACT, true);
                    material->setDamping(damping);
                }
                bool isExclusive = true;
                physx::PxShapeFlags shapeFlags = physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE;
                physx::PxShape *shape = m_physics->createShape(physx::PxPlaneGeometry(), *material, isExclusive, shapeFlags);
                physx::PxTransform transform = PxTransformFromPlaneEquation(physx::PxPlane(a, b, c, d));
                shape->setLocalPose(transform);
                shape->userData = planeGeom;
                m_world->attachShape(*shape); // planes have to be attached to non-dynamic bodies
                material->release();
                shape->release();
                break;
            }
            break;
        }
    }
    return nullptr;
}

std::string *PhysXPhysicsEngine::MoveBodies()
{
    for (auto &&iter : *simulation()->GetBodyList())
    {
        physx::PxRigidDynamic* rigidDynamic = m_bodyMap[iter.first];
        pgd::Vector3 position = iter.second->GetPosition();
        pgd::Quaternion quaternion = iter.second->GetQuaternion();
        physx::PxTransform transform(physx::PxVec3(position.x, position.y, position.z), physx::PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.n));
        rigidDynamic->setGlobalPose(transform);
    }
    return nullptr;
}

std::string *PhysXPhysicsEngine::Step()
{
    // clear the contacts
    g_contactReportCallback.contactData()->clear();

    // apply the point forces from the muscles
    for (auto &&iter :  *simulation()->GetMuscleList())
    {
        std::vector<std::unique_ptr<PointForce>> *pointForceList = iter.second->GetPointForceList();
        double tension = iter.second->GetTension();
        for (unsigned int i = 0; i < pointForceList->size(); i++)
        {
            const PointForce *pf = pointForceList->at(i).get();
            if (pf->body)
            {
                physx::PxRigidDynamic* rigidDynamic = m_bodyMap[pf->body->name()];
                physx::PxRigidBodyExt::addForceAtPos(*rigidDynamic, physx::PxVec3(pf->vector[0] * tension, pf->vector[1] * tension, pf->vector[2] * tension),
                                                     physx::PxVec3(pf->point[0], pf->point[1], pf->point[2]), physx::PxForceMode::eFORCE, true);
            }
        }
    }

    // apply the point forces from the  fluid sacs
    for (auto &&iter : *simulation()->GetFluidSacList())
    {
        for (size_t i = 0; i < iter.second->pointForceList().size(); i++)
        {
            const PointForce *pf = &iter.second->pointForceList().at(i);
            if (pf->body)
            {
                physx::PxRigidDynamic* rigidDynamic = m_bodyMap[pf->body->name()];
                physx::PxRigidBodyExt::addForceAtPos(*rigidDynamic, physx::PxVec3(pf->vector[0], pf->vector[1], pf->vector[2]),
                                                     physx::PxVec3(pf->point[0], pf->point[1], pf->point[2]), physx::PxForceMode::eFORCE, true);
            }
        }
    }

    // apply the forces from the drag
    for (auto &&iter : *simulation()->GetBodyList())
    {
        if (iter.second->dragControl() == Body::NoDrag) continue;
        pgd::Vector3 dragForce = iter.second->dragForce();
        pgd::Vector3 dragTorque = iter.second->dragTorque();
        iter.second->ComputeDrag();
        Marker marker(iter.second.get());
        pgd::Vector3 worldDragForce = marker.GetWorldVector(dragForce);
        pgd::Vector3 worldDragTorque = marker.GetWorldVector(dragTorque);
        physx::PxRigidDynamic* rigidDynamic = m_bodyMap[iter.first];
        rigidDynamic->addForce(physx::PxVec3(worldDragForce[0], worldDragForce[1], worldDragForce[2]), physx::PxForceMode::eFORCE, true);
        rigidDynamic->addTorque(physx::PxVec3(worldDragTorque[0], worldDragTorque[1], worldDragTorque[2]), physx::PxForceMode::eFORCE, true);
    }

    // run the simulation
    m_scene->simulate(simulation()->GetGlobal()->StepSize());
    m_scene->fetchResults(true);

#ifdef DEBUG_ACTORS
    physx::PxScene* scene;
    PxGetPhysics().getScenes(&scene,1);
    physx::PxU32 nbActors = scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);
    if (nbActors)
    {
        std::vector<physx::PxRigidActor *> actors(nbActors);
        scene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<physx::PxActor**>(&actors[0]), nbActors);
        for (size_t i = 0; i < actors.size(); i++)
        {
            auto actor = actors[i];
            if (actor->getName()) std::cerr << i << " " << actor->getName() << "\n";
            else std::cerr << i << "\n";
            physx::PxBounds3 bounds = actor->getWorldBounds(1.01f);
            physx::PxVec3 center = bounds.getCenter();
            std::cerr << center[0] << " " << center[1] << " " << center[2] << "\n";
        }
    }
#endif

    // update the objects with the new data
    for (auto &&iter : *simulation()->GetBodyList())
    {
        physx::PxRigidDynamic* rigidDynamic = m_bodyMap[iter.first];
        physx::PxTransform transform = rigidDynamic->getGlobalPose();
        physx::PxVec3 linearVelocity = rigidDynamic->getLinearVelocity();
        physx::PxVec3 angularVelocity = rigidDynamic->getAngularVelocity();
        iter.second->SetPosition(transform.p[0], transform.p[1], transform.p[2]);
        iter.second->SetQuaternion(transform.q.w, transform.q.x, transform.q.y, transform.q.z);
        iter.second->SetLinearVelocity(linearVelocity[0], linearVelocity[1], linearVelocity[2]);
        iter.second->SetAngularVelocity(angularVelocity[0], angularVelocity[1], angularVelocity[2]);
    }

    for (auto &&iter : *simulation()->GetJointList())
    {
        while (true)
        {
            if (HingeJoint *hingeJoint = dynamic_cast<HingeJoint *>(iter.second.get()))
            {
                physx::PxRevoluteJoint *joint = dynamic_cast<physx::PxRevoluteJoint *>(m_jointMap[iter.first]);
                if (!joint)
                {
                    std::cerr << "Error in PhysXPhysicsEngine::Step(): joint type mismatch\n";
                    continue;
                }
                physx::PxConstraint *constraint = joint->getConstraint();
                if (!constraint->isValid())
                {
                    std::cerr << "Error in PhysXPhysicsEngine::Step(): constraint not valid\n";
                    continue;
                }
                physx::PxVec3 linear;
                physx::PxVec3 angular;
                constraint->getForce(linear, angular);
                double angle = joint->getAngle();
                double angleRate = joint->getVelocity();
                physx::PxTransform localTransform = joint->getLocalPose(physx::PxJointActorIndex::Enum::eACTOR0);
                physx::PxRigidActor *actor0, *actor1;
                joint->getActors(actor0, actor1);
                physx::PxTransform bodyTransform = actor0->getGlobalPose();
                physx::PxTransform worldTransform = localTransform.transform(bodyTransform);
                physx::PxVec3 axis = worldTransform.rotate(physx::PxVec3(1, 0, 0));

                hingeJoint->setForce(pgd::Vector3(linear[0], linear[1], linear[2]));
                hingeJoint->setTorque(pgd::Vector3(angular[0], angular[1], angular[2]));
                hingeJoint->setAnchor(pgd::Vector3(worldTransform.p[0], worldTransform.p[1], worldTransform.p[2]));
                hingeJoint->setAxis(pgd::Vector3(axis[0], axis[1], axis[2]));
                hingeJoint->setAngle(angle);
                hingeJoint->setAngleRate(angleRate);
                break;
            }
            break;
        }
    }

    simulation()->GetContactList()->clear();
    double timeStep =simulation()->GetGlobal()->StepSize();
    for (size_t i = 0; i < g_contactReportCallback.contactData()->size(); i++)
    {
        physx::PxActor *actors[2];
        actors[0] = g_contactReportCallback.contactData()->at(i).actors[0];
        actors[1] = g_contactReportCallback.contactData()->at(i).actors[1];
        for (size_t j = 0; j < g_contactReportCallback.contactData()->at(i).positions.size(); j++)
        {
            physx::PxVec3 position = g_contactReportCallback.contactData()->at(i).positions[j];
            physx::PxVec3 impulse = g_contactReportCallback.contactData()->at(i).impulses[j];
            physx::PxShape *shape1 = g_contactReportCallback.contactData()->at(i).shapes[j * 2];
            physx::PxShape *shape2 = g_contactReportCallback.contactData()->at(i).shapes[j * 2 + 1];
            std::unique_ptr<Contact> myContact = std::make_unique<Contact>();
            myContact->setSimulation(simulation());
            myContact->setPosition(pgd::Vector3(position[0], position[1], position[2]));
            myContact->setForce(pgd::Vector3(impulse[0] / timeStep, impulse[1] / timeStep, impulse[2] / timeStep));
            Geom *geom1 = reinterpret_cast<Geom *>(shape1->userData);
            Geom *geom2 = reinterpret_cast<Geom *>(shape2->userData);
            geom1->AddContact(myContact.get());
            geom2->AddContact(myContact.get());
            myContact->setBody1(geom1->GetBody());
            myContact->setBody2(geom2->GetBody());
            simulation()->GetContactList()->push_back(std::move(myContact));
        }
    }

    return nullptr;
}


static physx::PxFilterFlags contactReportFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
                                                      physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
                                                      physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
    PX_UNUSED(attributes0);
    PX_UNUSED(attributes1);
    PX_UNUSED(filterData0);
    PX_UNUSED(filterData1);
    PX_UNUSED(constantBlockSize);
    PX_UNUSED(constantBlock);

    // all initial and persisting reports for everything, with per-point data
    pairFlags = physx::PxPairFlag::eSOLVE_CONTACT | physx::PxPairFlag::eDETECT_DISCRETE_CONTACT |
                physx::PxPairFlag::eNOTIFY_TOUCH_FOUND |
                physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS |
                physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
    return physx::PxFilterFlag::eDEFAULT;
}

} // namespace GaitSym


