#include "MuJoCoPhysicsEngine.h"

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
#include "GSUtil.h"

#include <deque>

using namespace std::string_literals;

MuJoCoPhysicsEngine::MuJoCoPhysicsEngine()
{
}

MuJoCoPhysicsEngine::~MuJoCoPhysicsEngine()
{
    if (m_mjData) mj_deleteData(m_mjData);
    if (m_mjModel) mj_deleteModel(m_mjModel);
}

std::string *MuJoCoPhysicsEngine::Initialise(Simulation *theSimulation)
{
    std::string *err = PhysicsEngine::Initialise(theSimulation);
    if (err) { return err; }

    // create the MuJoCo xml versions of the main elements
    err = CreateTree();
    if (err) { return err; }

    char error[1000] = "";
    m_mjModel = LoadModelFromString(m_mjXML, error, sizeof(error));
    if (!m_mjModel)
    {
        setLastError("Error: MuJoCoPhysicsEngine error in LoadModelFromString\n"s + std::string(error));
        return lastErrorPtr();
    }

    m_mjData = mj_makeData(m_mjModel);

    return err;
}

std::string *MuJoCoPhysicsEngine::CreateTree()
{
    // with no hint just assume the biggest body is the root
    Body *maxMassBody = nullptr;
    double maxMass = -std::numeric_limits<double>::max();
    for (auto &&iter : *simulation()->GetBodyList())
    {
        Body *body = iter.second.get();
        double mass = body->GetMass();
        if (mass > maxMass)
        {
            maxMass = mass;
            maxMassBody = body;
        }
    }

    m_rootTreeBody.body = maxMassBody;
    m_jointLoopDetector.clear();
    m_jointLoopDetector.insert(m_rootTreeBody.body);
    m_jointsLeftToInclude.clear();
    m_jointsLeftToInclude.reserve(simulation()->GetJointList()->size());
    for (auto &&iter : *simulation()->GetJointList()) { m_jointsLeftToInclude.push_back(iter.second.get()); }
    m_bodiesLeftToInclude.clear();
    m_bodiesLeftToInclude.reserve(simulation()->GetBodyList()->size());
    m_bodiesLeftToInclude.push_back(&m_rootTreeBody);
    while (m_bodiesLeftToInclude.size())
    {
// #define DEBUG_CREATE_TREE
#ifdef DEBUG_CREATE_TREE
        for (auto &&iter : m_bodiesLeftToInclude)
        {
            std::cerr << "TreeBodies left " << m_bodiesLeftToInclude.size() << "\n";
            std::cerr << "TreeBody " << iter->body->name() << "\n";
            for (auto &&iter2 : iter->childList)
            {
                std::cerr << "Child " << iter2->body->name() << "\n";
            }
        }
        for (auto &&iter : m_jointLoopDetector)
        {
            std::cerr << "Loop detector bodies left " << m_jointLoopDetector.size() << "\n";
            std::cerr << "Body " << iter->name() << "\n";
         }
#endif
        TreeBody *currentBody = m_bodiesLeftToInclude.back();
        m_bodiesLeftToInclude.pop_back();
        auto iter = m_jointsLeftToInclude.begin();
        while (iter != m_jointsLeftToInclude.end())
        {
            if ((*iter)->body1() != currentBody->body && (*iter)->body2() != currentBody->body)
            {
                iter++;
                continue;
            }
            // we have found a link to a child
            if (m_jointLoopDetector.count(currentBody->body) > 1)
            {
                if (currentBody->body) setLastError(GSUtil::ToString("Error: MuJoCoPhysicsEngine error in CreateTree. Trying to add \"%s\" twice", currentBody->body->name().c_str()));
                else setLastError(GSUtil::ToString("Error: MuJoCoPhysicsEngine error in CreateTree. Trying to add \"%s\" twice", "World"));
                return lastErrorPtr();
            }
            std::unique_ptr<TreeBody> newTreeBody = std::make_unique<TreeBody>();
            newTreeBody->body = ((*iter)->body1() == currentBody->body) ? (*iter)->body2() : (*iter)->body1();
            newTreeBody->parent = currentBody;
            newTreeBody->jointToParent = *iter;
            m_bodiesLeftToInclude.push_back(newTreeBody.get());
            m_jointLoopDetector.insert(newTreeBody->body);
            currentBody->childList.push_back(std::move(newTreeBody));
            iter = m_jointsLeftToInclude.erase(iter);
        }
        if (m_jointsLeftToInclude.size() > 0 && m_bodiesLeftToInclude.size() == 0)
        {
            setLastError(GSUtil::ToString("Error: MuJoCoPhysicsEngine error in CreateTree. Body list exhausted before joint list"));
            return lastErrorPtr();
        }
    }

#ifdef DEBUG_CREATE_TREE
    std::deque<TreeBody *> toProcess;
    toProcess.push_back(&m_rootTreeBody);
    while (toProcess.size())
    {
        std::cerr << "TreeBody " << toProcess.front()->body->name() << "\n";
        for (auto &&iter2 : toProcess.front()->childList)
        {
            std::cerr << "Child " << iter2->body->name() << "\n";
            toProcess.push_back(iter2.get());
        }
        toProcess.pop_front();
    }
#endif

    // start building the XML
    XMLInitiateTag(*m_mjXML, "mujoco"s, {{"model"s, "GaitSym"s}});

    // set some options
    XMLInitiateTag(*m_mjXML, "option"s, {{"timestep"s, GSUtil::ToString(simulation()->GetGlobal()->StepSize())}});

    // create the world body
    XMLInitiateTag(*m_mjXML, "worldbody"s);

    int depth = 1;
    TreeBody *currentTreeBody = &m_rootTreeBody;
    while (depth)
    {
        CreateBody(const Body *body)
    }

    XMLTerminateTag(m_mjXML, "mujoco"s);

    return nullptr;
}

void MuJoCoPhysicsEngine::XMLInitiateTag(std::string *xmlString, const std::string &tag, const std::map<std::string, std::string> &attributes, bool terminate)
{
    xmlString->push_back('<');
    for (auto &&iter : attributes)
    {
        xmlString->append(iter.first + "=\""s + iter.second + "\" "s);
    }
    if (attributes.size() > 0) xmlString->pop_back();
    if (terminate) { xmlString->append("/>\n"s); }
    else { xmlString->append(">\n"s); }
}

void XMLTerminateTag(std::string *xmlString, const std::string &tag)
{
    xmlString->append("</"s + tag + ">\n"s);
}

std::string *MuJoCoPhysicsEngine::CreateBody(const Body *body)
{
    double mass, ixx, iyy, izz, ixy, izx, iyz;
    body->GetMass(&mass, &ixx, &iyy, &izz, &ixy, &izx, &iyz);
    pgd::Vector3 position = body->GetConstructionPosition();
    pgd::Quaternion quaternion(true);
    std::map<std::string, std::string> attributes;
    attributes["name"s] = body->name();
    attributes["pos"s] = GSUtil::ToString(position);
    attributes["quat"s] = GSUtil::ToString(quaternion);
    attributes["fullinertia"s] = GSUtil::ToString("%.17g %.17g %.17g %.17g %.17g %.17g", ixx, iyy, izz, ixy, izx, iyz);
    XMLInitiateTag(&m_mjXML, "body", attributes);
    return nullptr;
}

std::string *MuJoCoPhysicsEngine::CreateJoints()
{
#if 0
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

                pgd::Vector2 stops = hingeJoint->stops();
                double springConstant = hingeJoint->stopSpring();
                double dampingConstant = hingeJoint->stopDamp();
                revolute->setLimit(physx::PxJointAngularLimitPair(-stops[1], -stops[0], physx::PxSpring(springConstant, dampingConstant))); // note the stops are reversed from the ODE values
                revolute->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eLIMIT_ENABLED, true);
                revolute->userData = hingeJoint;
                m_jointMap[iter.first] = revolute;
                break;
            }
            break;
        }
    }
#endif
    return nullptr;
}


std::string *MuJoCoPhysicsEngine::CreateGeoms()
{
#if 0
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
            PlaneGeom *planeGeom = dynamic_cast<PlaneGeom *>(iter.second.get());
            if (planeGeom)
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
#endif
    return nullptr;
}

std::string *MuJoCoPhysicsEngine::MoveBodies()
{
#if 0
    for (auto &&iter : *simulation()->GetBodyList())
    {
        physx::PxRigidDynamic* rigidDynamic = m_bodyMap[iter.first];
        pgd::Vector3 position = iter.second->GetPosition();
        pgd::Quaternion quaternion = iter.second->GetQuaternion();
        physx::PxTransform transform(physx::PxVec3(position.x, position.y, position.z), physx::PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.n));
        rigidDynamic->setGlobalPose(transform);
    }
#endif
    return nullptr;
}

std::string *MuJoCoPhysicsEngine::Step()
{
#if 0
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
            HingeJoint *hingeJoint = dynamic_cast<HingeJoint *>(iter.second.get());
            if (hingeJoint)
            {
                physx::PxRevoluteJoint *joint = dynamic_cast<physx::PxRevoluteJoint *>(m_jointMap[iter.first]);
                if (!joint)
                {
                    std::cerr << "Error in MuJoCoPhysicsEngine::Step(): joint type mismatch\n";
                    continue;
                }
                physx::PxConstraint *constraint = joint->getConstraint();
                if (!constraint->isValid())
                {
                    std::cerr << "Error in MuJoCoPhysicsEngine::Step(): constraint not valid\n";
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
#endif
    return nullptr;
}


mjModel *MuJoCoPhysicsEngine::LoadModelFromString(const std::string &xml, char* error, int error_size, mjVFS* vfs)
{
    // register string resource provider if not registered before
    if (mjp_getResourceProvider("LoadModelFromString:") == nullptr)
    {
        mjpResourceProvider resourceProvider;
        mjp_defaultResourceProvider(&resourceProvider);
        resourceProvider.prefix = "LoadModelFromString";
        resourceProvider.open = +[](mjResource* resource)
        {
            resource->data = &(resource->name[strlen("LoadModelFromString:")]);
            return 1;
        };
        resourceProvider.read = +[](mjResource* resource, const void** buffer)
        {
            *buffer = resource->data;
            return (int) strlen((const char*) resource->data);
        };
        resourceProvider.close = +[](mjResource* resource) {};
        mjp_registerResourceProvider(&resourceProvider);
    }
    std::string str = "LoadModelFromString:" +  xml;
    return mj_loadXML(str.c_str(), vfs, error, error_size);
}
