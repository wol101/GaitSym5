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

    std::ofstream of("D:\\wis\\Scratch\\test.xml");
    of << m_mjXML;
    of.close();

    char error[1000] = "";
    m_mjModel = LoadModelFromString(m_mjXML, error, sizeof(error));
    if (!m_mjModel)
    {
        setLastError("Error: MuJoCoPhysicsEngine error in LoadModelFromString\n"s + std::string(error));
        return lastErrorPtr();
    }

    m_mjData = mj_makeData(m_mjModel);
    mj_forward(m_mjModel, m_mjData);

    // move to start positions
    MoveBodies();

    return nullptr;
}

std::string *MuJoCoPhysicsEngine::CreateTree() // FIX ME - currently assumes a single connected model in the world
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
    XMLInitiateTag(&m_mjXML, "mujoco"s, {{"model"s, "GaitSym"s}});

    // set some options
    XMLInitiateTag(&m_mjXML, "option"s, {{"timestep"s, GSUtil::ToString(simulation()->GetGlobal()->StepSize())}}, true);

    // create the world body
    XMLInitiateTag(&m_mjXML, "worldbody"s);

    // create any geoms attached to world
    for (auto &&iter : *simulation()->GetGeomList())
    {
        if (iter.second->GetBody() == nullptr)
        {
            std::string *err = CreateGeom(iter.second.get());
            if (err) return err;
        }
    }

    // this creates the whole model recursively
    CreateBody(m_rootTreeBody);

    XMLTerminateTag(&m_mjXML, "worldbody"s);
    XMLTerminateTag(&m_mjXML, "mujoco"s);

    return nullptr;
}

void MuJoCoPhysicsEngine::XMLInitiateTag(std::string *xmlString, const std::string &tag, const std::map<std::string, std::string> &attributes, bool terminate)
{
    xmlString->append("<"s + tag + " "s);
    for (auto &&iter : attributes)
    {
        xmlString->append(iter.first + "=\""s + iter.second + "\" "s);
    }
    xmlString->pop_back();
    if (terminate) { xmlString->append("/>\n"s); }
    else { xmlString->append(">\n"s); }
}

void MuJoCoPhysicsEngine::XMLTerminateTag(std::string *xmlString, const std::string &tag)
{
    xmlString->append("</"s + tag + ">\n"s);
}

std::string *MuJoCoPhysicsEngine::CreateBody(const TreeBody &treeBody)
{
    Body *body = treeBody.body;
    pgd::Vector3 position = body->GetConstructionPosition();
    pgd::Quaternion quaternion(true);
    if (treeBody.parent)
    {
        Marker marker(treeBody.parent->body);
        marker.SetWorldPosition(position);
        position = marker.GetPosition();
        marker.SetWorldQuaternion(quaternion);
        quaternion = marker.GetQuaternion();
    }
    std::map<std::string, std::string> attributes;
    attributes["name"s] = body->name();
    attributes["pos"s] = GSUtil::ToString(position);
    attributes["quat"s] = GSUtil::ToString(quaternion);
    XMLInitiateTag(&m_mjXML, "body", attributes);

    double mass, ixx, iyy, izz, ixy, izx, iyz;
    body->GetMass(&mass, &ixx, &iyy, &izz, &ixy, &izx, &iyz);
    attributes.clear();
    attributes["pos"s] = GSUtil::ToString(pgd::Vector3());
    attributes["mass"s] = GSUtil::ToString(mass);
    attributes["fullinertia"s] = GSUtil::ToString("%.17g %.17g %.17g %.17g %.17g %.17g", ixx, iyy, izz, ixy, izx, iyz);
    XMLInitiateTag(&m_mjXML, "inertial", attributes, true);

    std::string *err = CreateJoint(treeBody.jointToParent);
    if (err) return err;

    for (auto &&iter : *simulation()->GetGeomList())
    {
        if (iter.second->GetBody() == body)
        {
            err = CreateGeom(iter.second.get());
            if (err) return err;
        }
    }

    for (auto &&iter : treeBody.childList)
    {
        err = CreateBody(*iter);
        if (err) return err;
    }

    XMLTerminateTag(&m_mjXML, "body");
    return nullptr;
}

std::string *MuJoCoPhysicsEngine::CreateJoint(const Joint *joint)
{
    std::map<std::string, std::string> attributes;
    if (!joint)
    {
        attributes["name"s] = GSUtil::ToString("root%02d", m_freeJointCount);
        m_freeJointCount++;
        XMLInitiateTag(&m_mjXML, "freejoint"s, attributes, true);
        return nullptr;
    }

    while (true)
    {
        const HingeJoint *hingeJoint = dynamic_cast<const HingeJoint *>(joint);
        if (hingeJoint)
        {
            Marker *marker1 = hingeJoint->body1Marker();
            // Marker *marker2 = hingeJoint->body2Marker();
            pgd::Vector3 p1 = marker1->GetPosition();
            // pgd::Vector3 p2 = marker2->GetPosition();
            pgd::Vector3 axis = marker1->GetAxis(Marker::X);
            pgd::Vector2 stops = hingeJoint->stops();
            // double springConstant = hingeJoint->stopSpring();
            // double dampingConstant = hingeJoint->stopDamp();
            attributes["name"s] = hingeJoint->name();
            attributes["axis"s] = GSUtil::ToString(axis);
            attributes["pos"s] = GSUtil::ToString(p1);
            attributes["limited"s] = GSUtil::ToString(true);
            attributes["range"s] = GSUtil::ToString(stops);
            XMLInitiateTag(&m_mjXML, "joint"s, attributes, true);
            break;
        }
        break;
    }
    return nullptr;
}


std::string *MuJoCoPhysicsEngine::CreateGeom(const Geom *geom)
{
    std::map<std::string, std::string> attributes;
    while (true)
    {
        const SphereGeom *sphereGeom = dynamic_cast<const SphereGeom *>(geom);
        if (sphereGeom)
        {
            double radius = sphereGeom->radius();
            pgd::Vector3 position = sphereGeom->GetPosition();
            pgd::Quaternion quaternion = sphereGeom->GetQuaternion();
            // double staticFriction = sphereGeom->GetContactMu();
            // double dynamicFriction = staticFriction;
            // double contactBounce = sphereGeom->GetContactBounce();
            attributes["name"s] = sphereGeom->name();
            attributes["type"s] = "sphere"s;
            attributes["size"s] = GSUtil::ToString(radius);
            attributes["pos"s] = GSUtil::ToString(position);
            attributes["quat"s] = GSUtil::ToString(quaternion);
            XMLInitiateTag(&m_mjXML, "geom"s, attributes, true);
            break;
        }
        const PlaneGeom *planeGeom = dynamic_cast<const PlaneGeom *>(geom);
        if (planeGeom)
        {
            Marker *marker = planeGeom->geomMarker();
            pgd::Vector3 position = marker->GetPosition();
            pgd::Vector3 zAxis = marker->GetAxis(Marker::Z);
            attributes["name"s] = planeGeom->name();
            attributes["type"s] = "plane"s;
            attributes["pos"s] = GSUtil::ToString(position);
            attributes["zaxis"s] = GSUtil::ToString(zAxis);
            attributes["size"s] = GSUtil::ToString(pgd::Vector3(1, 1, 1));
            XMLInitiateTag(&m_mjXML, "geom"s, attributes, true);
            break;
        }
        break;
    }
    return nullptr;
}

std::string *MuJoCoPhysicsEngine::MoveBodies()
{
    for (size_t i = 0; i < m_mjModel->nbody; i++)
    {
        mjtObj type = mjOBJ_BODY;
        std::string bodyName(m_mjModel->names + m_mjModel->name_bodyadr[i]);
        if (bodyName == "world"s) { continue; }
        Body *body = simulation()->GetBody(bodyName);
        if (!body)
        {
            setLastError(GSUtil::ToString("Error: MuJoCoPhysicsEngine::MoveBodies \"%s\" not found", bodyName.c_str()));
            return lastErrorPtr();
        }
        pgd::Vector3 position = body->GetPosition();
        pgd::Quaternion quaternion = body->GetQuaternion();
        std::string parentName(m_mjModel->names + m_mjModel->name_bodyadr[m_mjModel->body_parentid[i]]);
        Body *parent = nullptr;
        if (parentName != "world"s)
        {
            parent = simulation()->GetBody(parentName);
            if (!parent)
            {
                setLastError(GSUtil::ToString("Error: MuJoCoPhysicsEngine::MoveBodies \"%s\" not found", parentName.c_str()));
                return lastErrorPtr();
            }
        }
        Marker marker(parent);
        marker.SetWorldPosition(position);
        marker.SetWorldQuaternion(quaternion);

        int qposSize = mj_stateSize(m_mjModel, mjSTATE_QPOS);
        int qvelSize = mj_stateSize(m_mjModel, mjSTATE_QVEL);
        std::vector<mjtNum> qpos((size_t(qposSize)));
        std::vector<mjtNum> qvel((size_t(qvelSize)));
        mj_getState(m_mjModel, m_mjData, qpos.data(), mjSTATE_QPOS);
        mj_getState(m_mjModel, m_mjData, qpos.data(), mjSTATE_QVEL);
        for (int i = 0; i < qposSize; i++)
        {
            std::cerr << qpos[i] << "\n";
        }
    }

    return nullptr;
}

std::string *MuJoCoPhysicsEngine::Step()
{
#if 0
    // Apply Cartesian force and torque (outside xfrc_applied mechanism).
    MJAPI void mj_applyFT(const mjModel* m, mjData* d, const mjtNum force[3], const mjtNum torque[3],
                          const mjtNum point[3], int body, mjtNum* qfrc_target);

    // Set perturb pos,quat in d->mocap when selected body is mocap, and in d->qpos otherwise.
    // Write d->qpos only if flg_paused and subtree root for selected body has free joint.
    MJAPI void mjv_applyPerturbPose(const mjModel* m, mjData* d, const mjvPerturb* pert,
                                    int flg_paused);

    // Set perturb force,torque in d->xfrc_applied, if selected body is dynamic.
    MJAPI void mjv_applyPerturbForce(const mjModel* m, mjData* d, const mjvPerturb* pert);

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
