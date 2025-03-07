/*
 *  ODEPhysicsEngine.cpp
 *  GaitSym5
 *
 *  Created by Bill Sellers on 20/03/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#include "ODEPhysicsEngine.h"

#include "PGDMath.h"
#include "Simulation.h"
#include "Body.h"
#include "Muscle.h"
#include "FluidSac.h"
#include "Geom.h"
#include "HingeJoint.h"
#include "FixedJoint.h"
#include "BallJoint.h"
#include "SphereGeom.h"
#include "PlaneGeom.h"
#include "Marker.h"
#include "MagicMuscle.h"
#include "GSUtil.h"

#include "collision_util.h"

using namespace std::string_literals;
namespace GaitSym {

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

std::string *ODEPhysicsEngine::Initialise(Simulation *theSimulation)
{
    std::string *err = PhysicsEngine::Initialise(theSimulation);
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

    return nullptr;
}

std::string *ODEPhysicsEngine::CreateBodies()
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
    return nullptr;
}

std::string *ODEPhysicsEngine::CreateJoints()
{
    for (auto &&iter : *simulation()->GetJointList())
    {
        while (true)
        {
            dJointID jointID = nullptr;
            auto jointFeedback = std::make_unique<dJointFeedback>();
            if (HingeJoint *hingeJoint = dynamic_cast<HingeJoint *>(iter.second.get()))
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
                dJointSetHingeParam(jointID, dParamLoStop, loStop); // needs to be done twice to guarantee to set the stops properly
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
            if (BallJoint *ballJoint = dynamic_cast<BallJoint *>(iter.second.get()))
            {
                pgd::Vector3 anchor = ballJoint->anchor();
                jointID = dJointCreateBall(m_worldID, nullptr);
                dJointSetData(jointID, ballJoint);
                iter.second->setData(jointID);
                dBodyID body1 = (ballJoint->body1()->name() == "World"s) ? nullptr : reinterpret_cast<dBodyID>(ballJoint->body1()->data());
                dBodyID body2 = (ballJoint->body2()->name() == "World"s) ? nullptr : reinterpret_cast<dBodyID>(ballJoint->body2()->data());
                dJointAttach(jointID, body1, body2);
                dJointSetFeedback(jointID, jointFeedback.get());
                dJointSetBallAnchor(jointID, anchor.x, anchor.y, anchor.z);
                // ODE stops on ball joints are implemented as AMotors
                if (auto stops = ballJoint->stops())
                {
                    dJointID motorJointID = dJointCreateAMotor (m_worldID, nullptr);
                    dJointAttach(motorJointID, body1, body2);
                    // auto motorJointFeedback = std::make_unique<dJointFeedback>(); // at some point I might want to think about how this could work
                    // dJointSetFeedback(motorJointID, motorJointFeedback.get());
                    dJointSetAMotorMode(motorJointID, dAMotorUser);
                    dJointSetAMotorNumAxes(motorJointID, 3);
                    int axisMode = 1; // axisMode: 0 global, 1 relative to body 1, 2 relative to body
                    pgd::Vector3 x, y, z;
                    ballJoint->body1Marker()->GetBasis(&x, &y, &z);
                    dJointSetAMotorAxis(motorJointID, 0, axisMode, x.x, x.y, x.z);
                    dJointSetAMotorAxis(motorJointID, 1, axisMode, y.x, y.y, y.z);
                    dJointSetAMotorAxis(motorJointID, 2, axisMode, z.x, z.y, z.z);
                    // needs to be done twice to guarantee to set the stops properly
                    dJointSetAMotorParam(motorJointID, dParamLoStop1, (*stops)[0].x);
                    dJointSetAMotorParam(motorJointID, dParamHiStop1, (*stops)[0].y);
                    dJointSetAMotorParam(motorJointID, dParamLoStop2, (*stops)[1].x);
                    dJointSetAMotorParam(motorJointID, dParamHiStop2, (*stops)[1].y);
                    dJointSetAMotorParam(motorJointID, dParamLoStop3, (*stops)[2].x);
                    dJointSetAMotorParam(motorJointID, dParamHiStop3, (*stops)[2].y);
                    dJointSetAMotorParam(motorJointID, dParamLoStop1, (*stops)[0].x);
                    dJointSetAMotorParam(motorJointID, dParamHiStop1, (*stops)[0].y);
                    dJointSetAMotorParam(motorJointID, dParamLoStop2, (*stops)[1].x);
                    dJointSetAMotorParam(motorJointID, dParamHiStop2, (*stops)[1].y);
                    dJointSetAMotorParam(motorJointID, dParamLoStop3, (*stops)[2].x);
                    dJointSetAMotorParam(motorJointID, dParamHiStop3, (*stops)[2].y);
                    m_amotorList.push_back(motorJointID);
                }
                m_jointFeedback[iter.first] = std::move(jointFeedback);
                break;
            }
            if (FixedJoint *fixedJoint = dynamic_cast<FixedJoint *>(iter.second.get()))
            {
                jointID = dJointCreateFixed(m_worldID, nullptr);
                dJointSetData(jointID, fixedJoint);
                iter.second->setData(jointID);
                dBodyID body1 = (fixedJoint->body1()->name() == "World"s) ? nullptr : reinterpret_cast<dBodyID>(fixedJoint->body1()->data());
                dBodyID body2 = (fixedJoint->body2()->name() == "World"s) ? nullptr : reinterpret_cast<dBodyID>(fixedJoint->body2()->data());
                dJointAttach(jointID, body1, body2);
                dJointSetFeedback(jointID, jointFeedback.get());
                m_jointFeedback[iter.first] = std::move(jointFeedback);
                dJointSetFixed(jointID);
                break;
            }
            break;
        }
    }
    return nullptr;
}

std::string *ODEPhysicsEngine::CreateGeoms()
{
    for (auto &&iter : *simulation()->GetGeomList())
    {
        while (true)
        {
            dGeomID geomID = nullptr;
            if (SphereGeom *sphereGeom = dynamic_cast<SphereGeom *>(iter.second.get()))
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
            if (PlaneGeom *planeGeom = dynamic_cast<PlaneGeom *>(iter.second.get()))
            {
                double a, b, c, d;
                planeGeom->GetPlane(&a, &b, &c, &d);
                geomID = dCreatePlane(m_spaceID, a, b, c, d);
                dGeomSetData(geomID, planeGeom);
                iter.second->setData(geomID);
                dGeomSetBody(geomID, nullptr); // planes have to be attached to the world
                break;
            }
            break;
        }
    }
    return nullptr;
}

std::string *ODEPhysicsEngine::MoveBodies()
{
    for (auto &&iter : *simulation()->GetBodyList())
    {
        dBodyID bodyID = reinterpret_cast<dBodyID>(iter.second->data());
        pgd::Vector3 position = iter.second->GetPosition();
        dBodySetPosition(bodyID, position.x, position.y, position.z);
        pgd::Quaternion quaternion = iter.second->GetQuaternion();
        dBodySetQuaternion(bodyID, quaternion.constData());
    }
    for (auto &&iter : *simulation()->GetJointList())
    {
        if (auto fixedJoint = dynamic_cast<FixedJoint *>(iter.second.get()))
        {
            if (fixedJoint->lateFix()) { dJointSetFixed(static_cast<dJointID>(iter.second->data())); }
        }
    }
    return nullptr;
}

std::string *ODEPhysicsEngine::Step()
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
            // if (auto muscle = dynamic_cast<MagicMuscle *>(iter.second.get()))
            // {
            //     std::ofstream log("c:/scratch/debug.log", std::ios_base::app);
            //     log << simulation()->GetTime() << "\t" << pf->body->name() << GSUtil::ToString(pf->point) << "\t" << GSUtil::ToString(pf->vector) << "\n";
            // }
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

    // update the user angles if necessary [FIX ME - this need checking]
    for (auto &&iter : m_amotorList)
    {
        dBodyID body1 = dJointGetBody(iter, 0);
        dBodyID body2 = dJointGetBody(iter, 1);
        double thetaX, thetaY, thetaZ;

        if (body1 == 0) // body 2 is connected to the world so it is already in the correct coodinates
        {
            const double *R2 = dBodyGetRotation(body2);
            dMatrix3 rotMat;
            for (int i = 0; i < dM3E__MAX; i++) rotMat[i] = R2[i];
            dGetEulerAngleFromRot(rotMat, thetaX, thetaY, thetaZ);
        }
        else
        {
            // find orientation of Body 2 wrt Body 1
            dMatrix3 rotMat;
            const double *R1 = dBodyGetRotation(body1);
            const double *R2 = dBodyGetRotation(body2);
            dMULTIPLY2_333(rotMat, R2, R1);

            // now find the X,Y,Z angles (use the Euler formulae for convenience not efficiency)
            dGetEulerAngleFromRot(rotMat, thetaX, thetaY, thetaZ);
        }

        dJointSetAMotorAngle(iter, 0, -thetaX);
        dJointSetAMotorAngle(iter, 1, -thetaY);
        dJointSetAMotorAngle(iter, 2, -thetaZ);

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
            if (HingeJoint *hingeJoint = dynamic_cast<HingeJoint *>(iter.second.get()))
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

    return nullptr;
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
    double rho = std::max(g1->GetContactRho(), g2->GetContactRho());
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
                myContact->setBody1(g1->GetBody());
                myContact->setBody2(g2->GetBody());
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


} // namespace GaitSym


