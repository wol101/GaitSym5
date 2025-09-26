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
std::string *Simulation::loadModel(const char *buffer, size_t length) // note this requires buffer to be a 0 terminated string of size length + 1
{
    std::string rootTag;
    std::string *ptr = m_parseXML.loadModel(buffer, length, &rootTag);
    if (ptr) return ptr;

    while(true)
    {
        if (rootTag == "GAITSYM5"s)
        {
            m_configFileRootTag = rootTag;
            break;
        }
        if (rootTag == "GAITSYM5L"s)
        {
            m_configFileRootTag = rootTag;
            break;
        }
        if (rootTag == "GAITSYM2019"s)
        {
            m_configFileRootTag = rootTag;
            break;
        }
        setLastError("Error in LoadModel: unrecognised XML, rootTag = \""s + rootTag);
        return lastErrorPtr();
    }

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
            if ((*it)->tag == "GLOBAL"s) parseGlobal(*it);
            else if ((*it)->tag == "BODY"s) parseBody(*it);
            else if ((*it)->tag == "JOINT"s) parseJoint(*it);
            else if ((*it)->tag == "GEOM"s) parseGeom(*it);
            else if ((*it)->tag == "STRAP"s) parseStrap(*it);
            else if ((*it)->tag == "MUSCLE"s) parseMuscle(*it);
            else if ((*it)->tag == "DRIVER"s) parseDriver(*it);
            else if ((*it)->tag == "DATATARGET"s) parseDataTarget(*it);
            else if ((*it)->tag == "MARKER"s) parseMarker(*it);
            else if ((*it)->tag == "REPORTER"s) parseReporter(*it);
            else if ((*it)->tag == "CONTROLLER"s) parseController(*it);
            else if ((*it)->tag == "FLUIDSAC"s) parseFluidSac(*it);
            else if ((*it)->tag == "LIGHT"s) parseLight(*it);
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
    for (auto &&it : m_bodyList) it.second->lateInitialisation();
    // and we recalculate the dynamic items with the new muscle positions
    for (auto &&it :  m_muscleList) it.second->lateInitialisation();
    for (auto &&it : m_fluidSacList) it.second->lateInitialisation();
    // and some joints require things to be done after the bodies are moved to their start positions
    for (auto &&it :  m_jointList) it.second->lateInitialisation();

    // and we need to set the cycle time
    // currently just using the maximum value but some sort of fuzzy lowest common multiple might be better
    // the easiest way to do that is to mutiply by an appropriate power of 10 with nearest number rounding (int(v * 10000 + 0.5)) to make the numbers into integers and then use an integer formula and convert back
    // using std::lcm from numeric with accumulate so it works on a container (a std::set makes sense for longer lists perhas)
    // std::vector<int> v{4, 6, 10};
    // auto lcm = std::accumulate(v.begin(), v.end(), 1, [](auto & a, auto & b) { return std::lcm(a, b); });
    m_cycleTime = 0;
    for (auto &&driver : m_driverList)
    {
        if (CyclicDriver *cyclicDriver = dynamic_cast<CyclicDriver*>(driver.second.get())) m_cycleTime = std::max(cyclicDriver->cycleTime(), m_cycleTime);
        else if (StackedBoxcarDriver *stackedBoxcarDriver = dynamic_cast<StackedBoxcarDriver*>(driver.second.get())) m_cycleTime = std::max(stackedBoxcarDriver->cycleTime(), m_cycleTime);
    }
    return nullptr;
}


//----------------------------------------------------------------------------
void Simulation::updateSimulation()
{
    std::string *err = nullptr;
    if (!m_physicsEngine)
    {
        switch (m_global->physicsEngine())
        {
        case Global::PhysicsEngine::ODE:
            {
                m_physicsEngine = std::make_unique<ODEPhysicsEngine>();
                err = m_physicsEngine->initialise(this);
                if (err)
                {
                    std::cerr << "Error: unable to initialise ODEPhysicsEngine\n" << *err << "\n";
                    m_simulationError = true;
                    return;
                }
                break;
            }
        case Global::PhysicsEngine::PhysX:
            {
                m_physicsEngine = std::make_unique<PhysXPhysicsEngine>();
                err = m_physicsEngine->initialise(this);
                if (err)
                {
                    std::cerr << "Error: unable to initialise PhysXPhysicsEngine\n" << *err << "\n";
                    m_simulationError = true;
                    return;
                }
                break;
            }
        case Global::PhysicsEngine::MuJoCo:
            {
                m_physicsEngine = std::make_unique<MuJoCoPhysicsEngine>();
                err = m_physicsEngine->initialise(this);
                if (err)
                {
                    std::cerr << "Error: unable to initialise MuJoCoPhysicsEngine\n" << *err << "\n";
                    m_simulationError = true;
                    return;
                }
                break;
            }
        }
    }

    // start by updating the scores
    double minScore = std::numeric_limits<double>::infinity();
    double maxScore = -std::numeric_limits<double>::infinity();
    for (auto &&it : m_dataTargetList)
    {
        double matchScore;
        bool matchScoreValid = it.second->calculateMatchValue(m_simulationTime, &matchScore);
        if (matchScoreValid)
        {
            m_targetMatchFitness += matchScore;
            if (matchScore < minScore) minScore = matchScore;
            if (matchScore > maxScore) maxScore = matchScore;
        }
    }
    if (minScore < std::numeric_limits<double>::infinity()) m_targetMatchMaxiMinFitness += minScore;
    if (maxScore > -std::numeric_limits<double>::infinity()) m_targetMatchMiniMaxFitness += maxScore;

    // now start the actual simulation

    // clear the contacts from the geoms and in the master list
    for (auto &&geomIter : m_geomList) { geomIter.second->clearContacts(); }
    m_contactList.clear();

    // update the drivers
    for (auto &&it : m_driverList)
    {
        it.second->update();
        it.second->sendData();
    }
    // and the controllers (which are drivers too probably)
    for (auto &&it : m_controllerList)
    {
        auto driver = dynamic_cast<Driver *>(it.second.get());
        if (driver)
        {
            driver->update();
            driver->sendData();
        }
        if (it.second->lastStepCount() != m_stepCount)
            std::cerr << "Warning: " << it.first << " controller not updated\n"; // currently cannot stack controllers although this is fixable
    }

    // update the muscles
    for (auto iter1 = m_muscleList.begin(); iter1 != m_muscleList.end(); /* no increment */ )
    {
        // muscle straps are valid at this point so they do not need recaulculating
        iter1->second->updateActivation();

        // check for breaking strain
        if (DampedSpringMuscle *dampedSpringMuscle = dynamic_cast<DampedSpringMuscle *>(iter1->second.get()))
        {
            if (dampedSpringMuscle->shouldBreak())
            {
                iter1 = m_muscleList.erase(iter1); // erase returns the next iterator [but m_MuscleList.erase(iter1++) would also work and is compatible with older C++ compilers]
                continue;
            }
        }
        iter1++; // this has to be done outside the for definition because erase returns the next iterator
    }

    // update the joints (needed for motors, end stops and stress calculations)
    for (auto &&jointIter : m_jointList) jointIter.second->update();

    // update the fluid sacs
    for (auto fsIter = m_fluidSacList.begin(); fsIter != m_fluidSacList.end(); fsIter++)
    {
        fsIter->second->calculateVolume();
        fsIter->second->calculatePressure();
        fsIter->second->calculateLoadsOnMarkers();
    }

    // update the bodies (needed for drag calculations)
    for (auto &&bodyIter : m_bodyList) { bodyIter.second->computeDrag(); }

    // output the model state if triggered
    if (m_outputModelStateAtTime >= 0.0)
    {
        if (m_simulationTime >= m_outputModelStateAtTime)
        {
            outputProgramState();
            m_outputModelStateAtTime = -1;
        }
    }
    else if (m_outputModelStateAtCycle >= 0 && m_cycleTime >= 0 && m_simulationTime >= m_cycleTime * m_outputModelStateAtCycle)
    {
        outputProgramState();
        m_outputModelStateAtCycle = -1;
    }

    // run the simulation
    m_physicsEngine->step();

    // now the muscle straps are invalid because the bodies have moved so they need recalculating
    for (auto &&iter1 : m_muscleList)
    {
        iter1.second->calculateStrap();
    }

    // calculate the energies
    for (auto &&iter1 : m_muscleList)
    {
        m_mechanicalEnergy += iter1.second->power() * m_global->stepSize();
        m_metabolicEnergy += iter1.second->metabolicPower() * m_global->stepSize();
    }
    m_metabolicEnergy += m_global->BMR() * m_global->stepSize();

    // update any contact force dependent drivers (because only after the simulation is the force valid
    // update the footprint indicator
    if (m_contactList.size() > 0)
    {
        for (auto &&it : m_driverList)
        {
            if (TegotaeDriver *tegotaeDriver = dynamic_cast<TegotaeDriver *>(it.second.get())) tegotaeDriver->updateReactionForce();
        }
    }

    // all reporting is done after a simulation step

    dumpObjects();

    // update the time counter
    m_simulationTime += m_global->stepSize();

    // update the step counter
    m_stepCount++;
}

//----------------------------------------------------------------------------
bool Simulation::testForCatastrophy()
{
    // check for simulation error
    if (m_simulationError)
    {
        std::cerr << "Failed due to simulation error " << m_simulationError << "\n";
        return true;
    }

    // check for contact abort
    if (m_contactAbort)
    {
        std::cerr << "Failed due to contact abort\n";
        for (auto &&it: m_contactAbortList) { std::cerr << it << "\n"; }
        return true;
    }

    // check for data target abort
    if (m_dataTargetAbort)
    {
        std::cerr << "Failed due to DataTarget abort\n";
        for (auto &&it: m_dataTargetAbortList) { std::cerr << it << "\n"; }
        return true;
    }

    // check that all bodies meet velocity and stop conditions

    Body::LimitTestResult p;
    for (auto &&iter1 : m_bodyList)
    {
        p = iter1.second->testLimits();
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

    for (auto &&iter3 : m_jointList)
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
            if (f->checkStressAbort())
            {
                std::cerr << "Failed due to stress limit error in: " << iter3.second->name() << " " << f->lowPassMinStress() << " " << f->lowPassMaxStress() << "\n";
                return true;
            }
        }
    }

    // and test the reporters for stop conditions
    for (auto &&reporterIter : m_reporterList)
    {
        if (reporterIter.second->shouldAbort())
        {
            std::cerr << "Failed due to Reporter Abort in: " << reporterIter.second->name() << "\n";
            return true;
        }
    }

    if (m_outputModelStateOccured && m_abortAfterModelStateOutput)
    {
        std::cerr << "Abort because ModelState successfully written\n";
        return true;
    }

    return false;
}


//----------------------------------------------------------------------------
double Simulation::calculateInstantaneousFitness()
{
    switch (m_global->fitnessType())
    {
    case Global::TargetSum:
        return m_targetMatchFitness;

    case Global::TargetMiniMax:
        return m_targetMatchMiniMaxFitness;

    case Global::TargetMaxiMin:
        return m_targetMatchMaxiMinFitness;
    }
    return 0;
}

std::string *Simulation::parseGlobal(const ParseXML::XMLElement *node)
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
    this->setGlobal(std::move(global));
    return nullptr;
}

std::string *Simulation::parseBody(const ParseXML::XMLElement *node)
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
    m_bodyList[body->name()] = std::move(body);
    return nullptr;
}

std::string *Simulation::parseMarker(const ParseXML::XMLElement *node)
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
    m_markerList[marker->name()] = std::move(marker);
    return nullptr;
}

std::string *Simulation::parseJoint(const ParseXML::XMLElement *node)
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
        setLastError("Simulation::parseJoint Type=\"" + buf + "\" not recognised");
        return lastErrorPtr();
    }

    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }
    m_jointList[joint->name()] = std::move(joint);
    return nullptr;
}

std::string *Simulation::parseGeom(const ParseXML::XMLElement *node)
{
    // GEOMs require a valid GLOBAL to get step size
    if (!m_global)
    {
        setLastError("Simulation::parseGeom requires a valid GLOBAL"s);
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
        setLastError("Simulation::parseGeom Type=\""s + buf + "\" not recognised"s);
        return lastErrorPtr();
    }

    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }

    m_geomList[geom->name()] = std::move(geom);
    return nullptr;
}

std::string *Simulation::parseMuscle(const ParseXML::XMLElement *node)
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
        setLastError("Simulation::parseMuscle Type=\""s + buf + "\" not recognised"s);
        return lastErrorPtr();
    }

    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }

    m_muscleList[muscle->name()] = std::move(muscle);
    return nullptr;
}

std::string *Simulation::parseStrap(const ParseXML::XMLElement *node)
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
        setLastError("Simulation::parseStrap Type=\""s + buf + "\" not recognised"s);
        return lastErrorPtr();
    }

    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }

    m_strapList[strap->name()] = std::move(strap);
    return nullptr;
}

std::string *Simulation::parseFluidSac(const ParseXML::XMLElement *node)
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
        setLastError("Simulation::parseFluidSac Type=\""s + buf + "\" not recognised"s);
        return lastErrorPtr();
    }

    if (errorMessage)
    {
        setLastError(*errorMessage);
        return lastErrorPtr();
    }

    m_fluidSacList[fluidSac->name()] = std::move(fluidSac);
    return nullptr;
}

std::string *Simulation::parseDriver(const ParseXML::XMLElement *node)
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
        setLastError("Simulation::parseDriver Type=\""s + buf + "\" not recognised"s);
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

    m_driverList[driver->name()] = std::move(driver);
    return nullptr;
}


std::string *Simulation::parseDataTarget(const ParseXML::XMLElement *node)
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
        setLastError("Simulation::parseDataTarget Type=\""s + buf + "\" not recognised"s);
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

    m_dataTargetList[dataTarget->name()] = std::move(dataTarget);
    return nullptr;

}

std::string *Simulation::parseReporter(const ParseXML::XMLElement *node)
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
        setLastError("Simulation::parseReporter Type=\""s + buf + "\" not recognised"s);
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

    m_reporterList[reporter->name()] = std::move(reporter);
    return nullptr;
}

std::string *Simulation::parseController(const ParseXML::XMLElement *node)
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
        setLastError("Simulation::parseController Type=\""s + buf + "\" not recognised"s);
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

    m_controllerList[controller->name()] = std::move(controller);
    return nullptr;
}

std::string *Simulation::parseLight(const ParseXML::XMLElement *node)
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
        setLastError("Simulation::parseLight Type=\""s + buf + "\" not recognised"s);
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


    m_lightList[light->name()] = std::move(light);
    return nullptr;
}

// save the current model state to XML
std::string Simulation::saveToXML()
{
    m_parseXML.elementList()->clear();

    m_global->saveToAttributes(); m_parseXML.addElement("GLOBAL"s, m_global->attributeMap());
    for (auto &&it : m_bodyList) { it.second->saveToAttributes(); m_parseXML.addElement("BODY"s, it.second->attributeMap()); }
    for (auto &&it : m_markerList) { it.second->saveToAttributes(); m_parseXML.addElement("MARKER"s, it.second->attributeMap()); }
    for (auto &&it : m_jointList) { it.second->saveToAttributes(); m_parseXML.addElement("JOINT"s, it.second->attributeMap()); }
    for (auto &&it : m_geomList) { it.second->saveToAttributes(); m_parseXML.addElement("GEOM"s, it.second->attributeMap()); }
    for (auto &&it : m_strapList) { it.second->saveToAttributes(); m_parseXML.addElement("STRAP"s, it.second->attributeMap()); }
    for (auto &&it : m_muscleList) { it.second->saveToAttributes(); m_parseXML.addElement("MUSCLE"s, it.second->attributeMap()); }
    for (auto &&it : m_fluidSacList) { it.second->saveToAttributes(); m_parseXML.addElement("FLUIDSAC"s, it.second->attributeMap()); }
    for (auto &&it : m_reporterList) { it.second->saveToAttributes(); m_parseXML.addElement("REPORTER"s, it.second->attributeMap()); }
    for (auto &&it : m_controllerList) { it.second->saveToAttributes(); m_parseXML.addElement("CONTROLLER"s, it.second->attributeMap()); }
    for (auto &&it : m_driverList) { it.second->saveToAttributes(); m_parseXML.addElement("DRIVER"s, it.second->attributeMap()); }
    for (auto &&it : m_dataTargetList) { it.second->saveToAttributes(); m_parseXML.addElement("DATATARGET"s, it.second->attributeMap()); }
    for (auto &&it : m_lightList) { it.second->saveToAttributes(); m_parseXML.addElement("LIGHT"s, it.second->attributeMap()); }

    std::stringstream comment;
    comment << "Simulation Time: " << m_simulationTime <<
               " Steps: " << m_stepCount <<
               " Score: " << calculateInstantaneousFitness() <<
               " Mechanical Energy: " << m_mechanicalEnergy <<
               " Metabolic Energy: " << m_metabolicEnergy;
    return m_parseXML.saveModel("GAITSYM5"s, comment.str());
}

// output the simulation state in an XML format that can be re-read
void Simulation::outputProgramState()
{
    std::string xmlString = saveToXML();
    DataFile outputFile;
    outputFile.setRawData(xmlString.c_str(), xmlString.size());
    outputFile.writeFile(m_outputModelStateFile);
}

void Simulation::setOutputModelStateFile(const std::string &filename)
{
    m_outputModelStateFile = filename;
}

void Simulation::setGlobal(std::unique_ptr<Global> &&global)
{
    m_global = std::move(global);
}

bool Simulation::shouldQuit()
{
    if (m_global->timeLimit() > 0 && m_simulationTime > m_global->timeLimit()) return true;
    if (m_global->mechanicalEnergyLimit() > 0 && m_mechanicalEnergy > m_global->mechanicalEnergyLimit()) return true;
    if (m_global->metabolicEnergyLimit() > 0 && m_metabolicEnergy > m_global->metabolicEnergyLimit()) return true;
    return false;
}

Body *Simulation::getBody(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_bodyList.find(name);
    if (iter != m_bodyList.end()) return iter->second.get();
    return nullptr;
}

Joint *Simulation::getJoint(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_jointList.find(name);
    if (iter != m_jointList.end()) return iter->second.get();
    return nullptr;
}

Geom *Simulation::getGeom(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_geomList.find(name);
    if (iter != m_geomList.end()) return iter->second.get();
    return nullptr;
}

Muscle *Simulation::getMuscle(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_muscleList.find(name);
    if (iter != m_muscleList.end()) return iter->second.get();
    return nullptr;
}

Strap *Simulation::getStrap(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_strapList.find(name);
    if (iter != m_strapList.end()) return iter->second.get();
    return nullptr;
}

FluidSac *Simulation::getFluidSac(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_fluidSacList.find(name);
    if (iter != m_fluidSacList.end()) return iter->second.get();
    return nullptr;
}

Driver *Simulation::getDriver(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_driverList.find(name);
    if (iter != m_driverList.end()) return iter->second.get();
    return nullptr;
}

DataTarget *Simulation::getDataTarget(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_dataTargetList.find(name);
    if (iter != m_dataTargetList.end()) return iter->second.get();
    return nullptr;
}

Marker *Simulation::getMarker(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_markerList.find(name);
    if (iter != m_markerList.end()) return iter->second.get();
    return nullptr;
}

Reporter *Simulation::getReporter(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_reporterList.find(name);
    if (iter != m_reporterList.end()) return iter->second.get();
    return nullptr;
}

Controller *Simulation::getController(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_controllerList.find(name);
    if (iter != m_controllerList.end()) return iter->second.get();
    return nullptr;
}

Light *Simulation::getLight(const std::string &name)
{
    // use find to allow null return if name not found
    auto iter = m_lightList.find(name);
    if (iter != m_lightList.end()) return iter->second.get();
    return nullptr;
}

Global *Simulation::global()
{
    return m_global.get();
}

void Simulation::dumpObjects()
{
    for (auto &&it : m_bodyList) dumpObject(it.second.get());
    for (auto &&it : m_markerList) dumpObject(it.second.get());
    for (auto &&it : m_jointList) dumpObject(it.second.get());
    for (auto &&it : m_geomList) dumpObject(it.second.get());
    for (auto &&it : m_fluidSacList) dumpObject(it.second.get());
    for (auto &&it : m_driverList) dumpObject(it.second.get());
    for (auto &&it : m_dataTargetList) dumpObject(it.second.get());
    for (auto &&it : m_reporterList) dumpObject(it.second.get());
    for (auto &&it : m_controllerList) dumpObject(it.second.get());
    for (auto &&it : m_muscleList)
    {
        dumpObject(it.second.get());
        dumpObject(it.second->strap());
    }
}

void Simulation::dumpObject(NamedObject *namedObject)
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
                output.open(DataFile::convertUTF8ToWide(namedObject->name() + m_dumpExtension));
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

std::string Simulation::configFileRootTag() const
{
    return m_configFileRootTag;
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
        std::string *err = m_physicsEngine->initialise(this);
        if (err)
        {
            std::cerr << "Error: unable to initialise PlaybackPhysicsEngine\n" << *err << "\n";
            m_simulationError = true;
            return;
        }
        m_physicsEngine->step();
        for (auto &&muscleIt : m_muscleList) { muscleIt.second->calculateStrap(); }
    }
}

PhysicsEngine* Simulation::physicsEngine() const
{
    return m_physicsEngine.get();
}

void Simulation::resetPhysicsEngine()
{
    m_physicsEngine.reset();
    m_simulationTime = 0;
}

std::vector<std::string> Simulation::nameList() const
{
    std::vector<std::string> output;
    size_t size = m_bodyList.size() +
                  m_jointList.size() +
                  m_geomList.size() +
                  m_muscleList.size() +
                  m_strapList.size() +
                  m_fluidSacList.size() +
                  m_driverList.size() +
                  m_dataTargetList.size() +
                  m_markerList.size() +
                  m_reporterList.size() +
                  m_controllerList.size();
    output.reserve(size);
    for (auto &&it : m_bodyList) output.push_back(it.first);
    for (auto &&it : m_jointList) output.push_back(it.first);
    for (auto &&it : m_geomList) output.push_back(it.first);
    for (auto &&it : m_muscleList) output.push_back(it.first);
    for (auto &&it : m_strapList) output.push_back(it.first);
    for (auto &&it : m_fluidSacList) output.push_back(it.first);
    for (auto &&it : m_driverList) output.push_back(it.first);
    for (auto &&it : m_dataTargetList) output.push_back(it.first);
    for (auto &&it : m_markerList) output.push_back(it.first);
    for (auto &&it : m_reporterList) output.push_back(it.first);
    for (auto &&it : m_controllerList) output.push_back(it.first);
    return output;
}

std::set<std::string> Simulation::nameSet() const
{
    std::set<std::string> output;
    for (auto &&it : m_bodyList) output.insert(it.first);
    for (auto &&it : m_jointList) output.insert(it.first);
    for (auto &&it : m_geomList) output.insert(it.first);
    for (auto &&it : m_muscleList) output.insert(it.first);
    for (auto &&it : m_strapList) output.insert(it.first);
    for (auto &&it : m_fluidSacList) output.insert(it.first);
    for (auto &&it : m_driverList) output.insert(it.first);
    for (auto &&it : m_dataTargetList) output.insert(it.first);
    for (auto &&it : m_markerList) output.insert(it.first);
    for (auto &&it : m_reporterList) output.insert(it.first);
    for (auto &&it : m_controllerList) output.insert(it.first);
    return output;
}

std::vector<NamedObject *> Simulation::objectList() const
{
    std::vector<NamedObject *> output;
    size_t size = m_bodyList.size() +
                  m_jointList.size() +
                  m_geomList.size() +
                  m_muscleList.size() +
                  m_strapList.size() +
                  m_fluidSacList.size() +
                  m_driverList.size() +
                  m_dataTargetList.size() +
                  m_markerList.size() +
                  m_reporterList.size() +
                  m_controllerList.size();
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
    for (auto &&it : m_bodyList) output.push_back(it.second.get());
    for (auto &&it : m_markerList) output.push_back(it.second.get());
    for (auto &&it : m_jointList) output.push_back(it.second.get());
    for (auto &&it : m_geomList) output.push_back(it.second.get());
    for (auto &&it : m_strapList) output.push_back(it.second.get());
    for (auto &&it : m_muscleList) output.push_back(it.second.get());
    for (auto &&it : m_fluidSacList) output.push_back(it.second.get());
    for (auto &&it : m_controllerList) output.push_back(it.second.get());
    for (auto &&it : m_driverList) output.push_back(it.second.get());
    for (auto &&it : m_dataTargetList) output.push_back(it.second.get());
    for (auto &&it : m_reporterList) output.push_back(it.second.get());
    return output;
}

NamedObject *Simulation::getNamedObject(const std::string &name) const
{
    auto BodyListIt = m_bodyList.find(name); if (BodyListIt != m_bodyList.end()) return BodyListIt->second.get();
    auto JointListIt = m_jointList.find(name); if (JointListIt != m_jointList.end()) return JointListIt->second.get();
    auto GeomListIt = m_geomList.find(name); if (GeomListIt != m_geomList.end()) return GeomListIt->second.get();
    auto MuscleListIt = m_muscleList.find(name); if (MuscleListIt != m_muscleList.end()) return MuscleListIt->second.get();
    auto StrapListIt = m_strapList.find(name); if (StrapListIt != m_strapList.end()) return StrapListIt->second.get();
    auto FluidSacListIt = m_fluidSacList.find(name); if (FluidSacListIt != m_fluidSacList.end()) return FluidSacListIt->second.get();
    auto DriverListIt = m_driverList.find(name); if (DriverListIt != m_driverList.end()) return DriverListIt->second.get();
    auto DataTargetListIt = m_dataTargetList.find(name); if (DataTargetListIt != m_dataTargetList.end()) return DataTargetListIt->second.get();
    auto MarkerListIt = m_markerList.find(name); if (MarkerListIt != m_markerList.end()) return MarkerListIt->second.get();
    auto ReporterListIt = m_reporterList.find(name); if (ReporterListIt != m_reporterList.end()) return ReporterListIt->second.get();
    auto ControllerListIt = m_controllerList.find(name); if (ControllerListIt != m_controllerList.end()) return ControllerListIt->second.get();
    return nullptr;
}

bool Simulation::deleteNamedObject(const std::string &name)
{
    auto BodyListIt = m_bodyList.find(name); if (BodyListIt != m_bodyList.end()) { m_bodyList.erase(BodyListIt); return true; }
    auto JointListIt = m_jointList.find(name); if (JointListIt != m_jointList.end()) { m_jointList.erase(JointListIt); return true; }
    auto GeomListIt = m_geomList.find(name); if (GeomListIt != m_geomList.end()) { m_geomList.erase(GeomListIt); return true; }
    auto MuscleListIt = m_muscleList.find(name); if (MuscleListIt != m_muscleList.end()) { m_muscleList.erase(MuscleListIt); return true; }
    auto StrapListIt = m_strapList.find(name); if (StrapListIt != m_strapList.end()) { m_strapList.erase(StrapListIt); return true; }
    auto FluidSacListIt = m_fluidSacList.find(name); if (FluidSacListIt != m_fluidSacList.end()) { m_fluidSacList.erase(FluidSacListIt); return true; }
    auto DriverListIt = m_driverList.find(name); if (DriverListIt != m_driverList.end()) { m_driverList.erase(DriverListIt); return true; }
    auto DataTargetListIt = m_dataTargetList.find(name); if (DataTargetListIt != m_dataTargetList.end()) { m_dataTargetList.erase(DataTargetListIt); return true; }
    auto MarkerListIt = m_markerList.find(name); if (MarkerListIt != m_markerList.end()) { m_markerList.erase(MarkerListIt); return true; }
    auto ReporterListIt = m_reporterList.find(name); if (ReporterListIt != m_reporterList.end()) { m_reporterList.erase(ReporterListIt); return true; }
    auto ControllerListIt = m_controllerList.find(name); if (ControllerListIt != m_controllerList.end()) { m_controllerList.erase(ControllerListIt); return true; }
    return false;
}

bool Simulation::hasAssembly()
{
    for (auto && it : m_jointList)
    {
        if (it.second->group() == "assembly"s) return true;
    }
    return false;
}


} // namespace GaitSym


