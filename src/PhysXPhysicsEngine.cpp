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

using namespace std::string_literals;

static physx::PxDefaultAllocator		g_allocator;
static physx::PxDefaultErrorCallback	g_errorCallback;

#define PVD_HOST "127.0.0.1"

PhysXPhysicsEngine::PhysXPhysicsEngine()
{
}

PhysXPhysicsEngine::~PhysXPhysicsEngine()
{
    PX_RELEASE(m_scene);
    PX_RELEASE(m_dispatcher);
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

int PhysXPhysicsEngine::Initialise(Simulation *theSimulation)
{
    int err = PhysicsEngine::Initialise(theSimulation);
    if (err) { return err; }

    m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_allocator, g_errorCallback);
    if(!m_foundation)
    {
        return __LINE__;
    }

    m_pvd = PxCreatePvd(*m_foundation);
    physx::PxPvdTransport *transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    m_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, physx::PxTolerancesScale(m_defaultLength, m_defaultSpeed), m_recordMemoryAllocations, m_pvd);
    if (!m_physics)
    {
        return __LINE__;
    }

    physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    m_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher	= m_dispatcher;
    sceneDesc.filterShader	= physx::PxDefaultSimulationFilterShader;
    m_scene = m_physics->createScene(sceneDesc);
    m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
    m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LIMITS, 1.0f);

    physx::PxPvdSceneClient* pvdClient = m_scene->getScenePvdClient();
    if(pvdClient)
    {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    // create the PhysX versions of the main elements
    CreateBodies();
    CreateJoints();
    CreateGeoms();

    return 0;
}

void PhysXPhysicsEngine::CreateBodies()
{
    // first create the bodies
    pgd::Quaternion zeroRotation( 1, 0, 0, 0);

    for (auto &&iter : *simulation()->GetBodyList())
    {
        double mass, ixx, iyy, izz, ixy, izx, iyz;
        iter.second->GetMass(&mass, &ixx, &iyy, &izz, &ixy, &izx, &iyz);
        // pgd::Vector3 constructionPosition = iter.second->GetConstructionPosition();
        pgd::Vector3 position = iter.second->GetPosition();
        pgd::Quaternion quaternion = iter.second->GetQuaternion();
        pgd::Vector3 linearVelocity = iter.second->GetLinearVelocity();
        pgd::Vector3 angularVelocity = iter.second->GetAngularVelocity();        
        physx::PxTransform transform(physx::PxVec3(position.x, position.y, position.z), physx::PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.n));
        physx::PxRigidDynamic* rigidDynamic = m_physics->createRigidDynamic(transform);
        rigidDynamic->setMass(mass);
        rigidDynamic->setMassSpaceInertiaTensor(physx::PxVec3(ixx, iyy, izz)); // this is only approximate, if products of inertia are significant then an appropriate mass space transform needs to be set using setCMassLocalPose() and then the principle moments of inertia used
        rigidDynamic->setLinearVelocity(physx::PxVec3(linearVelocity.x, linearVelocity.y, linearVelocity.z));
        rigidDynamic->setLinearVelocity(physx::PxVec3(angularVelocity.x, angularVelocity.y, angularVelocity.z));
        m_scene->addActor(*rigidDynamic);
        m_bodyMap[iter.first] = rigidDynamic;
    }
}

void PhysXPhysicsEngine::CreateJoints()
{
    for (auto &&iter : *simulation()->GetJointList())
    {
        while (true)
        {
            HingeJoint *hingeJoint = dynamic_cast<HingeJoint *>(iter.second.get());
            if (hingeJoint)
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

                // pgd::Vector2 stops = hingeJoint->stops();
                // double springConstant = hingeJoint->stopSpring();
                // double dampingConstant = hingeJoint->stopDamp();
                // revolute->setLimit(physx::PxJointAngularLimitPair(stops[0], stops[1], physx::PxSpring(springConstant, dampingConstant)));
                // revolute->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eLIMIT_ENABLED, true);
                break;
            }
            break;
        }
    }
}


void PhysXPhysicsEngine::CreateGeoms()
{
    for (auto &&iter : *simulation()->GetGeomList())
    {
        while (true)
        {
            SphereGeom *sphereGeom = dynamic_cast<SphereGeom *>(iter.second.get());
            if (sphereGeom)
            {
                double radius = sphereGeom->radius();
                pgd::Vector3 position = sphereGeom->GetPosition();
                pgd::Quaternion quaternion = sphereGeom->GetQuaternion();
                physx::PxReal staticFriction = 1.0f;
                physx::PxReal dynamicFriction = 1.0f;
                physx::PxReal restitution = 1.0f;
                physx::PxMaterial *material = m_physics->createMaterial(staticFriction, dynamicFriction, restitution);
                bool isExclusive = true;
                physx::PxShapeFlags shapeFlags = physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE;
                physx::PxShape *shape = m_physics->createShape(physx::PxSphereGeometry(radius), *material, isExclusive, shapeFlags);
                physx::PxTransform transform(physx::PxVec3(position.x, position.y, position.z), physx::PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.n));
                shape->setLocalPose(transform);
                m_bodyMap[sphereGeom->GetBody()->name()]->attachShape(*shape);
                material->release();
                shape->release();
                break;
            }
            PlaneGeom *planeGeom = dynamic_cast<PlaneGeom *>(iter.second.get());
            if (planeGeom)
            {
                // double a, b, c, d;
                // planeGeom->GetPlane(&a, &b, &c, &d);
                // double length = std::sqrt(a * a + b * b + c * c);
                // if (length < std::numeric_limits<double>::epsilon()) // standard fixup
                // {
                //     a = 0; b = 0; c = 1; d = 0;
                // }
                // else
                // {
                //     a = a / length; b = b / length; c = c / length;
                // }
                // geomID = dCreatePlane(m_spaceID, a, b, c, d);
                // dGeomSetData(geomID, planeGeom);
                // iter.second->setData(geomID);
                // dGeomSetBody(geomID, nullptr);
                break;
            }
            break;
        }
    }
}

void PhysXPhysicsEngine::MoveBodies()
{
    for (auto &&iter : *simulation()->GetBodyList())
    {
        // dBodyID bodyID = reinterpret_cast<dBodyID>(iter.second->data());
        // pgd::Vector3 position = iter.second->GetPosition();
        // dBodySetPosition(bodyID, position.x, position.y, position.z);
        // pgd::Quaternion quaternion = iter.second->GetQuaternion();
        // dBodySetQuaternion(bodyID, quaternion.constData());
    }
}

int PhysXPhysicsEngine::Step()
{
    // check collisions first
    dJointGroupEmpty(m_contactGroup);
    m_contactFeedbackList.clear();
    dSpaceCollide(m_spaceID, this, &NearCallback);

    // apply the point forces from the muscles
    for (auto &&iter :  *simulation()->GetMuscleList())
    {
        std::vector<std::unique_ptr<PointForce>> *pointForceList = iter.second->GetPointForceList();
        double tension = iter.second->GetTension();
        for (unsigned int i = 0; i < pointForceList->size(); i++)
        {
            PointForce *pf = (*pointForceList)[i].get();
            if (pf->body)
                dBodyAddForceAtPos(reinterpret_cast<dBodyID>(pf->body->data()),
                                   pf->vector[0] * tension, pf->vector[1] * tension, pf->vector[2] * tension,
                                   pf->point[0], pf->point[1], pf->point[2]);
        }
    }

    // apply the point forces from the  fluid sacs
    for (auto &&iter : *simulation()->GetFluidSacList())
    {
        for (size_t i = 0; i < iter.second->pointForceList().size(); i++)
        {
            const PointForce *pf = &iter.second->pointForceList().at(i);
            dBodyAddForceAtPos(reinterpret_cast<dBodyID>(pf->body->data()),
                               pf->vector[0], pf->vector[1], pf->vector[2], pf->point[0], pf->point[1], pf->point[2]);
        }
    }

    // apply the forces from the drag
    for (auto &&bodyIter : *simulation()->GetBodyList())
    {
        pgd::Vector3 dragForce = bodyIter.second->dragForce();
        pgd::Vector3 dragTorque = bodyIter.second->dragTorque();
        bodyIter.second->ComputeDrag();
        dBodyAddRelForce(reinterpret_cast<dBodyID>(bodyIter.second->data()), dragForce.x, dragForce.y, dragForce.z);
        dBodyAddRelTorque(reinterpret_cast<dBodyID>(bodyIter.second->data()), dragTorque.x, dragTorque.y, dragTorque.z);
    }

    // run the simulation
    m_scene->simulate(simulation()->GetGlobal()->StepSize());
    m_scene->fetchResults(true);

    PxScene* scene;
    PxGetPhysics().getScenes(&scene,1);
    PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
    if (nbActors)
    {
        std::vector<PxRigidActor*> actors(nbActors);
        scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
        for (size_t i = 0; i < actors.size(); i++)
        {
            auto actor = actors[i];
            if (actor->getName()) std::cerr << i << " " << actor->getName() << "\n";
            else std::cerr << i << "\n";
            PxBounds3 bounds = actor->getWorldBounds(1.01f);
            PxVec3 center = bounds.getCenter();
            std::cerr << center[0] << " " << center[1] << " " << center[2] << "\n";
        }
    }

    // update the objects with the new data
    for (auto &&iter : *simulation()->GetBodyList())
    {
        dBodyID bodyID = reinterpret_cast<dBodyID>(iter.second->data());
        const double *position = dBodyGetPosition(bodyID);
        const double *quaternion = dBodyGetQuaternion(bodyID);
        const double *linearVelocity = dBodyGetLinearVel(bodyID);
        const double *angularVelocity = dBodyGetAngularVel(bodyID);
        iter.second->SetPosition(position[0], position[1], position[2]);
        iter.second->SetQuaternion(quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
        iter.second->SetLinearVelocity(linearVelocity[0], linearVelocity[1], linearVelocity[2]);
        iter.second->SetAngularVelocity(angularVelocity[0], angularVelocity[1], angularVelocity[2]);
    }

    for (auto &&iter : *simulation()->GetJointList())
    {
        while (true)
        {
            HingeJoint *hingeJoint = dynamic_cast<HingeJoint *>(iter.second.get());
            if (hingeJoint)
            {
                dJointID jointID = reinterpret_cast<dJointID>(hingeJoint->data());
                dVector3 axis, anchor;
                dJointGetHingeAnchor(jointID, anchor);
                dJointGetHingeAxis(jointID, axis);
                double angle = dJointGetHingeAngle(jointID);
                double angleRate = dJointGetHingeAngleRate(jointID);
                hingeJoint->setAnchor(&anchor[0]);
                hingeJoint->setAxis(&axis[0]);
                hingeJoint->setAngle(angle);
                hingeJoint->setAngleRate(angleRate);
                dJointFeedback *jointFeedback = dJointGetFeedback(jointID);
                hingeJoint->setForce(pgd::Vector3(jointFeedback->f1));
                hingeJoint->setTorque(pgd::Vector3(jointFeedback->t1));
                break;
            }
            break;
        }
    }

    for (size_t i = 0; i < simulation()->GetContactList()->size(); i++)
    {
        Contact *contact = simulation()->GetContactList()->at(i).get();
        dJointID jointID = reinterpret_cast<dJointID>(contact->data());
        dJointFeedback *jointFeedback = dJointGetFeedback(jointID);
        contact->setForce(pgd::Vector3(jointFeedback->f1));
        contact->setTorque(pgd::Vector3(jointFeedback->t1));
    }

    return 0;
}

// this is called by dSpaceCollide when two objects in space are
// potentially colliding.

void PhysXPhysicsEngine::NearCallback(void *data, dGeomID o1, dGeomID o2)
{
    PhysXPhysicsEngine *s = reinterpret_cast<PhysXPhysicsEngine *>(data);
    Geom *g1 = reinterpret_cast<Geom *>(dGeomGetData(o1));
    Geom *g2 = reinterpret_cast<Geom *>(dGeomGetData(o2));

    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    if (b1 == b2)
    {
        return; // it is never useful for two contacts on the same body to collide [I'm not sure if this every happens - FIX ME - set up a test]
    }

    if (s->simulation()->GetGlobal()->AllowConnectedCollisions() == false)
    {
        if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact)) return;
    }

    if (s->simulation()->GetGlobal()->AllowInternalCollisions() == false)
    {
        if (g1->GetGeomLocation() == g2->GetGeomLocation()) return;
    }

    if (g1->GetExcludeList()->size())
    {
        std::vector<Geom *> *excludeList = g1->GetExcludeList();
        for (size_t i = 0; i < excludeList->size(); i++)
        {
            if (excludeList->at(i) == g2) return;
        }
    }
    if (g2->GetExcludeList()->size())
    {
        std::vector<Geom *> *excludeList = g2->GetExcludeList();
        for (size_t i = 0; i < excludeList->size(); i++)
        {
            if (excludeList->at(i) == g1) return;
        }
    }

    std::vector<dContact> contact(size_t(MAX_CONTACTS), dContact{}); // in this case default initialisation is potentially useful
    // std::unique_ptr<dContact[]> contact = std::make_unique<dContact[]>(size_t(s->m_MaxContacts)); // but this version would be slightly quicker
    // the choice of std::max(cfm) and std::min(erp) means that the softest contact should be used
    double cfm = std::max(g1->GetContactSoftCFM(), g2->GetContactSoftCFM());
    double erp = std::min(g1->GetContactSoftERP(), g2->GetContactSoftERP());
    // just use the largest for mu, rho and bounce
    double mu = std::max(g1->GetContactMu(), g2->GetContactMu());
    double bounce = std::max(g1->GetContactBounce(), g2->GetContactBounce());
    double rho = std::max(g1->GetRho(), g2->GetRho());
    if (erp < 0) // the only one that needs checking because all the others are std::max so values <0 will never be chosen if one value is >0
    {
        if (g1->GetContactSoftERP() < 0) erp = g2->GetContactSoftERP();
        else erp = g1->GetContactSoftERP();
    }
    for (size_t i = 0; i < size_t(MAX_CONTACTS); i++)
    {
        contact[i].surface.mode = dContactApprox1;
        contact[i].surface.mu = mu;
        if (bounce >= 0)
        {
            contact[i].surface.bounce = bounce;
            contact[i].surface.mode += dContactBounce;
        }
        if (rho >= 0)
        {
            contact[i].surface.rho = rho;
            contact[i].surface.mode += dContactRolling;
        }
        if (cfm >= 0)
        {
            contact[i].surface.soft_cfm = cfm;
            contact[i].surface.mode += dContactSoftCFM;
        }
        if (erp >= 0)
        {
            contact[i].surface.soft_erp = erp;
            contact[i].surface.mode += dContactSoftERP;
        }
    }
    int numc = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact));
    if (numc)
    {
        for (size_t i = 0; i < size_t(numc); i++)
        {
            if (g1->GetAbort()) s->simulation()->SetContactAbort(g1->name());
            if (g2->GetAbort()) s->simulation()->SetContactAbort(g2->name());
            dJointID c;
            if (g1->GetAdhesion() == false && g2->GetAdhesion() == false)
            {
                c = dJointCreateContact(s->worldID(), s->contactGroup(), &contact[i]);
                dJointAttach(c, b1, b2);
                std::unique_ptr<Contact> myContact = std::make_unique<Contact>();
                myContact->setSimulation(s->simulation());
                auto jointFeedback = std::make_unique<dJointFeedback>();
                dJointSetFeedback(c, jointFeedback.get());
                myContact->setData(c);
                myContact->setPosition(pgd::Vector3(contact[i].geom.pos[0], contact[i].geom.pos[1], contact[i].geom.pos[2]));
                g1->AddContact(myContact.get());
                g2->AddContact(myContact.get());
                s->simulation()->GetContactList()->push_back(std::move(myContact));
                s->contactFeedbackList()->push_back(std::move(jointFeedback));
            }
            else
            {
                // FIX ME adhesive joints are added permanently and forces cannot be measured
                c = dJointCreateBall(s->worldID(), nullptr);
                dJointAttach(c, b1, b2);
                dJointSetBallAnchor(c, contact[i].geom.pos[0], contact[i].geom.pos[1], contact[i].geom.pos[2]);
            }
        }
    }
}

void PhysXPhysicsEngine::ODEMessageTrap(int num, const char *msg, va_list ap)
{
    fflush (stderr);
    fflush (stdout);
    fprintf (stderr,"\n%d: ", num);
    vfprintf (stderr, msg, ap);
    fprintf (stderr, "\n");
    fflush (stderr);

    // reliably acquire the size
    // from a copy of the variable argument array
    // and a functionally reliable call to mock the formatting
    va_list vaArgsCopy;
    va_copy(vaArgsCopy, ap);
    const int iLen = std::vsnprintf(NULL, 0, msg, vaArgsCopy);
    va_end(vaArgsCopy);

    // return a formatted string without risking memory mismanagement
    // and without assuming any compiler or platform specific behavior
    std::vector<char> zc(iLen + 1);
    std::vsnprintf(zc.data(), zc.size(), msg, ap);
    va_end(ap);

    m_messageText.assign(zc.data(), iLen);
    m_messageNumber = num;
    m_messageFlag = true;
}

bool PhysXPhysicsEngine::GetErrorMessage(int *messageNumber, std::string *messageText)
{
    bool messageFlag = m_messageFlag;
    m_messageFlag = false;
    *messageNumber = m_messageNumber;
    *messageText = m_messageText;
    return messageFlag;
}

std::vector<std::unique_ptr<dJointFeedback>> *PhysXPhysicsEngine::contactFeedbackList()
{
    return &m_contactFeedbackList;
}

std::map<std::string, std::unique_ptr<dJointFeedback>> *PhysXPhysicsEngine::jointFeedbackMap()
{
    return &m_jointFeedback;
}

dJointGroupID PhysXPhysicsEngine::contactGroup() const
{
    return m_contactGroup;
}

dSpaceID PhysXPhysicsEngine::spaceID() const
{
    return m_spaceID;
}

dWorldID PhysXPhysicsEngine::worldID() const
{
    return m_worldID;
}


