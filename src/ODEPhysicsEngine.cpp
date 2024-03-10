#include "ODEPhysicsEngine.h"

#include "PGDMath.h"
#include "Simulation.h"
#include "Body.h"
#include "Muscle.h"
#include "FluidSac.h"
#include "Geom.h"
#include "HingeJoint.h"
#include "SphereGeom.h"
#include "PlaneGeom.h"

using namespace std::string_literals;

#define MAX_CONTACTS 64

std::string ODEPhysicsEngine::m_messageText;
int ODEPhysicsEngine::m_messageNumber = 0;
bool ODEPhysicsEngine::m_messageFlag = false;


ODEPhysicsEngine::ODEPhysicsEngine()
{
}

ODEPhysicsEngine::~ODEPhysicsEngine()
{
    // destroy the ODE world
    dSetMessageHandler(nullptr);
    dSetErrorHandler(nullptr);
    dSetDebugHandler(nullptr);
    if (m_contactGroup) dJointGroupDestroy(m_contactGroup);
    if (m_spaceID) dSpaceDestroy(m_spaceID);
    if (m_worldID) dWorldDestroy(m_worldID);
    dCloseODE();
}

int ODEPhysicsEngine::Initialise(Simulation *theSimulation)
{
    int err = PhysicsEngine::Initialise(theSimulation);
    if (err) { return err; }

    // initialise the ODE world
    dInitODE();
    m_worldID = dWorldCreate();
    m_spaceID = dHashSpaceCreate(nullptr); // FIX ME hash space is a compromise but this should probably be user controlled
    m_contactGroup = dJointGroupCreate(0);

    // set the error handlers
    dSetMessageHandler(ODEMessageTrap);
    dSetErrorHandler(ODEMessageTrap);
    dSetDebugHandler(ODEMessageTrap);

    // apply the global values
    Global *global = simulation()->GetGlobal();
    dWorldSetGravity(m_worldID, global->Gravity().x, global->Gravity().y, global->Gravity().z);
    dWorldSetERP(m_worldID, global->ERP());
    dWorldSetCFM(m_worldID, global->CFM());
    dWorldSetContactMaxCorrectingVel(m_worldID, global->ContactMaxCorrectingVel());
    dWorldSetContactSurfaceLayer(m_worldID, global->ContactSurfaceLayer());
    dWorldSetDamping(m_worldID, global->LinearDamping(), global->AngularDamping());

    // create the ODE versions of the main elements
    CreateBodies();
    CreateJoints();
    CreateGeoms();

    // And ODE requires that bodies be moved to their starting positions after joints have been created
    MoveBodies();

    return 0;
}

void ODEPhysicsEngine::CreateBodies()
{
    // first create the bodies
    const pgd::Quaternion zeroRotation( 1, 0, 0, 0);
    for (auto &&iter : *simulation()->GetBodyList())
    {
        dBodyID bodyID = dBodyCreate(m_worldID);
        dBodySetData(bodyID, iter.second.get());
        iter.second->setData(bodyID);
        double mass, ixx, iyy, izz, ixy, izx, iyz;
        iter.second->GetMass(&mass, &ixx, &iyy, &izz, &ixy, &izx, &iyz);
        dMass inertialProperties;
        dMassSetParameters(&inertialProperties, mass, 0, 0, 0, ixx, iyy, izz, ixy, izx, iyz);
        dBodySetMass(bodyID, &inertialProperties);
        pgd::Vector3 constructionPosition = iter.second->GetConstructionPosition();
        dBodySetPosition(bodyID, constructionPosition.x, constructionPosition.y, constructionPosition.z);
        dBodySetQuaternion(bodyID, zeroRotation.constData());
        pgd::Vector3 linearVelocity = iter.second->GetLinearVelocity();
        dBodySetLinearVel(bodyID, linearVelocity.x, linearVelocity.y, linearVelocity.z);
        pgd::Vector3 angularVelocity = iter.second->GetAngularVelocity();
        dBodySetAngularVel(bodyID, angularVelocity.x, angularVelocity.y, angularVelocity.z);
    }
}

void ODEPhysicsEngine::CreateJoints()
{
    for (auto &&iter : *simulation()->GetJointList())
    {
        while (true)
        {
            dJointID jointID = nullptr;
            auto jointFeedback = std::make_unique<dJointFeedback>();
            HingeJoint *hingeJoint = dynamic_cast<HingeJoint *>(iter.second.get());
            if (hingeJoint)
            {
                pgd::Vector3 anchor = hingeJoint->anchor();
                pgd::Vector3 axis = hingeJoint->axis();
                pgd::Vector2 stops = hingeJoint->stops();
                jointID = dJointCreateHinge(m_worldID, nullptr);
                dJointSetData(jointID, hingeJoint);
                iter.second->setData(jointID);
                dBodyID body1 = (hingeJoint->body1()->name() == "World"s) ? nullptr : reinterpret_cast<dBodyID>(hingeJoint->body1()->data());
                dBodyID body2 = (hingeJoint->body2()->name() == "World"s) ? nullptr : reinterpret_cast<dBodyID>(hingeJoint->body2()->data());
                dJointAttach(jointID, body1, body2);
                dJointSetFeedback(jointID, jointFeedback.get());
                dJointSetHingeAnchor(jointID, anchor.x, anchor.y, anchor.z);
                dJointSetHingeAxis(jointID, axis.x, axis.y, axis.z);
                double loStop = (stops[0] < -M_PI) ? -dInfinity : stops[0];
                double hiStop = (stops[1] > M_PI) ? dInfinity : stops[1];
                dJointSetHingeParam(jointID, dParamLoStop, loStop); // needs to be done twice to guarantee to see the stops properly
                dJointSetHingeParam(jointID, dParamHiStop, hiStop);
                dJointSetHingeParam(jointID, dParamLoStop, loStop);
                dJointSetHingeParam(jointID, dParamHiStop, hiStop);
                double springConstant = hingeJoint->stopSpring();
                double dampingConstant = hingeJoint->stopDamp();
                double integrationStep = simulation()->GetTimeIncrement();
                if (springConstant >= std::numeric_limits<double>::epsilon() && dampingConstant >= std::numeric_limits<double>::epsilon())
                {
                    double ERP = integrationStep * springConstant/(integrationStep * springConstant + dampingConstant);
                    double CFM = 1/(integrationStep * springConstant + dampingConstant);
                    dJointSetHingeParam (jointID, dParamStopCFM, CFM);
                    dJointSetHingeParam (jointID, dParamStopERP, ERP);
                }
                if (hingeJoint->stopBounce() >= 0) { dJointSetHingeParam (jointID, dParamBounce, hingeJoint->stopBounce()); }
                m_jointFeedback[iter.first] = std::move(jointFeedback);
                break;
            }
            break;
        }
    }
}

void ODEPhysicsEngine::CreateGeoms()
{
    for (auto &&iter : *simulation()->GetGeomList())
    {
        while (true)
        {
            dGeomID geomID = nullptr;
            SphereGeom *sphereGeom = dynamic_cast<SphereGeom *>(iter.second.get());
            if (sphereGeom)
            {
                double radius = sphereGeom->radius();
                pgd::Vector3 position = sphereGeom->GetPosition();
                pgd::Quaternion quaternion = sphereGeom->GetQuaternion();
                geomID = dCreateSphere(m_spaceID, radius);
                dGeomSetData(geomID, sphereGeom);
                iter.second->setData(geomID);
                dGeomSphereSetRadius(geomID, radius);
                dBodyID bodyID = reinterpret_cast<dBodyID>(sphereGeom->GetBody()->data());
                dGeomSetBody(geomID, bodyID);
                dGeomSetOffsetPosition(geomID, position.x, position.y, position.z);
                dGeomSetOffsetQuaternion(geomID, quaternion.constData());
                break;
            }
            PlaneGeom *planeGeom = dynamic_cast<PlaneGeom *>(iter.second.get());
            if (planeGeom)
            {
                double a, b, c, d;
                planeGeom->GetPlane(&a, &b, &c, &d);
                double length = std::sqrt(a * a + b * b + c * c);
                geomID = dCreatePlane(m_spaceID, a, b, c, d);
                dGeomSetData(geomID, planeGeom);
                iter.second->setData(geomID);
                dGeomSetBody(geomID, nullptr);
                break;
            }
            break;
        }
    }
}

void ODEPhysicsEngine::MoveBodies()
{
    for (auto &&iter : *simulation()->GetBodyList())
    {
        dBodyID bodyID = reinterpret_cast<dBodyID>(iter.second->data());
        pgd::Vector3 position = iter.second->GetPosition();
        dBodySetPosition(bodyID, position.x, position.y, position.z);
        pgd::Quaternion quaternion = iter.second->GetQuaternion();
        dBodySetQuaternion(bodyID, quaternion.constData());
    }
}

int ODEPhysicsEngine::Step()
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
            const PointForce *pf = pointForceList->at(i).get();
            if (pf->body)
            {
                dBodyAddForceAtPos(reinterpret_cast<dBodyID>(pf->body->data()),
                                   pf->vector[0] * tension, pf->vector[1] * tension, pf->vector[2] * tension,
                                   pf->point[0], pf->point[1], pf->point[2]);
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
                dBodyAddForceAtPos(reinterpret_cast<dBodyID>(pf->body->data()),
                                   pf->vector[0], pf->vector[1], pf->vector[2], pf->point[0], pf->point[1], pf->point[2]);
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
        dBodyAddRelForce(reinterpret_cast<dBodyID>(iter.second->data()), dragForce.x, dragForce.y, dragForce.z);
        dBodyAddRelTorque(reinterpret_cast<dBodyID>(iter.second->data()), dragTorque.x, dragTorque.y, dragTorque.z);
    }

    // run the simulation
    switch (simulation()->GetGlobal()->stepType())
    {
    case Global::World:
        dWorldStep(m_worldID, simulation()->GetGlobal()->StepSize());
        break;

    case Global::Quick:
        dWorldQuickStep(m_worldID, simulation()->GetGlobal()->StepSize());
        break;
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

void ODEPhysicsEngine::NearCallback(void *data, dGeomID o1, dGeomID o2)
{
    ODEPhysicsEngine *s = reinterpret_cast<ODEPhysicsEngine *>(data);
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

void ODEPhysicsEngine::ODEMessageTrap(int num, const char *msg, va_list ap)
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

bool ODEPhysicsEngine::GetErrorMessage(int *messageNumber, std::string *messageText)
{
    bool messageFlag = m_messageFlag;
    m_messageFlag = false;
    *messageNumber = m_messageNumber;
    *messageText = m_messageText;
    return messageFlag;
}

std::vector<std::unique_ptr<dJointFeedback>> *ODEPhysicsEngine::contactFeedbackList()
{
    return &m_contactFeedbackList;
}

std::map<std::string, std::unique_ptr<dJointFeedback>> *ODEPhysicsEngine::jointFeedbackMap()
{
    return &m_jointFeedback;
}

dJointGroupID ODEPhysicsEngine::contactGroup() const
{
    return m_contactGroup;
}

dSpaceID ODEPhysicsEngine::spaceID() const
{
    return m_spaceID;
}

dWorldID ODEPhysicsEngine::worldID() const
{
    return m_worldID;
}


