/*
 *  Simulation.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 24/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

// Simulation.h - this simulation object is used to encapsulate a simulation

#ifndef SIMULATION_H
#define SIMULATION_H

#include "Global.h"
#include "Contact.h"
#include "LightBase.h"
#include "ParseXML.h"
#include "SmartEnum.h"

#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <set>

namespace GaitSym
{

class Body;
class Joint;
class Geom;
class Muscle;
class Strap;
class FluidSac;
class Driver;
class DataTarget;
class Contact;
class Marker;
class Reporter;
class Controller;
class FixedJoint;
class SimulationWindow;
class MainWindow;
class Drivable;
class PhysicsEngine;
class Light;

class Simulation : NamedObject
{
public:

    Simulation();
    virtual ~Simulation();

    SMART_ENUM(AxisType, axisTypeStrings, axisTypeCount, XAxis, YAxis, ZAxis);
//    enum AxisType { XAxis, YAxis, ZAxis };

    std::string *loadModel(const char *buffer, size_t length);  // load parameters from the XML configuration file
    void updateSimulation(void);     // called at each iteration through simulation

    // get hold of various variables

    double simulationTime(void) { return m_simulationTime; }
    int64_t stepCount(void) { return m_stepCount; }
    double mechanicalEnergy(void) { return m_mechanicalEnergy; }
    double metabolicEnergy(void) { return m_metabolicEnergy; }
    Body *getBody(const std::string &name);
    Joint *getJoint(const std::string &name);
    Geom *getGeom(const std::string &name);
    Muscle *getMuscle(const std::string &name);
    Strap *getStrap(const std::string &name);
    FluidSac *getFluidSac(const std::string &name);
    Driver *getDriver(const std::string &name);
    DataTarget *getDataTarget(const std::string &name);
    Marker *getMarker(const std::string &name);
    Reporter *getReporter(const std::string &name);
    Controller *getController(const std::string &name);
    Light *getLight(const std::string &name);
    bool outputModelStateOccured() { return m_outputModelStateOccured; }

    void setOutputModelStateAtTime(double outputModelStateAtTime) { m_outputModelStateAtTime = outputModelStateAtTime; }
    void setOutputModelStateAtCycle(double outputModelStateAtCycle) { m_outputModelStateAtCycle = outputModelStateAtCycle; }
    void setOutputModelStateFile(const std::string &filename);

    // get hold of the internal lists (HANDLE WITH CARE)
    std::map<std::string, std::unique_ptr<Body>> *bodyList() { return &m_bodyList; }
    std::map<std::string, std::unique_ptr<Joint>> *jointList() { return &m_jointList; }
    std::map<std::string, std::unique_ptr<Geom>> *geomList() { return &m_geomList; }
    std::map<std::string, std::unique_ptr<Muscle>> *muscleList() { return &m_muscleList; }
    std::map<std::string, std::unique_ptr<Strap>> *strapList() { return &m_strapList; }
    std::map<std::string, std::unique_ptr<FluidSac>> *fluidSacList() { return &m_fluidSacList; }
    std::map<std::string, std::unique_ptr<Driver>> *driverList() { return &m_driverList; }
    std::map<std::string, std::unique_ptr<DataTarget>> *dataTargetList() { return &m_dataTargetList; }
    std::map<std::string, std::unique_ptr<Marker>> *markerList() { return &m_markerList; }
    std::map<std::string, std::unique_ptr<Reporter>> *reporterList() { return &m_reporterList; }
    std::map<std::string, std::unique_ptr<Controller>> *controllerList() { return &m_controllerList; }
    std::map<std::string, std::unique_ptr<Light>> *lightList() { return &m_lightList; }
    std::vector<std::unique_ptr<Contact>> *contactList() { return &m_contactList; }

    std::vector<std::string> nameList() const;
    std::set<std::string> nameSet() const;
    std::vector<NamedObject *> objectList() const;
    NamedObject *getNamedObject(const std::string &name) const;
    bool deleteNamedObject(const std::string &name);

    bool hasAssembly();

    // fitness related values
    bool testForCatastrophy();
    double calculateInstantaneousFitness();
    bool shouldQuit();
    void setContactAbort(const std::string &contactID) { m_contactAbort = true;  m_contactAbortList.push_back(contactID); }
    void setDataTargetAbort(const std::string &dataTargetID) { m_dataTargetAbort = true; m_dataTargetAbortList.push_back(dataTargetID); }

    std::string saveToXML();
    void outputProgramState();

    Global *global();
    void setGlobal(std::unique_ptr<Global> &&global);

    PhysicsEngine *physicsEngine() const;
    void resetPhysicsEngine();

    std::string kinematicsFile() const;
    void setKinematicsFile(const std::string &newKinematicsFile);

    std::string configFileRootTag() const;

private:

    std::string *parseGlobal(const ParseXML::XMLElement *node);
    std::string *parseBody(const ParseXML::XMLElement *node);
    std::string *parseGeom(const ParseXML::XMLElement *node);
    std::string *parseJoint(const ParseXML::XMLElement *node);
    std::string *parseMuscle(const ParseXML::XMLElement *node);
    std::string *parseStrap(const ParseXML::XMLElement *node);
    std::string *parseMarker(const ParseXML::XMLElement *node);
    std::string *parseFluidSac(const ParseXML::XMLElement *node);
    std::string *parseDriver(const ParseXML::XMLElement *node);
    std::string *parseDataTarget(const ParseXML::XMLElement *node);
    std::string *parseReporter(const ParseXML::XMLElement *node);
    std::string *parseController(const ParseXML::XMLElement *node);
    std::string *parseLight(const ParseXML::XMLElement *node);

    void dumpObjects();
    void dumpObject(NamedObject *namedObject);

    ParseXML m_parseXML;
    std::unique_ptr<Global> m_global;

   // these are the internal lists that are all owners of their respective objects
    std::map<std::string, std::unique_ptr<Body>> m_bodyList;
    std::map<std::string, std::unique_ptr<Joint>> m_jointList;
    std::map<std::string, std::unique_ptr<Geom>> m_geomList;
    std::map<std::string, std::unique_ptr<Muscle>> m_muscleList;
    std::map<std::string, std::unique_ptr<Strap>> m_strapList;
    std::map<std::string, std::unique_ptr<FluidSac>> m_fluidSacList;
    std::map<std::string, std::unique_ptr<Driver>> m_driverList;
    std::map<std::string, std::unique_ptr<DataTarget>> m_dataTargetList;
    std::map<std::string, std::unique_ptr<Marker>> m_markerList;
    std::map<std::string, std::unique_ptr<Reporter>> m_reporterList;
    std::map<std::string, std::unique_ptr<Controller>> m_controllerList;
    std::map<std::string, std::unique_ptr<Light>> m_lightList;

    // this is a list of contacts that are active at the current time step
    std::vector<std::unique_ptr<Contact>> m_contactList;

    // keep track of simulation time
    double m_simulationTime = 0; // current time
    int64_t m_stepCount = 0; // number of steps taken
    double m_cycleTime = 0;

    // and calculated energy
    double m_mechanicalEnergy = 0;
    double m_metabolicEnergy = 0;

    // FitnessType
    double m_targetMatchMiniMaxFitness = 0;
    double m_targetMatchMaxiMinFitness = 0;

    // some control values
    bool m_outputModelStateOccured = false;
    bool m_abortAfterModelStateOutput = false;
    double m_outputModelStateAtTime = -1;
    double m_outputModelStateAtCycle = -1;
    std::string m_outputModelStateFile;
    int m_simulationError = false;
    bool m_dataTargetAbort = false;
    bool m_contactAbort = false;
    std::vector<std::string> m_dataTargetAbortList;
    std::vector<std::string> m_contactAbortList;
    int m_numericalErrorCount = 0;

    // for fitness calculations
    double m_targetMatchFitness = 0;

    // values for dump output
    std::string m_dumpExtension = {".tab"};
    std::map<std::string, std::ofstream> m_dumpFileStreams;

    // physics engine
    std::unique_ptr<PhysicsEngine> m_physicsEngine;
    std::string m_kinematicsFile;

    // file read control
    std::string m_configFileRootTag;
};

}

#endif //SIMULATION_H
