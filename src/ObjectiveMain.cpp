/*
 *  ObjectiveMain.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 24/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#include "ObjectiveMain.h"
#include "GSUtil.h"
#include "DataFile.h"
#include "Simulation.h"
#include "Reporter.h"
#include "DataTarget.h"
#include "Driver.h"
#include "Joint.h"
#include "Muscle.h"
#include "Body.h"
#include "Geom.h"
#include "ArgParse.h"

#include "pocketpy.h"

#define MAX_ARGS 4096

using namespace std::string_literals;

#if defined(USE_CL)
int main(int argc, const char **argv)
{
    py_initialize(); // not bothering with py_finalize() because it can cause problems with py_resetvm in destructors
    GaitSym::ObjectiveMain objectiveMain(argc, argv);
    objectiveMain.run();
}
#endif

namespace GaitSym {

ObjectiveMain::ObjectiveMain(int argc, const char **argv)
{
    std::string compileDate(__DATE__);
    std::string compileTime(__TIME__);
    m_argparse.initialise(argc, argv, "ObjectiveMain command line interface to GaitSym2019 build "s + compileDate + " "s + compileTime, 0, 0);
    m_argparse.addArgument("-sc"s, "--score"s, "Score filename"s, ""s, 1, false, ArgParse::String);
    m_argparse.addArgument("-co"s, "--config"s, "Config filename"s, ""s, 1, true, ArgParse::String);
    m_argparse.addArgument("-ms"s, "--modelState"s, "Model state filename"s, ""s, 1, false, ArgParse::String);
    m_argparse.addArgument("-rt"s, "--runTimeLimit"s, "Run time limit"s, ""s, 1, false, ArgParse::Double);
    m_argparse.addArgument("-st"s, "--simulationTimeLimit"s, "Simulation time limit"s, ""s, 1, false, ArgParse::Double);
    m_argparse.addArgument("-mc"s, "--outputModelStateAtCycle"s, "Output model state at this cycle"s, ""s, 1, false, ArgParse::Double);
    m_argparse.addArgument("-mt"s, "--outputModelStateAtTime"s, "Output model state at this cycle"s, ""s, 1, false, ArgParse::Double);
    m_argparse.addArgument("-de"s, "--debug"s, "Turn debugging on"s);

    m_argparse.addArgument("-ol"s, "--outputList"s, "List of objects to produce output"s, ""s, 1, MAX_ARGS, false, ArgParse::String);

    int err = m_argparse.parse();
    if (err)
    {
        m_argparse.usage();
        exit(1);
    }

    m_argparse.get("--outputList"s, &m_outputList);
    m_argparse.get("--runTimeLimit"s, &m_runTimeLimit);
    m_argparse.get("--outputModelStateAtTime"s, &m_outputModelStateAtTime);
    m_argparse.get("--outputModelStateAtCycle"s, &m_outputModelStateAtCycle);
    m_argparse.get("--simulationTimeLimit"s, &m_simulationTimeLimit);
    m_argparse.get("--config"s, &m_configFilename);
    m_argparse.get("--score"s, &m_scoreFilename);
    m_argparse.get("--modelState"s, &m_outputModelStateFilename);
    m_argparse.get("--debug"s, &m_debug);
}

int ObjectiveMain::run()
{
    if (readModel()) return __LINE__;

    for (size_t i = 0; i < m_outputList.size(); i++)
    {
        if (m_simulation->bodyList()->find(m_outputList[i]) != m_simulation->bodyList()->end()) (*m_simulation->bodyList())[m_outputList[i]]->setDump(true);
        if (m_simulation->muscleList()->find(m_outputList[i]) != m_simulation->muscleList()->end()) (*m_simulation->muscleList())[m_outputList[i]]->setDump(true);
        if (m_simulation->strapList()->find(m_outputList[i]) != m_simulation->strapList()->end()) (*m_simulation->strapList())[m_outputList[i]]->setDump(true);
        if (m_simulation->geomList()->find(m_outputList[i]) != m_simulation->geomList()->end()) (*m_simulation->geomList())[m_outputList[i]]->setDump(true);
        if (m_simulation->jointList()->find(m_outputList[i]) != m_simulation->jointList()->end()) (*m_simulation->jointList())[m_outputList[i]]->setDump(true);
        if (m_simulation->driverList()->find(m_outputList[i]) != m_simulation->driverList()->end()) (*m_simulation->driverList())[m_outputList[i]]->setDump(true);
        if (m_simulation->dataTargetList()->find(m_outputList[i]) != m_simulation->dataTargetList()->end()) (*m_simulation->dataTargetList())[m_outputList[i]]->setDump(true);
        if (m_simulation->reporterList()->find(m_outputList[i]) != m_simulation->reporterList()->end()) (*m_simulation->reporterList())[m_outputList[i]]->setDump(true);
    }

    double startTime = GSUtil::systemTime();

    while(m_runTimeLimit <= 0 || m_simulationTime <= m_runTimeLimit)
    {
        m_simulationTime = GSUtil::systemTime() - startTime;
        if (m_simulation->shouldQuit()) break;
        if (m_simulation->testForCatastrophy()) break;
        m_simulation->updateSimulation();
    }

    if (writeOutput()) return __LINE__;

    return 0;
}

// this routine attemps to read the model specification and initialise the simulation
// it returns zero on success
int ObjectiveMain::readModel()
{
    DataFile myFile;
    myFile.setExitOnErrorFlag(true);

    if (m_debug) std::cerr << "Reading file \"" << m_configFilename << "\"\n";
    myFile.readFile(m_configFilename);
    if (m_debug) std::cerr << "Read " << myFile.size() << " bytes\n";

    // create the simulation object
    m_simulation = std::make_unique<Simulation>();
    if (m_outputModelStateFilename.size()) m_simulation->setOutputModelStateFile(m_outputModelStateFilename);
    if (m_outputModelStateAtTime >= 0) m_simulation->setOutputModelStateAtTime(m_outputModelStateAtTime);
    if (m_outputModelStateAtCycle >= 0) m_simulation->setOutputModelStateAtCycle(m_outputModelStateAtCycle);

    if (m_debug) std::cerr << "Loading model\n";
    if (m_simulation->loadModel(myFile.rawData(), myFile.size()))
    {
        m_simulation.reset();
        return 1;
    }
    if (m_debug) std::cerr << "Success\n";

    // late initialisation options
    if (m_simulationTimeLimit >= 0) m_simulation->global()->setTimeLimit(m_simulationTimeLimit);

    return 0;
}

// returns 0 if continuing
// returns 1 if exit requested
int ObjectiveMain::writeOutput()
{
    double score = m_simulation->calculateInstantaneousFitness();
    std::cerr << "Simulation Time: " << m_simulation->simulationTime() <<
                 " Steps: " << m_simulation->stepCount() <<
                 " Score: " << score <<
                 " Mechanical Energy: " << m_simulation->mechanicalEnergy() <<
                 " Metabolic Energy: " << m_simulation->metabolicEnergy() <<
                 " CPUTimeSimulation: " << m_simulationTime <<
                 "\n";

    if (m_scoreFilename.size())
    {
        DataFile myFile;
        myFile.setExitOnErrorFlag(false);
        myFile.setRawData(reinterpret_cast<char *>(&score), sizeof(double));
        if (myFile.writeFile(m_scoreFilename)) return __LINE__;
    }

    return 0;
}



} // namespace GaitSym


