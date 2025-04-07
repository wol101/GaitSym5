/*
 *  Simulation.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 24/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

// Simulation.cpp - this simulation object is used to encapsulate
// a ODE simulation

#include "Simulation.h"

#include "CyclicDriver.h"
#include "StepDriver.h"
#include "MarkerPositionDriver.h"
#include "DataTarget.h"
#include "DataTargetScalar.h"
#include "DataTargetQuaternion.h"
#include "DataTargetVector.h"
#include "DataTargetMarkerCompare.h"
#include "DataFile.h"
#include "Body.h"
#include "HingeJoint.h"
#include "BallJoint.h"
#include "FloatingHingeJoint.h"
#include "CappedCylinderGeom.h"
#include "SphereGeom.h"
#include "ConvexGeom.h"
#include "TrimeshGeom.h"
#include "Muscle.h"
#include "MAMuscle.h"
#include "MAMuscleComplete.h"
#include "DampedSpringMuscle.h"
#include "TwoPointStrap.h"
#include "CylinderWrapStrap.h"
#include "TwoCylinderWrapStrap.h"
#include "FluidSacIdealGas.h"
#include "FluidSacIncompressible.h"
#include "PlaneGeom.h"
#include "NPointStrap.h"
#include "FixedJoint.h"
#include "Marker.h"
#include "Reporter.h"
#include "MarkerReporter.h"
#include "UniversalJoint.h"
#include "PIDMuscleLengthController.h"
#include "Controller.h"
#include "AMotorJoint.h"
#include "LMotorJoint.h"
#include "BoxGeom.h"
#include "StackedBoxCarDriver.h"
#include "FixedDriver.h"
#include "PIDErrorInController.h"
#include "TegotaeDriver.h"
#include "ThreeHingeJointDriver.h"
#include "TwoHingeJointDriver.h"
#include "MarkerEllipseDriver.h"
#include "PhysicsEngine.h"
#include "ODEPhysicsEngine.h"
#include "PhysXPhysicsEngine.h"
#include "MuJoCoPhysicsEngine.h"
#include "PlaybackPhysicsEngine.h"
#include "LightBase.h"
#include "MagicMuscle.h"
#include "MagicStrap.h"

#include "pystring.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <list>
#include <algorithm>

using namespace std::string_literals;
namespace GaitSym {

Simulation::Simulation()
{
}

//----------------------------------------------------------------------------
Simulation::~Simulation()
{
}

//----------------------------------------------------------------------------
std::string *Simulation::LoadModel(const char *buffer, size_t length) // note this requires buffer to be a 0 terminated string of size length + 1
{
    std::string rootTag = "GAITSYM5"s;
    std::string *ptr = m_parseXML.LoadModel(buffer, length, &rootTag);
    if (ptr) return ptr;

    // this logic allows forward references at the expense of slightly less obvious error messages
    std::list<ParseXML::XMLElement *> unprocessedList;
    for (auto &&it : *m_parseXML.elementList()) unprocessedList.push_back(it.get());
    size_t lastSize = 0;
    size_t cycles = 0;
    std::vector<std::string> errorList;
    while (unprocessedList.size() > 0 && unprocessedList.size() != lastSize)
    {
        cycles++;
        lastSize = unprocessedList.size();
        errorList.clear();
        for (auto it = unprocessedList.begin(); it != unprocessedList.end();)
        {
            lastErrorPtr()->clear();
            if ((*it)->tag == "GLOBAL"s) ParseGlobal(*it);
            else if ((*it)->tag == "BODY"s) ParseBody(*it);
            else if ((*it)->tag == "JOINT"s) ParseJoint(*it);
            else if ((*it)->tag == "GEOM"s) ParseGeom(*it);
            else if ((*it)->tag == "STRAP"s) ParseStrap(*it);
            else if ((*it)->tag == "MUSCLE"s) ParseMuscle(*it);
            else if ((*it)->tag == "DRIVER"s) ParseDriver(*it);
            else if ((*it)->tag == "DATATARGET"s) ParseDataTarget(*it);
            else if ((*it)->tag == "MARKER"s) ParseMarker(*it);
            else if ((*it)->tag == "REPORTER"s) ParseReporter(*it);
            else if ((*it)->tag == "CONTROLLER"s) ParseController(*it);
            else if ((*it)->tag == "FLUIDSAC"s) ParseFluidSac(*it);
            else if ((*it)->tag == "LIGHT"s) ParseLight(*it);
            if (lastErrorPtr()->size())
            {
                errorList.push_back(*lastErrorPtr());
                it++;
            }
            else
            {
                it = unprocessedList.erase(it);
            }
        }
    }
    if (lastErrorPtr()->size())
    {
        setLastError(pystring::join("\n"s, errorList));
        return lastErrorPtr();
    }
    if (cycles > 1)
        std::cerr << "Warning: file took " << cycles << " cycles to parse. Consider reordering for speed.\n";

    // joints are created with the bodies in construction poses
    // then the bodies are moved to their starting poses
    for (auto &&it : m_BodyList) it.second->LateInitialisation();
    // and we recalculate the dynamic items with the new muscle positions
    for (auto &&it :  m_MuscleList) it.second->LateInitialisation();
    for (auto &&it : m_FluidSacList) it.second->LateInitialisation();
    // and some joints require things to be done after the bodies are moved to their start positions
    for (auto &&it :  m_JointList) it.second->LateInitialisation();

    // and we need to set the cycle time
    // currently just using the maximum value but some sort of fuzzy lowest common multiple might be better
    // the easiest way to do that is to mutiply by an appropriate power of 10 with nearest number rounding (int(v * 10000 + 0.5)) to make the numbers into integers and then use an integer formula and convert back
    // using std::lcm from numeric with accumulate so it works on a container (a std::set makes sense for longer lists perhas)
    // std::vector<int> v{4, 6, 10};
    // auto lcm = std::accumulate(v.begin(), v.end(), 1, [](auto & a, auto & b) { return std::lcm(a, b); });
    m_CycleTime = 0;
    for (auto &&driver : m_DriverList)
    {
        if (CyclicDriver *cyclicDriver = dynamic_cast<CyclicDriver*>(driver.second.get())) m_CycleTime = std::max(cyclicDriver->GetCycleTime(), m_CycleTime);
        else if (StackedBoxcarDriver *stackedBoxcarDriver = dynamic_cast<StackedBoxcarDriver*>(driver.second.get())) m_CycleTime = std::max(stackedBoxcarDriver->GetCycleTime(), m_CycleTime);
    }
    return nullptr;
}


//----------------------------------------------------------------------------
void Simulation::UpdateSimulation()
{
    std::string *err = nullptr;
    if (!m_physicsEngine)
    {
        switch (m_global->physicsEngine())
        {
        case Global::PhysicsEngine::ODE:
            {
                m_physicsEngine = std::make_unique<ODEPhysicsEngine>();
                err = m_physicsEngine->Initialise(this);
                if (err)
                {
                    std::cerr << "Error: unable to initialise ODEPhysicsEngine\n" << *err << "\n";
                    m_SimulationError = true;
                    return;
                }
                break;
            }
        case Global::PhysicsEngine::PhysX:
            {
                m_physicsEngine = std::make_unique<PhysXPhysicsEngine>();
                err = m_physicsEngine->Initialise(this);
                if (err)
                {
                    std::cerr << "Error: unable to initialise PhysXPhysicsEngine\n" << *err << "\n";
                    m_SimulationError = true;
                    return;
                }
                break;
            }
        case Global::PhysicsEngine::MuJoCo:
            {
                m_physicsEngine = std::make_unique<MuJoCoPhysicsEngine>();
                err = m_physicsEngine->Initialise(this);
                if (err)
                {
                    std::cerr << "Error: unable to initialise MuJoCoPhysicsEngine\n" << *err << "\n";
                    m_SimulationError = true;
                    return;
                }
                break;
            }
        }
    }

    // start by updating the scores
    double minScore = std::numeric_limits<double>::infinity();
    double maxScore = -std::numeric_limits<double>::infinity();
    for (auto &&it : m_DataTargetList)
    {
        double matchScore;
        bool matchScoreValid = it.second->calculateMatchValue(m_SimulationTime, &matchScore);
        if (matchScoreValid)
        {
            m_TargetMatchFitness += matchScore;
            if (matchScore < minScore) minScore = matchScore;
            if (matchScore > maxScore) maxScore = matchScore;
        }
    }
    if (minScore < std::numeric_limits<double>::infinity()) m_TargetMatchMaxiMinFitness += minScore;
    if (maxScore > -std::numeric_limits<double>::infinity()) m_TargetMatchMiniMaxFitness += maxScore;

    // now start the actual simulation

    // clear the contacts from the geoms and in the master list
    for (auto &&geomIter : m_GeomList) { geomIter.second->ClearContacts(); }
    m_ContactList.clear();

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
    for (auto iter1 = m_MuscleList.begin(); iter1 != m_MuscleList.end(); /* no increment */ )
    {
        // muscle straps are valid at this point so they do not need recaulculating
        iter1->second->SetActivation();

        // check for breaking strain
        if (DampedSpringMuscle *dampedSpringMuscle = dynamic_cast<DampedSpringMuscle *>(iter1->second.get()))
        {
            if (dampedSpringMuscle->ShouldBreak())
            {
                iter1 = m_MuscleList.erase(iter1); // erase returns the next iterator [but m_MuscleList.erase(iter1++) would also work and is compatible with older C++ compilers]
                continue;
            }
        }
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
    }

    // update the bodies (needed for drag calculations)
    for (auto &&bodyIter : m_BodyList) { bodyIter.second->ComputeDrag(); }

    // output the model state if triggered
    if (m_OutputModelStateAtTime >= 0.0)
    {
        if (m_SimulationTime >= m_OutputModelStateAtTime)
        {
            OutputProgramState();
            m_OutputModelStateAtTime = -1;
        }
    }
    else if (m_OutputModelStateAtCycle >= 0 && m_CycleTime >= 0 && m_SimulationTime >= m_CycleTime * m_OutputModelStateAtCycle)
    {
        OutputProgramState();
        m_OutputModelStateAtCycle = -1;
    }

    // run the simulation
    m_physicsEngine->Step();

    // now the muscle straps are invalid because the bodies have moved so they need recalculating
    for (auto &&iter1 : m_MuscleList)
    {
        iter1.second->CalculateStrap();
    }

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
            if (TegotaeDriver *tegotaeDriver = dynamic_cast<TegotaeDriver *>(it.second.get())) tegotaeDriver->UpdateReactionForce();
        }
    }

    // all reporting is done after a simulation step

    DumpObjects();

    // update the time counter
    m_SimulationTime += m_global->StepSize();

    // update the step counter
    m_StepCount++;
}

//----------------------------------------------------------------------------
bool Simulation::TestForCatastrophy()
{
    // check for simulation error
    if (m_SimulationError)
    {
        std::cerr << "Failed due to simulation error " << m_SimulationError << "\n";
        return true;
    }

    // check for contact abort
    if (m_ContactAbort)
    {
        std::cerr << "Failed due to contact abort\n";
        for (auto &&it: m_ContactAbortList) { std::cerr << it << "\n"; }
        return true;
    }

    // check for data target abort
    if (m_DataTargetAbort)
    {
        std::cerr << "Failed due to DataTarget abort\n";
        for (auto &&it: m_DataTargetAbortList) { std::cerr << it << "\n"; }
        return true;
    }

    // check that all bodies meet velocity and stop conditions

    Body::LimitTestResult p;
    for (auto &&iter1 : m_BodyList)
    {
        p = iter1.second->TestLimits();
        switch (p)
        {
        case Body::WithinLimits:
            break;

        case Body::XPosError:
        case Body::YPosError:
        case Body::ZPosError:
            std::cerr << "Failed due to position error " << Body::limitTestResultStrings(p) << " in: " << iter1.second->name() << "\n";
            return true;

        case Body::XVelError:
        case Body::YVelError:
        case Body::ZVelError:
            std::cerr << "Failed due to linear velocity error " << Body::limitTestResultStrings(p) << " in: " << iter1.second->name() << "\n";
            return true;

        case Body::XAVelError:
        case Body::YAVelError:
        case Body::ZAVelError:
            std::cerr << "Failed due to angular velocity error " << Body::limitTestResultStrings(p) << " in: " << iter1.second->name() << "\n";
            return true;

        case Body::NumericalError:
            std::cerr << "Failed due to numerical error " << Body::limitTestResultStrings(p) << " in: " << iter1.second->name() << "\n";
            return true;
        }
    }

    for (auto &&iter3 : m_JointList)
    {

        if (auto j = dynamic_cast<HingeJoint *>(iter3.second.get()))
        {
            int t = j->TestLimits();
            if (t < 0)
            {
                std::cerr << "Failed due to LoStopTorqueLimit error in: " << iter3.second->name() << "\n";
                return true;
            }
            else if (t > 0)
            {
                std::cerr << "Failed due to HiStopTorqueLimit error in: " << iter3.second->name() << "\n";
                return true;
            }
        }

        if (auto f = dynamic_cast<FixedJoint *>(iter3.second.get()))
        {
            if (f->CheckStressAbort())
            {
                std::cerr << "Failed due to stress limit error in: " << iter3.second->name() << " " << f->GetLowPassMinStress() << " " << f->GetLowPassMaxStress() << "\n";
                return true;
            }
        }
    }

    // and test the reporters for stop conditions
    for (auto &&reporterIter : m_ReporterList)
    {
        if (reporterIter.second->ShouldAbort())
        {
            std::cerr << "Failed due to Reporter Abort in: " << reporterIter.second->name() << "\n";
            return true;
        }
    }

    if (m_OutputModelStateOccured && m_AbortAfterModelStateOutput)
    {
        std::cerr << "Abort because ModelState successfully written\n";
        return true;
    }

    return false;
}


//----------------------------------------------------------------------------
double Simulation::CalculateInstantaneousFitness()
{
    switch (m_global->fitnessType())
    {
    case Global::TargetSum:
        return m_TargetMatchFitness;

    case Global::TargetMiniMax:
        return m_TargetMatchMiniMaxFitness;

    case Global::TargetMaxiMin:
        return m_TargetMatchMaxiMinFitness;
    }
    return 0;
}

std::string *Simulation::ParseGlobal(const ParseXML::XMLElement *node)
{
    std::unique_ptr<Global> global = std::make_unique<Global>();
    global->setSimulation(this);
    global->createAttributeMap(node->attributes);
    std::string *errorMessage = global->createFromAttributes();
    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }
    this->SetGlobal(std::move(global));
    return nullptr;
}

std::string *Simulation::ParseBody(const ParseXML::XMLElement *node)
{
    std::unique_ptr<Body> body = std::make_unique<Body>();
    body->setSimulation(this);
    body->createAttributeMap(node->attributes);
    std::string *errorMessage = body->createFromAttributes();
    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }
    m_BodyList[body->name()] = std::move(body);
    return nullptr;
}

std::string *Simulation::ParseMarker(const ParseXML::XMLElement *node)
{
    std::unique_ptr<Marker> marker = std::make_unique<Marker>(nullptr);
    marker->setSimulation(this);
    marker->createAttributeMap(node->attributes);
    std::string *errorMessage = marker->createFromAttributes();
    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }
    m_MarkerList[marker->name()] = std::move(marker);
    return nullptr;
}

std::string *Simulation::ParseJoint(const ParseXML::XMLElement *node)
{
    std::unique_ptr<Joint> joint;
    std::string buf = NamedObject::searchNames(node->attributes, "Type"s);
    std::string *errorMessage = nullptr;
    if (buf == "Hinge"s)
    {
        joint = std::make_unique<HingeJoint>();
        joint->setSimulation(this);
        joint->createAttributeMap(node->attributes);
        errorMessage = joint->createFromAttributes();
    }
    else if (buf == "Ball"s)
    {
        joint = std::make_unique<BallJoint>();
        joint->setSimulation(this);
        joint->createAttributeMap(node->attributes);
        errorMessage = joint->createFromAttributes();
    }
    else if (buf == "Fixed"s)
    {
        joint = std::make_unique<FixedJoint>();
        joint->setSimulation(this);
        joint->createAttributeMap(node->attributes);
        errorMessage = joint->createFromAttributes();
    }
    else if (buf == "FloatingHinge"s)
    {
        joint = std::make_unique<FloatingHingeJoint>();
        joint->setSimulation(this);
        joint->createAttributeMap(node->attributes);
        errorMessage = joint->createFromAttributes();
    }
    else if (buf == "Universal"s)
    {
        joint = std::make_unique<UniversalJoint>();
        joint->setSimulation(this);
        joint->createAttributeMap(node->attributes);
        errorMessage = joint->createFromAttributes();
    }
    else if (buf == "AMotor"s)
    {
        joint = std::make_unique<AMotorJoint>();
        joint->setSimulation(this);
        joint->createAttributeMap(node->attributes);
        errorMessage = joint->createFromAttributes();
    }
    else if (buf == "LMotor"s)
    {
        joint = std::make_unique<LMotorJoint>();
        joint->setSimulation(this);
        joint->createAttributeMap(node->attributes);
        errorMessage = joint->createFromAttributes();
    }
    else
    {
        setLastError("Simulation::ParseJoint Type=\"" + buf + "\" not recognised");
        return lastErrorPtr();
    }

    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }
    m_JointList[joint->name()] = std::move(joint);
    return nullptr;
}

std::string *Simulation::ParseGeom(const ParseXML::XMLElement *node)
{
    // GEOMs require a valid GLOBAL to get step size
    if (!m_global)
    {
        setLastError("Simulation::ParseGeom requires a valid GLOBAL"s);
        return lastErrorPtr();
    }
    std::unique_ptr<Geom> geom;
    std::string buf = NamedObject::searchNames(node->attributes, "Type"s);
    std::string *errorMessage = nullptr;
    if (buf == "Box"s)
    {
        std::unique_ptr<BoxGeom> boxGeom = std::make_unique<BoxGeom>(1.0, 1.0, 1.0);
        boxGeom->setSimulation(this);
        boxGeom->createAttributeMap(node->attributes);
        errorMessage = boxGeom->createFromAttributes();
        geom = std::move(boxGeom);
    }
    else if (buf == "CappedCylinder"s)
    {
        std::unique_ptr<CappedCylinderGeom> cappedCylinderGeom = std::make_unique<CappedCylinderGeom>(1.0, 1.0);
        cappedCylinderGeom->setSimulation(this);
        cappedCylinderGeom->createAttributeMap(node->attributes);
        errorMessage = cappedCylinderGeom->createFromAttributes();
        geom = std::move(cappedCylinderGeom);
    }
    else if (buf == "Plane"s)
    {
        std::unique_ptr<PlaneGeom> planeGeom = std::make_unique<PlaneGeom>(10.0, 0.0, 1.0, 0.0);
        planeGeom->setSimulation(this);
        planeGeom->createAttributeMap(node->attributes);
        errorMessage = planeGeom->createFromAttributes();
        geom = std::move(planeGeom);
    }
    else if (buf == "Sphere"s)
    {
        std::unique_ptr<SphereGeom> sphereGeom = std::make_unique<SphereGeom>(1.0);
        sphereGeom->setSimulation(this);
        sphereGeom->createAttributeMap(node->attributes);
        errorMessage = sphereGeom->createFromAttributes();
        geom = std::move(sphereGeom);
    }
    else if (buf == "Convex"s)
    {
        std::unique_ptr<ConvexGeom> convexGeom = std::make_unique<ConvexGeom>();
        convexGeom->setSimulation(this);
        convexGeom->createAttributeMap(node->attributes);
        errorMessage = convexGeom->createFromAttributes();
        geom = std::move(convexGeom);
    }
    else if (buf == "Trimesh"s)
    {
        std::unique_ptr<TrimeshGeom> trimeshGeom = std::make_unique<TrimeshGeom>();
        trimeshGeom->setSimulation(this);
        trimeshGeom->createAttributeMap(node->attributes);
        errorMessage = trimeshGeom->createFromAttributes();
        geom = std::move(trimeshGeom);
    }
    else
    {
        setLastError("Simulation::ParseGeom Type=\""s + buf + "\" not recognised"s);
        return lastErrorPtr();
    }

    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }

    m_GeomList[geom->name()] = std::move(geom);
    return nullptr;
}

std::string *Simulation::ParseMuscle(const ParseXML::XMLElement *node)
{
    std::unique_ptr<Muscle> muscle;
    std::string buf = NamedObject::searchNames(node->attributes, "Type"s);
    std::string *errorMessage = nullptr;
    if (buf == "MinettiAlexander"s)
    {
        muscle = std::make_unique<MAMuscle>();
        muscle->setSimulation(this);
        muscle->createAttributeMap(node->attributes);
        errorMessage = muscle->createFromAttributes();
    }
    else if (buf == "MinettiAlexanderComplete"s)
    {
        muscle = std::make_unique<MAMuscleComplete>();
        muscle->setSimulation(this);
        muscle->createAttributeMap(node->attributes);
        errorMessage = muscle->createFromAttributes();
    }
    else if (buf == "DampedSpring"s)
    {
        muscle = std::make_unique<DampedSpringMuscle>();
        muscle->setSimulation(this);
        muscle->createAttributeMap(node->attributes);
        errorMessage = muscle->createFromAttributes();
    }
    else if (buf == "Magic"s)
    {
        muscle = std::make_unique<MagicMuscle>();
        muscle->setSimulation(this);
        muscle->createAttributeMap(node->attributes);
        errorMessage = muscle->createFromAttributes();
    }
    else
    {
        setLastError("Simulation::ParseMuscle Type=\""s + buf + "\" not recognised"s);
        return lastErrorPtr();
    }

    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }

    m_MuscleList[muscle->name()] = std::move(muscle);
    return nullptr;
}

std::string *Simulation::ParseStrap(const ParseXML::XMLElement *node)
{
    std::unique_ptr<Strap> strap;
    std::string buf = NamedObject::searchNames(node->attributes, "Type"s);
    std::string *errorMessage = nullptr;
    if (buf == "TwoPoint"s)
    {
        strap = std::make_unique<TwoPointStrap>();
        strap->setSimulation(this);
        strap->createAttributeMap(node->attributes);
        errorMessage = strap->createFromAttributes();
    }
    else if (buf == "NPoint"s)
    {
        strap = std::make_unique<NPointStrap>();
        strap->setSimulation(this);
        strap->createAttributeMap(node->attributes);
        errorMessage = strap->createFromAttributes();
    }
    else if (buf == "CylinderWrap"s)
    {
        strap = std::make_unique<CylinderWrapStrap>();
        strap->setSimulation(this);
        strap->createAttributeMap(node->attributes);
        errorMessage = strap->createFromAttributes();
    }
    else if (buf == "TwoCylinderWrap"s)
    {
        strap = std::make_unique<TwoCylinderWrapStrap>();
        strap->setSimulation(this);
        strap->createAttributeMap(node->attributes);
        errorMessage = strap->createFromAttributes();
    }
    else if (buf == "Magic"s)
    {
        strap = std::make_unique<MagicStrap>();
        strap->setSimulation(this);
        strap->createAttributeMap(node->attributes);
        errorMessage = strap->createFromAttributes();
    }
    else
    {
        setLastError("Simulation::ParseStrap Type=\""s + buf + "\" not recognised"s);
        return lastErrorPtr();
    }

    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }

    m_StrapList[strap->name()] = std::move(strap);
    return nullptr;
}

std::string *Simulation::ParseFluidSac(const ParseXML::XMLElement *node)
{
    std::unique_ptr<FluidSac> fluidSac;
    std::string buf = NamedObject::searchNames(node->attributes, "Type"s);
    std::string *errorMessage = nullptr;
    if (buf == "IdealGas"s)
    {
        fluidSac = std::make_unique<FluidSacIdealGas>();
        fluidSac->setSimulation(this);
        fluidSac->createAttributeMap(node->attributes);
        errorMessage = fluidSac->createFromAttributes();
    }
    else if (buf == "Incompressible"s)
    {
        fluidSac = std::make_unique<FluidSacIncompressible>();
        fluidSac->setSimulation(this);
        fluidSac->createAttributeMap(node->attributes);
        errorMessage = fluidSac->createFromAttributes();
    }
    else
    {
        setLastError("Simulation::ParseFluidSac Type=\""s + buf + "\" not recognised"s);
        return lastErrorPtr();
    }

    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }

    m_FluidSacList[fluidSac->name()] = std::move(fluidSac);
    return nullptr;
}

std::string *Simulation::ParseDriver(const ParseXML::XMLElement *node)
{
    std::unique_ptr<Driver> driver;
    std::string buf = NamedObject::searchNames(node->attributes, "Type"s);
    std::string *errorMessage = nullptr;
    if (buf == "Cyclic"s)
    {
        driver = std::make_unique<CyclicDriver>();
    }
    else if (buf == "Fixed"s)
    {
        driver = std::make_unique<FixedDriver>();
    }
    else if (buf == "StackedBoxcar"s)
    {
        driver = std::make_unique<StackedBoxcarDriver>();
    }
    else if (buf == "Step"s)
    {
        driver = std::make_unique<StepDriver>();
    }
    else if (buf == "Tegotae"s)
    {
        driver = std::make_unique<TegotaeDriver>();
    }
    else if (buf == "ThreeHingeJoint"s)
    {
        driver = std::make_unique<ThreeHingeJointDriver>();
    }
    else if (buf == "TwoHingeJoint"s)
    {
        driver = std::make_unique<TwoHingeJointDriver>();
    }
    else if (buf == "MarkerPosition"s)
    {
        driver = std::make_unique<MarkerPositionDriver>();
    }
    else if (buf == "MarkerEllipse"s)
    {
        driver = std::make_unique<MarkerEllipseDriver>();
    }
    else
    {
        setLastError("Simulation::ParseDriver Type=\""s + buf + "\" not recognised"s);
        return lastErrorPtr();
    }

    driver->setSimulation(this);
    driver->createAttributeMap(node->attributes);
    errorMessage = driver->createFromAttributes();
    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }

    m_DriverList[driver->name()] = std::move(driver);
    return nullptr;
}


std::string *Simulation::ParseDataTarget(const ParseXML::XMLElement *node)
{
    std::unique_ptr<DataTarget> dataTarget;
    std::string buf = NamedObject::searchNames(node->attributes, "Type"s);
    std::string *errorMessage = nullptr;
    if (buf == "Scalar"s)
    {
        dataTarget = std::make_unique<DataTargetScalar>();
    }
    else if (buf == "Quaternion"s)
    {
        dataTarget = std::make_unique<DataTargetQuaternion>();
    }
    else if (buf == "Vector"s)
    {
        dataTarget = std::make_unique<DataTargetVector>();
    }
    else if (buf == "MarkerCompare"s)
    {
        dataTarget = std::make_unique<DataTargetMarkerCompare>();
    }
    else
    {
        setLastError("Simulation::ParseDataTarget Type=\""s + buf + "\" not recognised"s);
        return lastErrorPtr();
    }

    dataTarget->setSimulation(this);
    dataTarget->createAttributeMap(node->attributes);
    errorMessage = dataTarget->createFromAttributes();
    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }

    m_DataTargetList[dataTarget->name()] = std::move(dataTarget);
    return nullptr;

}

std::string *Simulation::ParseReporter(const ParseXML::XMLElement *node)
{
    std::unique_ptr<Reporter> reporter;
    std::string buf = NamedObject::searchNames(node->attributes, "Type"s);
    std::string *errorMessage = nullptr;
    if (buf == "MarkerReporter"s)
    {
        reporter = std::make_unique<MarkerReporter>();
    }
    else
    {
        setLastError("Simulation::ParseReporter Type=\""s + buf + "\" not recognised"s);
        return lastErrorPtr();
    }

    reporter->setSimulation(this);
    reporter->createAttributeMap(node->attributes);
    errorMessage = reporter->createFromAttributes();
    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }

    m_ReporterList[reporter->name()] = std::move(reporter);
    return nullptr;
}

std::string *Simulation::ParseController(const ParseXML::XMLElement *node)
{
    std::unique_ptr<Controller> controller;
    std::string buf = NamedObject::searchNames(node->attributes, "Type"s);
    std::string *errorMessage = nullptr;
    if (buf == "PIDMuscleLength"s)
    {
        controller = std::make_unique<PIDMuscleLengthController>();
    }
    else if (buf == "PIDErrorIn"s)
    {
        controller = std::make_unique<PIDErrorInController>();
    }
    else
    {
        setLastError("Simulation::ParseController Type=\""s + buf + "\" not recognised"s);
        return lastErrorPtr();
    }

    controller->setSimulation(this);
    controller->createAttributeMap(node->attributes);
    errorMessage = controller->createFromAttributes();
    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }

    m_ControllerList[controller->name()] = std::move(controller);
    return nullptr;
}

std::string *Simulation::ParseLight(const ParseXML::XMLElement *node)
{
    std::unique_ptr<Light> light;
    std::string buf = NamedObject::searchNames(node->attributes, "Type"s);
    std::string *errorMessage = nullptr;
    while (true)
    {
        if (buf == "Ambient"s)
        {
            light = std::make_unique<AmbientLight>();
            break;
        }
        if (buf == "Directional"s)
        {
            light = std::make_unique<DirectionalLight>();
            break;
        }
        if (buf == "Spot"s)
        {
            light = std::make_unique<SpotLight>();
            break;
        }
        if (buf == "Point"s)
        {
            light = std::make_unique<PointLight>();
            break;
        }
        setLastError("Simulation::ParseLight Type=\""s + buf + "\" not recognised"s);
        return lastErrorPtr();
    }

    light->setSimulation(this);
    light->createAttributeMap(node->attributes);
    errorMessage = light->createFromAttributes();
    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }


    m_LightList[light->name()] = std::move(light);
    return nullptr;
}

// save the current model state to XML
std::string Simulation::SaveToXML()
{
    m_parseXML.elementList()->clear();

    m_global->saveToAttributes(); m_parseXML.AddElement("GLOBAL"s, m_global->attributeMap());
    for (auto &&it : m_BodyList) { it.second->saveToAttributes(); m_parseXML.AddElement("BODY"s, it.second->attributeMap()); }
    for (auto &&it : m_MarkerList) { it.second->saveToAttributes(); m_parseXML.AddElement("MARKER"s, it.second->attributeMap()); }
    for (auto &&it : m_JointList) { it.second->saveToAttributes(); m_parseXML.AddElement("JOINT"s, it.second->attributeMap()); }
    for (auto &&it : m_GeomList) { it.second->saveToAttributes(); m_parseXML.AddElement("GEOM"s, it.second->attributeMap()); }
    for (auto &&it : m_StrapList) { it.second->saveToAttributes(); m_parseXML.AddElement("STRAP"s, it.second->attributeMap()); }
    for (auto &&it : m_MuscleList) { it.second->saveToAttributes(); m_parseXML.AddElement("MUSCLE"s, it.second->attributeMap()); }
    for (auto &&it : m_FluidSacList) { it.second->saveToAttributes(); m_parseXML.AddElement("FLUIDSAC"s, it.second->attributeMap()); }
    for (auto &&it : m_ReporterList) { it.second->saveToAttributes(); m_parseXML.AddElement("REPORTER"s, it.second->attributeMap()); }
    for (auto &&it : m_ControllerList) { it.second->saveToAttributes(); m_parseXML.AddElement("CONTROLLER"s, it.second->attributeMap()); }
    for (auto &&it : m_DriverList) { it.second->saveToAttributes(); m_parseXML.AddElement("DRIVER"s, it.second->attributeMap()); }
    for (auto &&it : m_DataTargetList) { it.second->saveToAttributes(); m_parseXML.AddElement("DATATARGET"s, it.second->attributeMap()); }
    for (auto &&it : m_LightList) { it.second->saveToAttributes(); m_parseXML.AddElement("LIGHT"s, it.second->attributeMap()); }

    std::stringstream comment;
    comment << "Simulation Time: " << m_SimulationTime <<
               " Steps: " << m_StepCount <<
               " Score: " << CalculateInstantaneousFitness() <<
               " Mechanical Energy: " << m_MechanicalEnergy <<
               " Metabolic Energy: " << m_MetabolicEnergy;
    return m_parseXML.SaveModel("GAITSYM5"s, comment.str());
}

// output the simulation state in an XML format that can be re-read
void Simulation::OutputProgramState()
{
    std::string xmlString = SaveToXML();
    DataFile outputFile;
    outputFile.SetRawData(xmlString.c_str(), xmlString.size());
    outputFile.WriteFile(m_OutputModelStateFile);
}

void Simulation::SetOutputModelStateFile(const std::string &filename)
{
    m_OutputModelStateFile = filename;
}

void Simulation::SetGlobal(std::unique_ptr<Global> &&global)
{
    m_global = std::move(global);
}

bool Simulation::ShouldQuit()
{
    if (m_global->TimeLimit() > 0 && m_SimulationTime > m_global->TimeLimit()) return true;
    if (m_global->MechanicalEnergyLimit() > 0 && m_MechanicalEnergy > m_global->MechanicalEnergyLimit()) return true;
    if (m_global->MetabolicEnergyLimit() > 0 && m_MetabolicEnergy > m_global->MetabolicEnergyLimit()) return true;
    return false;
}

Body *Simulation::GetBody(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_BodyList.find(name);
    if (iter != m_BodyList.end()) return iter->second.get();
    return nullptr;
}

Joint *Simulation::GetJoint(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_JointList.find(name);
    if (iter != m_JointList.end()) return iter->second.get();
    return nullptr;
}

Geom *Simulation::GetGeom(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_GeomList.find(name);
    if (iter != m_GeomList.end()) return iter->second.get();
    return nullptr;
}

Muscle *Simulation::GetMuscle(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_MuscleList.find(name);
    if (iter != m_MuscleList.end()) return iter->second.get();
    return nullptr;
}

Strap *Simulation::GetStrap(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_StrapList.find(name);
    if (iter != m_StrapList.end()) return iter->second.get();
    return nullptr;
}

FluidSac *Simulation::GetFluidSac(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_FluidSacList.find(name);
    if (iter != m_FluidSacList.end()) return iter->second.get();
    return nullptr;
}

Driver *Simulation::GetDriver(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_DriverList.find(name);
    if (iter != m_DriverList.end()) return iter->second.get();
    return nullptr;
}

DataTarget *Simulation::GetDataTarget(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_DataTargetList.find(name);
    if (iter != m_DataTargetList.end()) return iter->second.get();
    return nullptr;
}

Marker *Simulation::GetMarker(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_MarkerList.find(name);
    if (iter != m_MarkerList.end()) return iter->second.get();
    return nullptr;
}

Reporter *Simulation::GetReporter(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_ReporterList.find(name);
    if (iter != m_ReporterList.end()) return iter->second.get();
    return nullptr;
}

Controller *Simulation::GetController(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_ControllerList.find(name);
    if (iter != m_ControllerList.end()) return iter->second.get();
    return nullptr;
}

Light *Simulation::GetLight(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_LightList.find(name);
    if (iter != m_LightList.end()) return iter->second.get();
    return nullptr;
}

Global *Simulation::GetGlobal()
{
    return m_global.get();
}

void Simulation::DumpObjects()
{
    for (auto &&it : m_BodyList) DumpObject(it.second.get());
    for (auto &&it : m_MarkerList) DumpObject(it.second.get());
    for (auto &&it : m_JointList) DumpObject(it.second.get());
    for (auto &&it : m_GeomList) DumpObject(it.second.get());
    for (auto &&it : m_FluidSacList) DumpObject(it.second.get());
    for (auto &&it : m_DriverList) DumpObject(it.second.get());
    for (auto &&it : m_DataTargetList) DumpObject(it.second.get());
    for (auto &&it : m_ReporterList) DumpObject(it.second.get());
    for (auto &&it : m_ControllerList) DumpObject(it.second.get());
    for (auto &&it : m_MuscleList)
    {
        DumpObject(it.second.get());
        DumpObject(it.second->GetStrap());
    }
}

void Simulation::DumpObject(NamedObject *namedObject)
{
    if (namedObject->dump())
    {
        if (namedObject->firstDump())
        {
            std::ofstream output;
            output.exceptions(std::ios::failbit|std::ios::badbit);
            try
            {
#if defined _WIN32 && defined _MSC_VER // required because windows and visual studio require wstring for full filename support
                output.open(DataFile::ConvertUTF8ToWide(namedObject->name() + m_dumpExtension));
#else
                output.open(namedObject->name() + m_dumpExtension);
#endif
            }
            catch (...)
            {
                std::cerr << "Error opening dump file\n";
            }
            m_dumpFileStreams[namedObject->name()] = std::move(output);
        }
        auto fileIt = m_dumpFileStreams.find(namedObject->name());
        try
        {
            if (fileIt != m_dumpFileStreams.end()) fileIt->second << namedObject->dumpToString();
        }
        catch (...)
        {
            std::cerr << "Error writing dump file\n";
        }
    }
}

std::string Simulation::kinematicsFile() const
{
    return m_kinematicsFile;
}

void Simulation::setKinematicsFile(const std::string &newKinematicsFile)
{
    m_kinematicsFile = newKinematicsFile;

    if (m_kinematicsFile.size())
    {
        m_physicsEngine = std::make_unique<PlaybackPhysicsEngine>();
        std::string *err = m_physicsEngine->Initialise(this);
        if (err)
        {
            std::cerr << "Error: unable to initialise PlaybackPhysicsEngine\n" << *err << "\n";
            m_SimulationError = true;
            return;
        }
        m_physicsEngine->Step();
        for (auto &&muscleIt : m_MuscleList) { muscleIt.second->CalculateStrap(); }
    }
}

PhysicsEngine* Simulation::physicsEngine() const
{
    return m_physicsEngine.get();
}

void Simulation::resetPhysicsEngine()
{
    m_physicsEngine.reset();
    m_SimulationTime = 0;
}

std::vector<std::string> Simulation::GetNameList() const
{
    std::vector<std::string> output;
    size_t size = m_BodyList.size() +
                  m_JointList.size() +
                  m_GeomList.size() +
                  m_MuscleList.size() +
                  m_StrapList.size() +
                  m_FluidSacList.size() +
                  m_DriverList.size() +
                  m_DataTargetList.size() +
                  m_MarkerList.size() +
                  m_ReporterList.size() +
                  m_ControllerList.size();
    output.reserve(size);
    for (auto &&it : m_BodyList) output.push_back(it.first);
    for (auto &&it : m_JointList) output.push_back(it.first);
    for (auto &&it : m_GeomList) output.push_back(it.first);
    for (auto &&it : m_MuscleList) output.push_back(it.first);
    for (auto &&it : m_StrapList) output.push_back(it.first);
    for (auto &&it : m_FluidSacList) output.push_back(it.first);
    for (auto &&it : m_DriverList) output.push_back(it.first);
    for (auto &&it : m_DataTargetList) output.push_back(it.first);
    for (auto &&it : m_MarkerList) output.push_back(it.first);
    for (auto &&it : m_ReporterList) output.push_back(it.first);
    for (auto &&it : m_ControllerList) output.push_back(it.first);
    return output;
}

std::set<std::string> Simulation::GetNameSet() const
{
    std::set<std::string> output;
    for (auto &&it : m_BodyList) output.insert(it.first);
    for (auto &&it : m_JointList) output.insert(it.first);
    for (auto &&it : m_GeomList) output.insert(it.first);
    for (auto &&it : m_MuscleList) output.insert(it.first);
    for (auto &&it : m_StrapList) output.insert(it.first);
    for (auto &&it : m_FluidSacList) output.insert(it.first);
    for (auto &&it : m_DriverList) output.insert(it.first);
    for (auto &&it : m_DataTargetList) output.insert(it.first);
    for (auto &&it : m_MarkerList) output.insert(it.first);
    for (auto &&it : m_ReporterList) output.insert(it.first);
    for (auto &&it : m_ControllerList) output.insert(it.first);
    return output;
}

std::vector<NamedObject *> Simulation::GetObjectList() const
{
    std::vector<NamedObject *> output;
    size_t size = m_BodyList.size() +
                  m_JointList.size() +
                  m_GeomList.size() +
                  m_MuscleList.size() +
                  m_StrapList.size() +
                  m_FluidSacList.size() +
                  m_DriverList.size() +
                  m_DataTargetList.size() +
                  m_MarkerList.size() +
                  m_ReporterList.size() +
                  m_ControllerList.size();
    output.reserve(size);
    // note: the order is important for resolving dependencies
    // bodies depend on nothing
    // markers depend on bodies
    // joints depend on markers
    // geoms depend on markers
    // straps depend on markers
    // muscles depend on straps
    // fluid sacs depend on markers
    // controllers depend on muscles and other drivables
    // drivers depend on controllers, muscles and other drivables
    // data targets can depend on almost anything
    // reporters can depend on almost anything
    for (auto &&it : m_BodyList) output.push_back(it.second.get());
    for (auto &&it : m_MarkerList) output.push_back(it.second.get());
    for (auto &&it : m_JointList) output.push_back(it.second.get());
    for (auto &&it : m_GeomList) output.push_back(it.second.get());
    for (auto &&it : m_StrapList) output.push_back(it.second.get());
    for (auto &&it : m_MuscleList) output.push_back(it.second.get());
    for (auto &&it : m_FluidSacList) output.push_back(it.second.get());
    for (auto &&it : m_ControllerList) output.push_back(it.second.get());
    for (auto &&it : m_DriverList) output.push_back(it.second.get());
    for (auto &&it : m_DataTargetList) output.push_back(it.second.get());
    for (auto &&it : m_ReporterList) output.push_back(it.second.get());
    return output;
}

NamedObject *Simulation::GetNamedObject(const std::string &name) const
{
    auto BodyListIt = m_BodyList.find(name); if (BodyListIt != m_BodyList.end()) return BodyListIt->second.get();
    auto JointListIt = m_JointList.find(name); if (JointListIt != m_JointList.end()) return JointListIt->second.get();
    auto GeomListIt = m_GeomList.find(name); if (GeomListIt != m_GeomList.end()) return GeomListIt->second.get();
    auto MuscleListIt = m_MuscleList.find(name); if (MuscleListIt != m_MuscleList.end()) return MuscleListIt->second.get();
    auto StrapListIt = m_StrapList.find(name); if (StrapListIt != m_StrapList.end()) return StrapListIt->second.get();
    auto FluidSacListIt = m_FluidSacList.find(name); if (FluidSacListIt != m_FluidSacList.end()) return FluidSacListIt->second.get();
    auto DriverListIt = m_DriverList.find(name); if (DriverListIt != m_DriverList.end()) return DriverListIt->second.get();
    auto DataTargetListIt = m_DataTargetList.find(name); if (DataTargetListIt != m_DataTargetList.end()) return DataTargetListIt->second.get();
    auto MarkerListIt = m_MarkerList.find(name); if (MarkerListIt != m_MarkerList.end()) return MarkerListIt->second.get();
    auto ReporterListIt = m_ReporterList.find(name); if (ReporterListIt != m_ReporterList.end()) return ReporterListIt->second.get();
    auto ControllerListIt = m_ControllerList.find(name); if (ControllerListIt != m_ControllerList.end()) return ControllerListIt->second.get();
    return nullptr;
}

bool Simulation::DeleteNamedObject(const std::string &name)
{
    auto BodyListIt = m_BodyList.find(name); if (BodyListIt != m_BodyList.end()) { m_BodyList.erase(BodyListIt); return true; }
    auto JointListIt = m_JointList.find(name); if (JointListIt != m_JointList.end()) { m_JointList.erase(JointListIt); return true; }
    auto GeomListIt = m_GeomList.find(name); if (GeomListIt != m_GeomList.end()) { m_GeomList.erase(GeomListIt); return true; }
    auto MuscleListIt = m_MuscleList.find(name); if (MuscleListIt != m_MuscleList.end()) { m_MuscleList.erase(MuscleListIt); return true; }
    auto StrapListIt = m_StrapList.find(name); if (StrapListIt != m_StrapList.end()) { m_StrapList.erase(StrapListIt); return true; }
    auto FluidSacListIt = m_FluidSacList.find(name); if (FluidSacListIt != m_FluidSacList.end()) { m_FluidSacList.erase(FluidSacListIt); return true; }
    auto DriverListIt = m_DriverList.find(name); if (DriverListIt != m_DriverList.end()) { m_DriverList.erase(DriverListIt); return true; }
    auto DataTargetListIt = m_DataTargetList.find(name); if (DataTargetListIt != m_DataTargetList.end()) { m_DataTargetList.erase(DataTargetListIt); return true; }
    auto MarkerListIt = m_MarkerList.find(name); if (MarkerListIt != m_MarkerList.end()) { m_MarkerList.erase(MarkerListIt); return true; }
    auto ReporterListIt = m_ReporterList.find(name); if (ReporterListIt != m_ReporterList.end()) { m_ReporterList.erase(ReporterListIt); return true; }
    auto ControllerListIt = m_ControllerList.find(name); if (ControllerListIt != m_ControllerList.end()) { m_ControllerList.erase(ControllerListIt); return true; }
    return false;
}

bool Simulation::HasAssembly()
{
    for (auto && it : m_JointList)
    {
        if (it.second->group() == "assembly"s) return true;
    }
    return false;
}


} // namespace GaitSym


