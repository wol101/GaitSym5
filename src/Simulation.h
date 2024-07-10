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

    std::string *LoadModel(const char *buffer, size_t length);  // load parameters from the XML configuration file
    void UpdateSimulation(void);     // called at each iteration through simulation

    // get hold of various variables

    double GetTime(void) { return m_SimulationTime; }
    double GetTimeIncrement(void) { return m_global->StepSize(); }
    long long GetStepCount(void) { return m_StepCount; }
    double GetMechanicalEnergy(void) { return m_MechanicalEnergy; }
    double GetMetabolicEnergy(void) { return m_MetabolicEnergy; }
    double GetTimeLimit(void) { return m_global->TimeLimit(); }
    double GetMetabolicEnergyLimit(void) { return m_global->MetabolicEnergyLimit(); }
    double GetMechanicalEnergyLimit(void) { return m_global->MechanicalEnergyLimit(); }
    Body *GetBody(const std::string &name);
    Joint *GetJoint(const std::string &name);
    Geom *GetGeom(const std::string &name);
    Muscle *GetMuscle(const std::string &name);
    Strap *GetStrap(const std::string &name);
    FluidSac *GetFluidSac(const std::string &name);
    Driver *GetDriver(const std::string &name);
    DataTarget *GetDataTarget(const std::string &name);
    Marker *GetMarker(const std::string &name);
    Reporter *GetReporter(const std::string &name);
    Controller *GetController(const std::string &name);
    Light *GetLight(const std::string &name);
    bool GetOutputModelStateOccured() { return m_OutputModelStateOccured; }

    void SetTimeLimit(double timeLimit) { m_global->setTimeLimit(timeLimit); }
    void SetMetabolicEnergyLimit(double energyLimit) { m_global->setMetabolicEnergyLimit(energyLimit); }
    void SetMechanicalEnergyLimit(double energyLimit) { m_global->setMechanicalEnergyLimit(energyLimit); }
    void SetOutputModelStateAtTime(double outputModelStateAtTime) { m_OutputModelStateAtTime = outputModelStateAtTime; }
    void SetOutputModelStateAtCycle(double outputModelStateAtCycle) { m_OutputModelStateAtCycle = outputModelStateAtCycle; }
    void SetOutputModelStateFile(const std::string &filename);

    // get hold of the internal lists (HANDLE WITH CARE)
    std::map<std::string, std::unique_ptr<Body>> *GetBodyList() { return &m_BodyList; }
    std::map<std::string, std::unique_ptr<Joint>> *GetJointList() { return &m_JointList; }
    std::map<std::string, std::unique_ptr<Geom>> *GetGeomList() { return &m_GeomList; }
    std::map<std::string, std::unique_ptr<Muscle>> *GetMuscleList() { return &m_MuscleList; }
    std::map<std::string, std::unique_ptr<Strap>> *GetStrapList() { return &m_StrapList; }
    std::map<std::string, std::unique_ptr<FluidSac>> *GetFluidSacList() { return &m_FluidSacList; }
    std::map<std::string, std::unique_ptr<Driver>> *GetDriverList() { return &m_DriverList; }
    std::map<std::string, std::unique_ptr<DataTarget>> *GetDataTargetList() { return &m_DataTargetList; }
    std::map<std::string, std::unique_ptr<Marker>> *GetMarkerList() { return &m_MarkerList; }
    std::map<std::string, std::unique_ptr<Reporter>> *GetReporterList() { return &m_ReporterList; }
    std::map<std::string, std::unique_ptr<Controller>> *GetControllerList() { return &m_ControllerList; }
    std::map<std::string, std::unique_ptr<Light>> *GetlightList() { return &m_LightList; }
    std::vector<std::unique_ptr<Contact>> *GetContactList() { return &m_ContactList; }

    std::vector<std::string> GetNameList() const;
    std::set<std::string> GetNameSet() const;
    std::vector<NamedObject *> GetObjectList() const;
    NamedObject *GetNamedObject(const std::string &name) const;
    bool DeleteNamedObject(const std::string &name);

    bool HasAssembly();

    // fitness related values
    bool TestForCatastrophy();
    double CalculateInstantaneousFitness();
    bool ShouldQuit();
    void SetContactAbort(const std::string &contactID) { m_ContactAbort = true;  m_ContactAbortList.push_back(contactID); }
    void SetDataTargetAbort(const std::string &dataTargetID) { m_DataTargetAbort = true; m_DataTargetAbortList.push_back(dataTargetID); }

    std::string SaveToXML();
    void OutputProgramState();

    Global *GetGlobal();
    void SetGlobal(std::unique_ptr<Global> &&global);

    PhysicsEngine *physicsEngine() const;
    void resetPhysicsEngine();

    std::string kinematicsFile() const;
    void setKinematicsFile(const std::string &newKinematicsFile);

private:

    std::string *ParseGlobal(const ParseXML::XMLElement *node);
    std::string *ParseBody(const ParseXML::XMLElement *node);
    std::string *ParseGeom(const ParseXML::XMLElement *node);
    std::string *ParseJoint(const ParseXML::XMLElement *node);
    std::string *ParseMuscle(const ParseXML::XMLElement *node);
    std::string *ParseStrap(const ParseXML::XMLElement *node);
    std::string *ParseMarker(const ParseXML::XMLElement *node);
    std::string *ParseFluidSac(const ParseXML::XMLElement *node);
    std::string *ParseDriver(const ParseXML::XMLElement *node);
    std::string *ParseDataTarget(const ParseXML::XMLElement *node);
    std::string *ParseReporter(const ParseXML::XMLElement *node);
    std::string *ParseController(const ParseXML::XMLElement *node);
    std::string *ParseLight(const ParseXML::XMLElement *node);

    void DumpObjects();
    void DumpObject(NamedObject *namedObject);

    ParseXML m_parseXML;
    std::unique_ptr<Global> m_global;

   // these are the internal lists that are all owners of their respective objects
    std::map<std::string, std::unique_ptr<Body>> m_BodyList;
    std::map<std::string, std::unique_ptr<Joint>> m_JointList;
    std::map<std::string, std::unique_ptr<Geom>> m_GeomList;
    std::map<std::string, std::unique_ptr<Muscle>> m_MuscleList;
    std::map<std::string, std::unique_ptr<Strap>> m_StrapList;
    std::map<std::string, std::unique_ptr<FluidSac>> m_FluidSacList;
    std::map<std::string, std::unique_ptr<Driver>> m_DriverList;
    std::map<std::string, std::unique_ptr<DataTarget>> m_DataTargetList;
    std::map<std::string, std::unique_ptr<Marker>> m_MarkerList;
    std::map<std::string, std::unique_ptr<Reporter>> m_ReporterList;
    std::map<std::string, std::unique_ptr<Controller>> m_ControllerList;
    std::map<std::string, std::unique_ptr<Light>> m_LightList;

    // this is a list of contacts that are active at the current time step
    std::vector<std::unique_ptr<Contact>> m_ContactList;

    // keep track of simulation time
    double m_SimulationTime = 0; // current time
    int64_t m_StepCount = 0; // number of steps taken
    double m_CycleTime = 0;

    // and calculated energy
    double m_MechanicalEnergy = 0;
    double m_MetabolicEnergy = 0;

    // FitnessType
    double m_TargetMatchMiniMaxFitness = 0;
    double m_TargetMatchMaxiMinFitness = 0;

    // some control values
    bool m_OutputModelStateOccured = false;
    bool m_AbortAfterModelStateOutput = false;
    double m_OutputModelStateAtTime = -1;
    double m_OutputModelStateAtCycle = -1;
    std::string m_OutputModelStateFile;
    int m_SimulationError = false;
    bool m_OutputKinematicsFirstTimeFlag = false;
    bool m_DataTargetAbort = false;
    bool m_ContactAbort = false;
    std::vector<std::string> m_DataTargetAbortList;
    std::vector<std::string> m_ContactAbortList;
    int m_numericalErrorCount = 0;

    // for fitness calculations
    double m_TargetMatchFitness = 0;

    // values for energy partition
    double m_PositiveMechanicalWork = 0;
    double m_NegativeMechanicalWork = 0;
    double m_PositiveContractileWork = 0;
    double m_NegativeContractileWork = 0;
    double m_PositiveSerialElasticWork = 0;
    double m_NegativeSerialElasticWork = 0;
    double m_PositiveParallelElasticWork = 0;
    double m_NegativeParallelElasticWork = 0;

    // values for dump output
    std::string m_dumpExtension = {".tab"};
    std::map<std::string, std::ofstream> m_dumpFileStreams;

    // physics engine
    std::unique_ptr<PhysicsEngine> m_physicsEngine;
    std::string m_kinematicsFile;
};

}

#endif //SIMULATION_H
