#include "ODEPhysicsEngine.h"

#include "Simulation.h"
#include "Body.h"
#include "Muscle.h"
#include "FluidSac.h"
#include "Geom.h"
#include "HingeJoint.h"
#include "SphereGeom.h"
#include "PlaneGeom.h"

#define MAX_CONTACTS 64

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

int ODEPhysicsEngine::Initialise(Simulation *simulation)
{
    int err = PhysicsEngine::Initialise(simulation);
    if (err) { return err; }

    // initialise the ODE world
    dInitODE();
    m_worldID = dWorldCreate();
    m_spaceID = dHashSpaceCreate(nullptr); // FIX ME hash space is a compromise but this should probably be user controlled
    m_contactGroup = dJointGroupCreate(0);

    // glue for calling a C++ callback
    // Store member function and the instance using std::bind.
    Callback<void(int, const char *, va_list)>::func = std::bind(&ErrorHandler::ODEMessageTrap, &m_errorHandler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    // Convert callback-function to c-pointer.
    void (*c_func)(int, const char *, va_list) = static_cast<decltype(c_func)>(Callback<void(int, const char *, va_list)>::callback);

    // c_func is now the required function pointer
    dSetMessageHandler(c_func);
    dSetErrorHandler(c_func);
    dSetDebugHandler(c_func);

    // first create the bodies
    pgd::Quaternion zeroRotation( 1, 0, 0, 0);
    for (auto &&iter :  *simulation->GetBodyList())
    {
        dBodyID bodyID = dBodyCreate(m_worldID);
        dBodySetData(bodyID, iter.second.get());
        iter.second->setData(bodyID);
        pgd::Vector3 constructionPosition = iter.second->GetConstructionPosition();
        dBodySetPosition(bodyID, constructionPosition.x, constructionPosition.y, constructionPosition.z);
        dBodySetQuaternion(bodyID, zeroRotation.constData());
        pgd::Vector3 linearVelocity = iter.second->GetLinearVelocity();
        dBodySetLinearVel(bodyID, linearVelocity.x, linearVelocity.y, linearVelocity.z);
        pgd::Vector3 angularVelocity = iter.second->GetAngularVelocity();
        dBodySetAngularVel(bodyID, angularVelocity.x, angularVelocity.y, angularVelocity.z);
    }

    // then the joints
    for (auto &&iter :  *simulation->GetJointList())
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
                dJointSetFeedback(jointID, jointFeedback.get());
                dJointSetHingeAnchor(jointID, anchor.x, anchor.y, anchor.z);
                dJointSetHingeAxis(jointID, axis.x, axis.y, axis.z);
                dJointSetHingeParam(jointID, dParamLoStop, stops[0]); // needs to be done twice to guarantee to see the stops properly
                dJointSetHingeParam(jointID, dParamHiStop, stops[1]);
                dJointSetHingeParam(jointID, dParamLoStop, stops[0]);
                dJointSetHingeParam(jointID, dParamHiStop, stops[1]);
                m_jointFeedback[iter.first] = std::move(jointFeedback);
                iter.second->setData(jointID);
                break;
            }
            break;
        }
    }

    // then the geoms
    for (auto &&iter :  *simulation->GetGeomList())
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
                dGeomSphereSetRadius(geomID, radius);
                dBodyID bodyID = reinterpret_cast<dBodyID>(sphereGeom->GetBody()->data());
                dGeomSetBody(geomID, bodyID);
                dGeomSetOffsetPosition(geomID, position.x, position.y, position.z);
                dGeomSetOffsetQuaternion(geomID, quaternion.constData());
                iter.second->setData(geomID);
                break;
            }
            PlaneGeom *planeGeom = dynamic_cast<PlaneGeom *>(iter.second.get());
            if (planeGeom)
            {
                double a, b, c, d;
                planeGeom->GetPlane(&a, &b, &c, &d);
                double length = std::sqrt(a * a + b * b + c * c);
                if (length < std::numeric_limits<double>::epsilon()) // standard fixup
                {
                    a = 0; b = 0; c = 1; d = 0;
                }
                else
                {
                    a = a / length; b = b / length; c = c / length;
                }
                geomID = dCreatePlane(m_spaceID, a, b, c, d);
                dGeomSetData(geomID, planeGeom);
                iter.second->setData(geomID);
                break;
            }
            break;
        }
    }

    // finally move the bodies to their starting positions
    for (auto &&iter :  *simulation->GetBodyList())
    {
        dBodyID bodyID = reinterpret_cast<dBodyID>(iter.second->data());
        pgd::Vector3 position = iter.second->GetPosition();
        dBodySetPosition(bodyID, position.x, position.y, position.z);
        pgd::Quaternion quaternion = iter.second->GetQuaternion();
        dBodySetQuaternion(bodyID, quaternion.constData());
    }

    return 0;
}

int ODEPhysicsEngine::Step()
{
    // check collisions first
    dJointGroupEmpty(m_contactGroup);
    m_contactList.clear();
    // for (auto &&geomIter : m_GeomList) geomIter.second->ClearContacts();
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

    // update the bodies (needed for drag calculations)
#ifdef FIX_ME
    for (auto &&bodyIter : *simulation()->GetBodyList()) { bodyIter.second->ComputeDrag(); }
#endif

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
    for (auto &&iter :  *simulation()->GetBodyList())
    {
        dBodyID bodyID = dBodyCreate(m_worldID);
        dBodySetData(bodyID, iter.second.get());
        iter.second->setData(bodyID);
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
#ifdef FIX_ME
                dJointSetFeedback(c, myContact->GetJointFeedback());
                myContact->SetJointID(c);
                std::copy_n(contact[i].geom.pos, dV3E__MAX, myContact->GetContactPosition());
#endif
                g1->AddContact(myContact.get());
                g2->AddContact(myContact.get());
                s->contactList()->push_back(std::move(myContact));
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

std::vector<std::unique_ptr<Contact>> *ODEPhysicsEngine::contactList()
{
    return &m_contactList;
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


