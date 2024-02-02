#include "ODEPhysicsEngine.h"

ODEPhysicsEngine::ODEPhysicsEngine()
{
}

ODEPhysicsEngine::~ODEPhysicsEngine()
{
    // destroy the ODE world
    dSetMessageHandler(nullptr);
    dSetErrorHandler(nullptr);
    dSetDebugHandler(nullptr);
    if (m_ContactGroup) dJointGroupDestroy(m_ContactGroup);
    if (m_SpaceID) dSpaceDestroy(m_SpaceID);
    if (m_WorldID) dWorldDestroy(m_WorldID);
    dCloseODE();
}

int ODEPhysicsEngine::Initialise(Simulation *simulation)
{
    PhysicsEngine::Initialise(simulation);

    // initialise the ODE world
    dInitODE();
    m_WorldID = dWorldCreate();
    m_SpaceID = dHashSpaceCreate(nullptr); // FIX ME hash space is a compromise but this should probably be user controlled
    m_ContactGroup = dJointGroupCreate(0);

    // glue for calling a C++ callback
    // Store member function and the instance using std::bind.
    Callback<void(int, const char *, va_list)>::func = std::bind(&ErrorHandler::ODEMessageTrap, &m_errorHandler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    // Convert callback-function to c-pointer.
    void (*c_func)(int, const char *, va_list) = static_cast<decltype(c_func)>(Callback<void(int, const char *, va_list)>::callback);

    // c_func is now the required function pointer
    dSetMessageHandler(c_func);
    dSetErrorHandler(c_func);
    dSetDebugHandler(c_func);

    return 0;
}

int ODEPhysicsEngine::Step()
{
    // check collisions first
    dJointGroupEmpty(m_ContactGroup);
    m_ContactList.clear();
    // for (auto &&geomIter : m_GeomList) geomIter.second->ClearContacts();
    dSpaceCollide(m_SpaceID, this, &NearCallback);
/*
    // update the drivers
    for (auto &&it : m_DriverList)
    {
        it.second->Update();
        it.second->SendData();
    }
    // and the controllers (which are drivers too probably)
    for (auto &&it : m_ControllerList)
    {
        auto driver = dynamic_cast<Driver *>(it.second.get());
        if (driver)
        {
            driver->Update();
            driver->SendData();
        }
        if (it.second->lastStepCount() != m_StepCount)
            std::cerr << "Warning: " << it.first << " controller not updated\n"; // currently cannot stack controllers although this is fixable
    }

    // update the muscles
    for (auto iter1 = m_MuscleList.begin(); iter1 != m_MuscleList.end(); /* no increment */)
    {
        iter1->second->CalculateStrap();
        iter1->second->SetActivation();

        // check for breaking strain
        DampedSpringMuscle *dampedSpringMuscle = dynamic_cast<DampedSpringMuscle *>(iter1->second.get());
        if (dampedSpringMuscle)
        {
            if (dampedSpringMuscle->ShouldBreak())
            {
                iter1 = m_MuscleList.erase(iter1); // erase returns the next iterator [but m_MuscleList.erase(iter1++) would also work and is compatible with older C++ compilers]
                continue;
            }
        }

        std::vector<std::unique_ptr<PointForce>> *pointForceList = iter1->second->GetPointForceList();
        double tension = iter1->second->GetTension();
#ifdef DEBUG_CHECK_FORCES
        pgd::Vector3 force(0, 0, 0);
#endif
        for (unsigned int i = 0; i < pointForceList->size(); i++)
        {
            PointForce *pointForce = (*pointForceList)[i].get();
            if (pointForce->body)
                dBodyAddForceAtPos(pointForce->body->GetBodyID(),
                                   pointForce->vector[0] * tension, pointForce->vector[1] * tension, pointForce->vector[2] * tension,
                                   pointForce->point[0], pointForce->point[1], pointForce->point[2]);
#ifdef DEBUG_CHECK_FORCES
            force += pgd::Vector3(pointForce->vector[0] * tension, pointForce->vector[1] * tension, pointForce->vector[2] * tension);
#endif
        }
#ifdef DEBUG_CHECK_FORCES
        std::cerr.setf(std::ios::floatfield, std::ios::fixed);
        std::cerr << iter1->first << " " << force.x << " " << force.y << " " << force.z << "\n";
        std::cerr.unsetf(std::ios::floatfield);
#endif
        iter1++; // this has to be done outside the for definition because erase returns the next iterator
    }

    // update the joints (needed for motors, end stops and stress calculations)
    for (auto &&jointIter : m_JointList) jointIter.second->Update();

    // update the fluid sacs
    for (auto fsIter = m_FluidSacList.begin(); fsIter != m_FluidSacList.end(); fsIter++)
    {
        fsIter->second->calculateVolume();
        fsIter->second->calculatePressure();
        fsIter->second->calculateLoadsOnMarkers();
        for (size_t i = 0; i < fsIter->second->pointForceList().size(); i++)
        {
            const PointForce *pf = &fsIter->second->pointForceList().at(i);
            dBodyAddForceAtPos(pf->body->GetBodyID(), pf->vector[0], pf->vector[1], pf->vector[2], pf->point[0], pf->point[1], pf->point[2]);
        }
    }

    // update the bodies (needed for drag calculations)
    for (auto &&bodyIter : m_BodyList) bodyIter.second->ComputeDrag();

    // run the simulation
    switch (m_global->stepType())
    {
    case Global::World:
        dWorldStep(m_WorldID, m_global->StepSize());
        break;

    case Global::Quick:
        dWorldQuickStep(m_WorldID, m_global->StepSize());
        break;
    }

    // test for penalties
    if (m_errorHandler.IsMessage()) m_KinematicMatchFitness += m_global->NumericalErrorsScore();

    // calculate the energies
    for (auto &&iter1 : m_MuscleList)
    {
        m_MechanicalEnergy += iter1.second->GetPower() * m_global->StepSize();
        m_MetabolicEnergy += iter1.second->GetMetabolicPower() * m_global->StepSize();
    }
    m_MetabolicEnergy += m_global->BMR() * m_global->StepSize();

    // update any contact force dependent drivers (because only after the simulation is the force valid
    // update the footprint indicator
    if (m_ContactList.size() > 0)
    {
        for (auto &&it : m_DriverList)
        {
            TegotaeDriver *tegotaeDriver = dynamic_cast<TegotaeDriver *>(it.second.get());
            if (tegotaeDriver) tegotaeDriver->UpdateReactionForce();
        }
    }

    // all reporting is done after a simulation step

    DumpObjects();

    // update the time counter
    m_SimulationTime += m_global->StepSize();

    // update the step counter
    m_StepCount++;
    return 0;
}

// this is called by dSpaceCollide when two objects in space are
// potentially colliding.

void ODEPhysicsEngine::NearCallback(void *data, dGeomID o1, dGeomID o2)
{
    Simulation *s = reinterpret_cast<Simulation *>(data);
    Geom *g1 = reinterpret_cast<Geom *>(dGeomGetData(o1));
    Geom *g2 = reinterpret_cast<Geom *>(dGeomGetData(o2));

    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    if (b1 == b2)
    {
        return; // it is never useful for two contacts on the same body to collide [I'm not sure if this every happens - FIX ME - set up a test]
    }

    if (s->m_global->AllowConnectedCollisions() == false)
    {
        if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact)) return;
    }

    if (s->m_global->AllowInternalCollisions() == false)
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

    std::vector<dContact> contact(size_t(s->m_MaxContacts), dContact{}); // in this case default initialisation is potentially useful
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
    for (size_t i = 0; i < size_t(s->m_MaxContacts); i++)
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
    int numc = dCollide(o1, o2, s->m_MaxContacts, &contact[0].geom, sizeof(dContact));
    if (numc)
    {
        for (size_t i = 0; i < size_t(numc); i++)
        {
            if (g1->GetAbort()) s->SetContactAbort(g1->name());
            if (g2->GetAbort()) s->SetContactAbort(g2->name());
            dJointID c;
            if (g1->GetAdhesion() == false && g2->GetAdhesion() == false)
            {
                c = dJointCreateContact(s->m_WorldID, s->m_ContactGroup, &contact[i]);
                dJointAttach(c, b1, b2);
                std::unique_ptr<Contact> myContact = std::make_unique<Contact>();
                myContact->setSimulation(s);
                dJointSetFeedback(c, myContact->GetJointFeedback());
                myContact->SetJointID(c);
                std::copy_n(contact[i].geom.pos, dV3E__MAX, myContact->GetContactPosition());
                //                // only add the contact information once
                //                // and add it to the non-environment geom
                //                if (g1->GetGeomLocation() == Geom::environment)
                //                    g2->AddContact(myContact.get());
                //                else
                //                    g1->AddContact(myContact.get());
                // add the contact information to both geoms
                g1->AddContact(myContact.get());
                g2->AddContact(myContact.get());
                s->m_ContactList.push_back(std::move(myContact));
            }
            else
            {
                // FIX ME adhesive joints are added permanently and forces cannot be measured
                c = dJointCreateBall(s->m_WorldID, nullptr);
                dJointAttach(c, b1, b2);
                dJointSetBallAnchor(c, contact[i].geom.pos[0], contact[i].geom.pos[1], contact[i].geom.pos[2]);
            }
        }
    }
}


