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
    err = CreateConnectedGroups();
    if (err) { return err; }

    err = CreateTree();
    if (err) { return err; }

#define DEBUG_MUJOCO_XML
#ifdef DEBUG_MUJOCO_XML
    std::ofstream of("C:\\Scratch\\test.xml");
    of << m_mjXML;
    of.close();
#endif

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

    // now the mjc body ids have been defined we can put them into the TreeBody tree
    for (auto &&iter : m_rootTreeBodyList) { InsertMJBodyIDs(&iter); }

    return nullptr;
}

std::string *MuJoCoPhysicsEngine::CreateConnectedGroups()
{
    std::map<std::string, Body *> untestedBodies;
    for (auto &&iter : *simulation()->GetBodyList()) { untestedBodies[iter.first] = iter.second.get(); }
    std::map<std::string, Joint *> untestedJoints;
    for (auto &&iter : *simulation()->GetJointList()) { untestedJoints[iter.first] = iter.second.get(); }

    while (untestedBodies.size())
    {
        std::map<std::string, Body *> bodiesToTest;
        auto bodyHandle = untestedBodies.extract(untestedBodies.begin());
        bodiesToTest[bodyHandle.key()] = bodyHandle.mapped();
        auto currentConnectedGroup = std::make_unique<std::map<std::string, Body *>>();
        (*currentConnectedGroup)[bodyHandle.key()] = bodyHandle.mapped();
        while (bodiesToTest.size() && untestedJoints.size())
        {
            bodyHandle = bodiesToTest.extract(bodiesToTest.begin());
            for (auto jointIter = untestedJoints.begin(); jointIter != untestedJoints.end(); /* no increment */)
            {
                if (jointIter->second->body1() == bodyHandle.mapped() || jointIter->second->body2() == bodyHandle.mapped())
                {
                    Body *linkedBody = jointIter->second->body1() == bodyHandle.mapped() ? jointIter->second->body2() : jointIter->second->body1();
                    auto linkedBodyIter = untestedBodies.find(linkedBody->name());
                    if (linkedBodyIter != untestedBodies.end()) // needed because we might have loops
                    {
                        bodiesToTest[linkedBody->name()] = linkedBody;
                        (*currentConnectedGroup)[linkedBody->name()] = linkedBody;
                        untestedBodies.erase(linkedBodyIter);
                    }
                    else
                    {
                        std::cerr << "Warning: MuJoCoPhysicsEngine::CreateConnectedGroups possible loop detected\n";
                    }
                    jointIter = untestedJoints.erase(jointIter);
                }
                else
                {
                    jointIter++;
                }
            }
        }
// #define DEBUG_MUJOCO_CREATE_CONNECTED_GROUPS
#ifdef DEBUG_MUJOCO_CREATE_CONNECTED_GROUPS
        std::cerr << "New Group\n";
        for (auto && iter : *currentConnectedGroup)
        {
            std::cerr << "currentConnectedGroup[" << iter.first << "] = " << iter.second->name() << "\n";
        }
#endif
        m_connectedGroups.push_back(std::move(currentConnectedGroup));
    }
    return nullptr;
}



std::string *MuJoCoPhysicsEngine::CreateTree()
{
    // with no hint just assume the biggest body is the root
    for (auto &&groupIter : m_connectedGroups)
    {
    Body *maxMassBody = nullptr;
    double maxMass = -std::numeric_limits<double>::max();
        for (auto &&bodyIter : *groupIter)
        {
            Body *body = bodyIter.second;
            double mass = body->GetMass();
            if (mass > maxMass)
            {
                maxMass = mass;
                maxMassBody = body;
            }
        }
        m_rootTreeBodyList.emplace_back();
        m_rootTreeBodyList.back().body = maxMassBody;
    }

    for (size_t i = 0; i < m_rootTreeBodyList.size(); i++)
    {
        m_jointLoopDetector.clear();
        m_jointLoopDetector.insert(m_rootTreeBodyList[i].body);
        m_jointsLeftToInclude.clear();
        m_jointsLeftToInclude.reserve(simulation()->GetJointList()->size());
        for (auto &&iter : *simulation()->GetJointList()) { m_jointsLeftToInclude.push_back(iter.second.get()); }
        m_bodiesLeftToInclude.clear();
        m_bodiesLeftToInclude.reserve(simulation()->GetBodyList()->size());
        m_bodiesLeftToInclude.push_back(&m_rootTreeBodyList[i]);
        while (m_bodiesLeftToInclude.size())
        {
// #define DEBUG_MUJOCO_CREATE_TREE
#ifdef DEBUG_MUJOCO_CREATE_TREE
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
        }
    }

#ifdef DEBUG_MUJOCO_CREATE_TREE
    for (auto &&iter : m_rootTreeBodyList)
    {
        std::deque<TreeBody *> toProcess;
        toProcess.push_back(&iter);
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
    }
#endif

    // start building the XML
    XMLInitiateTag(&m_mjXML, "mujoco"s, {{"model"s, "GaitSym"s}});

    // set some options
    XMLInitiateTag(&m_mjXML, "compiler"s, {{"angle"s, "radian"s}, {"autolimits"s, "true"s}}, true);
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
    for (auto &&iter : m_rootTreeBodyList)
    {
        CreateBody(iter);
    }

    XMLTerminateTag(&m_mjXML, "worldbody"s);

    XMLInitiateTag(&m_mjXML, "sensor"s);
    m_mjXML.append(m_mjXMLSensors);
    XMLTerminateTag(&m_mjXML, "sensor"s);

    XMLTerminateTag(&m_mjXML, "mujoco"s);

    return nullptr;
}

void MuJoCoPhysicsEngine::InsertMJBodyIDs(TreeBody *treeBody)
{
    int bodyID = mj_name2id(m_mjModel, mjOBJ_BODY, treeBody->body->name().c_str());
    treeBody->bodyID = bodyID;
    m_flatTreeBodyList.push_back(treeBody);
    if (treeBody->childList.size() == 0) return;
    for (auto &&child : treeBody->childList)
    {
        InsertMJBodyIDs(child.get());
    }
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
        pgd::Vector3 parentConstructionPosition = treeBody.parent->body->GetConstructionPosition(); // only need position because construction rotation is always zero
        position = position - parentConstructionPosition;
    }
    std::map<std::string, std::string> attributes;
    attributes["name"s] = body->name();
    attributes["pos"s] = GSUtil::ToString(position);
    attributes["quat"s] = GSUtil::ToString(quaternion);
    XMLInitiateTag(&m_mjXML, "body", attributes);
    XMLInitiateTag(&m_mjXML, "geom", {{"type"s, "sphere"s}, {"size"s, ".1"s}}, true); // this is just a CM marker for debugging

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
        if (const HingeJoint *hingeJoint = dynamic_cast<const HingeJoint *>(joint))
        {
            // Marker *marker1 = hingeJoint->body1Marker();
            // pgd::Vector3 p1 = marker1->GetPosition();
            // pgd::Vector3 axis1 = marker1->GetAxis(Marker::X);
            // attributes["axis"s] = GSUtil::ToString(axis1);
            // attributes["pos"s] = GSUtil::ToString(p1);
            Marker *marker2 = hingeJoint->body2Marker();
            pgd::Vector3 p2 = marker2->GetPosition();
            pgd::Vector3 axis2 = marker2->GetAxis(Marker::X);
            pgd::Vector2 stops = hingeJoint->stops();
            // double springConstant = hingeJoint->stopSpring();
            // double dampingConstant = hingeJoint->stopDamp();
            attributes["name"s] = hingeJoint->name();
            attributes["axis"s] = GSUtil::ToString(axis2);
            attributes["pos"s] = GSUtil::ToString(p2);
            attributes["limited"s] = GSUtil::ToString(true);
            pgd::Vector2 reversedStops(-stops[1], -stops[0]);
            attributes["range"s] = GSUtil::ToString(reversedStops);
            XMLInitiateTag(&m_mjXML, "joint"s, attributes, true);
            // put a site on the joint
            attributes.clear();
            attributes["name"s] = hingeJoint->name() + "_site"s;
            attributes["pos"s] = GSUtil::ToString(p2);
            XMLInitiateTag(&m_mjXML, "site"s, attributes, true);
            // we also need sensors to get reaction forces and torques
            attributes.clear();
            attributes["name"s] = hingeJoint->name() + "_jointpos"s;
            attributes["joint"s] = hingeJoint->name();
            XMLInitiateTag(&m_mjXMLSensors, "jointpos"s, attributes, true);
            attributes["name"s] = hingeJoint->name() + "_jointvel"s;
            attributes["joint"s] = hingeJoint->name();
            XMLInitiateTag(&m_mjXMLSensors, "jointvel"s, attributes, true);
            attributes.clear();
            attributes["name"s] = hingeJoint->name() + "_force"s;
            attributes["site"s] = hingeJoint->name() + "_site"s;
            XMLInitiateTag(&m_mjXMLSensors, "force"s, attributes, true);
            attributes["name"s] = hingeJoint->name() + "_torque"s;
            XMLInitiateTag(&m_mjXMLSensors, "torque"s, attributes, true);
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
        if (const SphereGeom *sphereGeom = dynamic_cast<const SphereGeom *>(geom))
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
        if (const PlaneGeom *planeGeom = dynamic_cast<const PlaneGeom *>(geom))
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
    // in mucojo everything is done via the joints
    // these are specified in mjModel as
    // int*      jnt_type;             // type of joint (mjtJoint)                 (njnt x 1)
    // int*      jnt_qposadr;          // start addr in 'qpos' for joint's data    (njnt x 1)
    // int*      jnt_dofadr;           // start addr in 'qvel' for joint's data    (njnt x 1)
    // int*      jnt_bodyid;           // id of joint's body                       (njnt x 1)
    // and these point to values in mjData that can be set at the beginning of the simulation
    // mjtNum* qpos;              // position                                         (nq x 1)
    // mjtNum* qvel;              // velocity                                         (nv x 1)
    // the number of values in qpos and qvel varies with the joint type
    // mjtJoint = mjJNT_FREE has 7 values in qpos and 3 in qvel
    // mjtJoint = mjJNT_HINGE has 1 in qpos and 1 in qvel
    // note: these state values are sticky unless something like mj_resetData is called

    for (size_t jointID = 0; jointID < m_mjModel->njnt; jointID++)
    {
        int jnt_type = m_mjModel->jnt_type[jointID];
        int jnt_qposadr = m_mjModel->jnt_qposadr[jointID];
        int jnt_dofadr = m_mjModel->jnt_dofadr[jointID];
        int jnt_bodyid = m_mjModel->jnt_bodyid[jointID];
        std::string jointName(mj_id2name(m_mjModel, mjOBJ_JOINT, int(jointID)));
        std::string bodyName(mj_id2name(m_mjModel, mjOBJ_BODY, int(jnt_bodyid)));
        switch (jnt_type)
        {
        case mjJNT_FREE:
        {
#ifdef DEBUG_MUJOCO_MOVE_BODIES
            pgd::Vector3 p(m_mjData->qpos[jnt_qposadr + 0], m_mjData->qpos[jnt_qposadr + 1], m_mjData->qpos[jnt_qposadr + 2]);
            pgd::Quaternion q(m_mjData->qpos[jnt_qposadr + 3], m_mjData->qpos[jnt_qposadr + 4], m_mjData->qpos[jnt_qposadr + 5], m_mjData->qpos[jnt_qposadr + 6]);
            pgd::Vector3 v(m_mjData->qvel[jnt_dofadr + 0], m_mjData->qvel[jnt_dofadr + 1], m_mjData->qvel[jnt_dofadr + 2]);
            pgd::Vector3 av(m_mjData->qvel[jnt_dofadr + 3], m_mjData->qvel[jnt_dofadr + 4], m_mjData->qvel[jnt_dofadr + 5]);
            std::cerr << "Joint Name = " << jointName << "\n";
            std::cerr << "Position = " << GSUtil::ToString(p) << "\n";
            std::cerr << "Quaternion = " << GSUtil::ToString(q) << "\n";
            std::cerr << "Velocity = " << GSUtil::ToString(v) << "\n";
            std::cerr << "Angular Velocity = " << GSUtil::ToString(av) << "\n";
#endif
            Body *body = simulation()->GetBody(bodyName);
            if (!body)
            {
                setLastError(GSUtil::ToString("Error: MuJoCoPhysicsEngine::MoveBodies \"%s\" body not found", bodyName.c_str()));
                return lastErrorPtr();
            }
            pgd::Vector3 p = body->GetPosition();
            pgd::Quaternion q = body->GetQuaternion();
            pgd::Vector3 v = body->GetLinearVelocity();
            pgd::Vector3 av = body->GetAngularVelocity();
            // now set the values in the MuJoCo data structure
            m_mjData->qpos[jnt_qposadr + 0] = p.x; m_mjData->qpos[jnt_qposadr + 1] = p.y; m_mjData->qpos[jnt_qposadr + 2] = p.z;
            m_mjData->qpos[jnt_qposadr + 3] = q.n; m_mjData->qpos[jnt_qposadr + 4] = q.x; m_mjData->qpos[jnt_qposadr + 5] = q.y; m_mjData->qpos[jnt_qposadr + 6] = q.z;
            m_mjData->qvel[jnt_dofadr + 0] = v.x; m_mjData->qvel[jnt_dofadr + 1] = v.y; m_mjData->qvel[jnt_dofadr + 2] = v.z;
            m_mjData->qvel[jnt_dofadr + 3] = av.x; m_mjData->qvel[jnt_dofadr + 4] = av.y; m_mjData->qvel[jnt_dofadr + 5] = av.z;
            break;
        }
        case mjJNT_HINGE:
        {
#ifdef DEBUG_MUJOCO_MOVE_BODIES
            double a = m_mjData->qpos[jnt_qposadr];
            double av = m_mjData->qvel[jnt_dofadr];
            Joint *joint = simulation()->GetJoint(jointName);
            pgd::Quaternion rotation = joint->GetWorldRotation();
            pgd::Matrix3x3 basis = joint->body1Marker()->GetWorldBasis();
            pgd::Vector3 eulerAngles = pgd::MakeEulerAnglesFromQRadian(rotation, basis);
            pgd::Vector3 angularVelocity = joint->body1Marker()->GetVector(joint->GetWorldAngularVelocity());
            double angle; pgd::Vector3 axis;
            pgd::MakeAxisAngleFromQ(rotation, &axis, &angle);
            std::cerr << "Joint Name = " << jointName << "\n";
            std::cerr << "Angle = " << GSUtil::ToString(a) << "\n";
            std::cerr << "Angular Velocity = " << GSUtil::ToString(av) << "\n";
            std::cerr << "Axis = " << GSUtil::ToString(axis) << " Angle = " << pgd::RadToDeg(angle) << " degrees\n";
            std::cerr << "Basis = " << GSUtil::ToString(basis) << "\n";
            std::cerr << "Euler Angles = " << GSUtil::ToString(eulerAngles) << "\n";
            std::cerr << "Angular Velocity = " << GSUtil::ToString(angularVelocity) << "\n";
#endif
            Joint *joint = simulation()->GetJoint(jointName);
            if (!joint)
            {
                setLastError(GSUtil::ToString("Error: MuJoCoPhysicsEngine::MoveBodies \"%s\" joint not found", jointName.c_str()));
                return lastErrorPtr();
            }
            pgd::Quaternion rotation = joint->GetWorldRotation();
            pgd::Matrix3x3 basis = joint->body1Marker()->GetWorldBasis();
            pgd::Vector3 eulerAngles = pgd::MakeEulerAnglesFromQRadian(rotation, basis);
            pgd::Vector3 angularVelocity = joint->body1Marker()->GetVector(joint->GetWorldAngularVelocity());
            m_mjData->qpos[jnt_qposadr] = eulerAngles.x;
            m_mjData->qvel[jnt_dofadr] = angularVelocity.x;
            break;
        }
        default:
            setLastError(GSUtil::ToString("Error: MuJoCoPhysicsEngine::MoveBodies \"%s\" unimplmented joint type", jointName.c_str()));
            return lastErrorPtr();
        }
    }
    return nullptr;
}

std::string *MuJoCoPhysicsEngine::Step()
{
    // apply the point forces from the muscles
    // choices are to apply the forces and torques to the bodies directly using xfrc_applied
    // or to convert to qfrc_applied using mj_applyFT
    std::vector<double> qfrc_target(m_mjModel->nv);
    for (auto &&iter :  *simulation()->GetMuscleList())
    {
        std::vector<std::unique_ptr<PointForce>> *pointForceList = iter.second->GetPointForceList();
        double tension = iter.second->GetTension();
        for (unsigned int i = 0; i < pointForceList->size(); i++)
        {
            const PointForce *pf = pointForceList->at(i).get();
            if (pf->body)
            {
                pgd::Vector3 force = pf->vector * tension;
                int bodyID = mj_name2id(m_mjModel, mjOBJ_BODY, pf->body->name().c_str());
                mj_applyFT(m_mjModel, m_mjData, force.constData(), nullptr, pf->point.constData(), bodyID, qfrc_target.data());
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
                int bodyID = mj_name2id(m_mjModel, mjOBJ_BODY, pf->body->name().c_str());
                mj_applyFT(m_mjModel, m_mjData, pf->vector.constData(), nullptr, pf->point.constData(), bodyID, qfrc_target.data());
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
        int bodyID = mj_name2id(m_mjModel, mjOBJ_BODY, iter.first.c_str());
        mj_applyFT(m_mjModel, m_mjData, worldDragForce.constData(), worldDragTorque.constData(), iter.second->GetPosition().constData(), bodyID, qfrc_target.data());
    }

    // copy the accumulated qfrc values to the main data structure
    std::copy_n(qfrc_target.data(), m_mjModel->nv, m_mjData->qfrc_applied);

    // NB. This is the simple case where we simply add the passive forces and step the model
    // MuJoCo allows the step to be split so recalculated velocities can be used to generate forces
    // this can be done using the mjcb_passive callback (which works with RK4 integrator)
    // of by using mj_step1 and mj_step2 instead of mj_step (which does not work with RK4)
    mj_step(m_mjModel, m_mjData);

    // update the objects with the new data
    for (size_t i = 0; i < m_flatTreeBodyList.size(); i++) // m_flatTreeBodyList defines the bodies in tree order so there are no parent defined after their children
    {
        TreeBody *treeBody = m_flatTreeBodyList[i];
        int bodyID = treeBody->bodyID;
        Body *body = treeBody->body;
        pgd::Vector3 p(m_mjData->xpos[bodyID * 3 + 0], m_mjData->xpos[bodyID * 3 + 1], m_mjData->xpos[bodyID * 3 + 2]);
        pgd::Quaternion q(m_mjData->xquat[bodyID * 4 + 0], m_mjData->xquat[bodyID * 4 + 1], m_mjData->xquat[bodyID * 4 + 2], m_mjData->xquat[bodyID * 4 + 3]);
        pgd::Vector3 av(m_mjData->cvel[bodyID * 6 + 0], m_mjData->cvel[bodyID * 6 + 1], m_mjData->cvel[bodyID * 6 + 2]);
        pgd::Vector3 v(m_mjData->cvel[bodyID * 6 + 3], m_mjData->cvel[bodyID * 6 + 4], m_mjData->cvel[bodyID * 6 + 5]);
        // if (treeBody->parent)
        // {
        //     Marker marker(treeBody->parent->body);
        //     p = marker.GetWorldPosition(p);
        //     q = marker.GetWorldQuaternion(q);
        //     av = marker.GetWorldVector(av);
        //     v = marker.GetWorldVector(v);
        // }
        body->SetPosition(p);
        body->SetQuaternion(q);
        body->SetLinearVelocity(v);
        body->SetAngularVelocity(av);
#ifdef DEBUG_MUJOCO_STEP
        std::cerr << "\nBody Name = " << body->name() << "\n";
        std::cerr << "Body ID = " << bodyID << " Name from ID = " << mj_id2name(m_mjModel, mjOBJ_BODY, bodyID) << "\n";
        std::cerr << "Position = " << GSUtil::ToString(p) << "\n";
        std::cerr << "Quaternion = " << GSUtil::ToString(q) << "\n";
        std::cerr << "Velocity = " << GSUtil::ToString(v) << "\n";
        std::cerr << "Angular Velocity = " << GSUtil::ToString(av) << "\n";
#endif
    }

    for (auto &&iter : *simulation()->GetJointList())
    {
        while (true)
        {
            if (HingeJoint *hingeJoint = dynamic_cast<HingeJoint *>(iter.second.get()))
            {
                int jointID = mj_name2id(m_mjModel, mjOBJ_JOINT, hingeJoint->name().c_str());
                int jnt_type = m_mjModel->jnt_type[jointID];
                assert(jnt_type == mjJNT_HINGE);
                // int jnt_qposadr = m_mjModel->jnt_qposadr[jointID]; // not used
                // int jnt_dofadr = m_mjModel->jnt_dofadr[jointID]; // not used
                pgd::Vector3 anchor(m_mjData->xanchor[jointID * 3 + 0], m_mjData->xanchor[jointID * 3 + 1], m_mjData->xanchor[jointID * 3 + 2]);
                pgd::Vector3 axis(m_mjData->xaxis[jointID * 3 + 0], m_mjData->xaxis[jointID * 3 + 1], m_mjData->xaxis[jointID * 3 + 2]);
                // a hinge joint only has 1 dof
                // pgd::Vector3 constraintTorque(m_mjData->qfrc_constraint[jnt_dofadr * 3 + 0], m_mjData->qfrc_constraint[jnt_dofadr * 3 + 1], m_mjData->qfrc_constraint[jnt_dofadr * 3 + 2]);
                // pgd::Vector3 constraintForce(m_mjData->qfrc_constraint[jnt_dofadr * 3 + 4], m_mjData->qfrc_constraint[jnt_dofadr * 3 + 5], m_mjData->qfrc_constraint[jnt_dofadr * 3 + 6]);
                // hingeJoint->setForce(constraintForce);
                // hingeJoint->setTorque(constraintTorque);
                int jointposSensorID = mj_name2id(m_mjModel, mjOBJ_SENSOR, (hingeJoint->name() + "_jointpos"s).c_str());
                int jointposSensorDim = m_mjModel->sensor_dim[jointposSensorID];
                assert(jointposSensorDim == 1);
                mjtNum *jointposSensorPtr = m_mjData->sensordata + m_mjModel->sensor_adr[jointposSensorID];
                mjtNum angle = *jointposSensorPtr;
                int jointvelSensorID = mj_name2id(m_mjModel, mjOBJ_SENSOR, (hingeJoint->name() + "_jointvel"s).c_str());
                int jointvelSensorDim = m_mjModel->sensor_dim[jointvelSensorID];
                assert(jointvelSensorDim == 1);
                mjtNum *jointvelSensorPtr = m_mjData->sensordata + m_mjModel->sensor_adr[jointvelSensorID];
                mjtNum angleRate = *jointvelSensorPtr;
                int jointforceSensorID = mj_name2id(m_mjModel, mjOBJ_SENSOR, (hingeJoint->name() + "_force"s).c_str());
                int jointforceSensorDim = m_mjModel->sensor_dim[jointforceSensorID];
                assert(jointforceSensorDim == 3);
                mjtNum *jointforceSensorPtr = m_mjData->sensordata + m_mjModel->sensor_adr[jointforceSensorID];
                pgd::Vector3 jointforce(jointforceSensorPtr[0], jointforceSensorPtr[1], jointforceSensorPtr[2]);
                int jointtorqueSensorID = mj_name2id(m_mjModel, mjOBJ_SENSOR, (hingeJoint->name() + "_torque"s).c_str());
                int jointtorqueSensorDim = m_mjModel->sensor_dim[jointtorqueSensorID];
                assert(jointtorqueSensorDim == 3);
                mjtNum *jointtorqueSensorPtr = m_mjData->sensordata + m_mjModel->sensor_adr[jointtorqueSensorID];
                pgd::Vector3 jointtorque(jointtorqueSensorPtr[0], jointtorqueSensorPtr[1], jointtorqueSensorPtr[2]);
                // FIX ME
                hingeJoint->setAnchor(anchor); // this probably doesn't change
                hingeJoint->setAxis(axis); // this probably doesn't change
                hingeJoint->setAngle(angle); // and these should be the same as the internally calculated values
                hingeJoint->setAngleRate(angleRate); // and these should be the same as the internally calculated values
                Marker marker(iter.second.get()->body1());
                hingeJoint->setForce(marker.GetWorldVector(jointforce));
                hingeJoint->setTorque(marker.GetWorldVector(jointtorque));
                break;
            }
            break;
        }
    }

    simulation()->GetContactList()->clear();
    // double timeStep =simulation()->GetGlobal()->StepSize();
    // for (size_t i = 0; i < g_contactReportCallback.contactData()->size(); i++)
    // {
    //     physx::PxActor *actors[2];
    //     actors[0] = g_contactReportCallback.contactData()->at(i).actors[0];
    //     actors[1] = g_contactReportCallback.contactData()->at(i).actors[1];
    //     for (size_t j = 0; j < g_contactReportCallback.contactData()->at(i).positions.size(); j++)
    //     {
    //         physx::PxVec3 position = g_contactReportCallback.contactData()->at(i).positions[j];
    //         physx::PxVec3 impulse = g_contactReportCallback.contactData()->at(i).impulses[j];
    //         physx::PxShape *shape1 = g_contactReportCallback.contactData()->at(i).shapes[j * 2];
    //         physx::PxShape *shape2 = g_contactReportCallback.contactData()->at(i).shapes[j * 2 + 1];
    //         std::unique_ptr<Contact> myContact = std::make_unique<Contact>();
    //         myContact->setSimulation(simulation());
    //         myContact->setPosition(pgd::Vector3(position[0], position[1], position[2]));
    //         myContact->setForce(pgd::Vector3(impulse[0] / timeStep, impulse[1] / timeStep, impulse[2] / timeStep));
    //         Geom *geom1 = reinterpret_cast<Geom *>(shape1->userData);
    //         Geom *geom2 = reinterpret_cast<Geom *>(shape2->userData);
    //         geom1->AddContact(myContact.get());
    //         geom2->AddContact(myContact.get());
    //         myContact->setBody1(geom1->GetBody());
    //         myContact->setBody2(geom2->GetBody());
    //         simulation()->GetContactList()->push_back(std::move(myContact));
    //     }
    // }
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
