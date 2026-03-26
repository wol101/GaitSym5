/*
 *  ObjectiveMainASIOAsync.cpp
 *  GaitSym2019
 *
 *  Created by Bill Sellers on 24/12/2019.
 *  Copyright 2019 Bill Sellers. All rights reserved.
 *
 */

#include "ObjectiveMainASIOAsync.h"
#include "GSUtil.h"
#include "Simulation.h"
#include "Reporter.h"
#include "DataTarget.h"
#include "Driver.h"
#include "Joint.h"
#include "Muscle.h"
#include "Body.h"
#include "Geom.h"
#include "ArgParse.h"

#include "pystring.h"

#include <chrono>
#include <thread>
#include <algorithm>
#include <random>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <WinSock2.h>
#else
#include <netdb.h>
#endif

#define MAX_ARGS 4096

using namespace std::string_literals;

#if defined(USE_ASIO_ASYNC)
#include "pocketpy.h"
int main(int argc, const char **argv)
{
    py_initialize();
    GaitSym::ObjectiveMainASIOAsync objectiveMain(argc, argv);
    objectiveMain.run();
    py_finalize();
    return 0;
}
#endif

#if defined(USE_ASIO_ASYNC_MPI)
#include "pocketpy.h"
#include <mpi.h>
int main(int argc, const char **argv)
{
    int debug = 0;
    int mpiCommSize = -1;
    int mpiCommRank = -1;
    int rc = MPI_Init(&argc, &argv);
    if (debug > 0) std::cerr << "MPI_Init " << rc << "\n";
    rc = MPI_Comm_size(MPI_COMM_WORLD, &mpiCommSize); // this is the total number of MPI instances being run
    rc = MPI_Comm_rank(MPI_COMM_WORLD, &mpiCommRank); // this is the rank number of the current instance [0 to mpiCommSize-1] with 0 often used to run some sort of server process
    if (debug > 0) std::cout << "Size " << mpiCommSize << " Rank " << mpiCommRank << "\n";

    py_initialize();
    GaitSym::ObjectiveMainASIOAsync objectiveMain(argc, argv);
    objectiveMain.run();
    py_finalize();

    rc = MPI_Finalize();
    if (debug > 0) std::cerr << "MPI_Finalize " << rc << "\n";
    return 0;
}
#endif


namespace GaitSym {

ObjectiveMainASIOAsync::ObjectiveMainASIOAsync(int argc, const char **argv)
{
    std::string compileDate(__DATE__);
    std::string compileTime(__TIME__);
    m_argparse.initialise(argc, argv, "ObjectiveMainASIOAsync command line interface to GaitSym2019 build "s + compileDate + " "s + compileTime, 0, 0);
    m_argparse.addArgument("-sc"s, "--score"s, "Score filename"s, ""s, 1, false, ArgParse::String);
    m_argparse.addArgument("-ms"s, "--modelState"s, "Model state filename"s, ""s, 1, false, ArgParse::String);
    m_argparse.addArgument("-rt"s, "--runTimeLimit"s, "Run time limit"s, ""s, 1, false, ArgParse::Double);
    m_argparse.addArgument("-st"s, "--simulationTimeLimit"s, "Simulation time limit"s, ""s, 1, false, ArgParse::Double);
    m_argparse.addArgument("-mc"s, "--outputModelStateAtCycle"s, "Output model state at this cycle"s, ""s, 1, false, ArgParse::Double);
    m_argparse.addArgument("-mt"s, "--outputModelStateAtTime"s, "Output model state at this cycle"s, ""s, 1, false, ArgParse::Double);
    m_argparse.addArgument("-de"s, "--debug"s, "Turn debugging on"s);
    m_argparse.addArgument("-qu"s, "--quiet"s, "Turn quiet mode on"s);

    m_argparse.addArgument("-ol"s, "--outputList"s, "List of objects to produce output"s, ""s, 1, MAX_ARGS, false, ArgParse::String);

    m_argparse.addArgument("-ho"s, "--host"s, "Host and port"s, "127.0.0.1:8086"s, 1, true, ArgParse::String);

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
    m_argparse.get("--quiet"s, &m_quiet);

    std::string rawHost;
    std::vector<std::string> result;
    m_argparse.get("--host"s, &rawHost);
    pystring::split(rawHost, result, ":"s);
    if (result.size() == 2)
    {
        m_host = result[0];
        m_port = uint16_t(GSUtil::toInt(result[1]));
    }
    else
    {
        std::cerr << "Error parsing host\n";
        exit(1);
    }


    // complicated stuff for the random number generator
    std::random_device rd;
    std::mt19937_64::result_type seed = rd();
    m_gen = std::mt19937_64(seed);
    m_distrib = std::uniform_real_distribution<double>(0.5, 1.5);
}

int ObjectiveMainASIOAsync::run()
{
    double startTime = GSUtil::systemTime();
    double runTime = 0;
    double computeTime = 0;
    int status = 0;
    while(m_runTimeLimit == 0 || runTime <= m_runTimeLimit)
    {
        // construct the new thread and run it
        double score = 0;
        uint32_t runID = std::numeric_limits<uint32_t>::max() - 1;
        uint64_t evolveIdentifier = 0;
        std::string xmlCopy;
        if (m_lastGenomeValid && m_XMLConverter.baseXMLString().size())
        {
            runID = reinterpret_cast<const DataMessage *>(m_lastGenomeDataMessageRaw.data())->runID;
            evolveIdentifier = reinterpret_cast<const DataMessage *>(m_lastGenomeDataMessageRaw.data())->evolveIdentifier;
            if (m_debug) std::cerr <<  "Run runID = " << runID << " evolveIdentifier = " << evolveIdentifier << "\n";
            std::vector<double> data(reinterpret_cast<const DataMessage *>(m_lastGenomeDataMessageRaw.data())->payload.genome, reinterpret_cast<const DataMessage *>(m_lastGenomeDataMessageRaw.data())->payload.genome + reinterpret_cast<const DataMessage *>(m_lastGenomeDataMessageRaw.data())->genomeLength);
            m_XMLConverter.applyGenome(data);
            m_XMLConverter.getFormattedXML(&xmlCopy);
        }
        m_statusDoSimulation = __LINE__;
        std::thread simulationThread(&ObjectiveMainASIOAsync::doSimulation, this, xmlCopy.data(), xmlCopy.size(), &score, &computeTime);

        // while the simulation is running send off the last result and get the new task
        if (m_scoreToSend)
        {
            status = writeOutput(m_host, m_port, m_lastEvolveIdentifier, m_lastRunID, m_lastScore);
            if (status && m_debug) std::cerr << "Failed to write output score\n";
            m_scoreToSend = false;
        }
        status = readGenome(m_host, m_port, &m_lastGenomeDataMessageRaw);
        if (status) m_lastGenomeValid = false;
        else m_lastGenomeValid = true;
        if (m_lastGenomeValid)
        {
            if (!hashEqual(m_hash.data(), reinterpret_cast<const DataMessage *>(m_lastGenomeDataMessageRaw.data())->md5, m_hash.size()))
            {
                std::string rawMessage;
                readXML(m_host, m_port, &rawMessage);
                if (hashEqual(reinterpret_cast<const DataMessage *>(rawMessage.data())->md5, reinterpret_cast<const DataMessage *>(m_lastGenomeDataMessageRaw.data())->md5, m_hash.size())
                        && reinterpret_cast<const DataMessage *>(rawMessage.data())->evolveIdentifier == reinterpret_cast<const DataMessage *>(m_lastGenomeDataMessageRaw.data())->evolveIdentifier)
                {
                    for (size_t i = 0; i < m_hash.size(); i++) { m_hash[i] = reinterpret_cast<const DataMessage *>(rawMessage.data())->md5[i]; }
                    m_XMLConverter.loadBaseXMLString(reinterpret_cast<const DataMessage *>(rawMessage.data())->payload.xml, reinterpret_cast<const DataMessage *>(rawMessage.data())->xmlLength);
                }
                else
                {
                    m_lastGenomeValid = false;
                }
            }
        }

        // wait for the simulation thread
        simulationThread.join();
        if (m_statusDoSimulation == 0)
        {
            m_scoreToSend = true;
            m_lastScore = score;
            m_lastRunID = runID;
            m_lastEvolveIdentifier = evolveIdentifier;
        }
        else
        {
            m_scoreToSend = false;
            m_lastScore = 0;
            m_lastRunID = std::numeric_limits<uint32_t>::max() - 1;;
            m_lastEvolveIdentifier = 0;
        }

        runTime = GSUtil::systemTime() - startTime;
        double housekeeping = runTime - computeTime;
        double utilisation = computeTime / runTime;
        if (!m_quiet) std::cerr << "runTime: " << runTime << " computeTime: " << computeTime << " housekeeping: " << housekeeping << " utilisation: " << utilisation * 100.0 << "%\n";
    }
    return 0;
}

void ObjectiveMainASIOAsync::doSimulation(const char *xmlPtr, size_t xmlLen, double *score, double *computeTime)
{
    if (xmlLen == 0)
    {
        m_statusDoSimulation = __LINE__;
        return;
    }
    m_statusDoSimulation = 0;

    double startTime = GSUtil::systemTime();

    // std::ofstream of("c:/scratch/output.log");
    // of << xmlPtr;
    // exit(1);

    // create the simulation object locally so delete happens before the next one is create otherwise we get problems with ODE error tracking
    std::unique_ptr<Simulation> simulation = std::make_unique<Simulation>();
    simulation->setQuiet(m_quiet);
    if (m_outputModelStateFilename.size()) simulation->setOutputModelStateFile(m_outputModelStateFilename);
    if (m_outputModelStateAtTime >= 0) simulation->setOutputModelStateAtTime(m_outputModelStateAtTime);
    if (m_outputModelStateAtCycle >= 0) simulation->setOutputModelStateAtCycle(m_outputModelStateAtCycle);

    if (simulation->loadModel(xmlPtr, xmlLen))
    {
        m_statusDoSimulation = __LINE__;
        return;
    }

    // late initialisation options
    if (m_simulationTimeLimit >= 0) simulation->setTimeLimit(m_simulationTimeLimit);
    for (size_t i = 0; i < m_outputList.size(); i++)
    {
        if (simulation->bodyList()->find(m_outputList[i]) != simulation->bodyList()->end()) (*simulation->bodyList())[m_outputList[i]]->setDump(true);
        if (simulation->muscleList()->find(m_outputList[i]) != simulation->muscleList()->end()) (*simulation->muscleList())[m_outputList[i]]->setDump(true);
        if (simulation->geomList()->find(m_outputList[i]) != simulation->geomList()->end()) (*simulation->geomList())[m_outputList[i]]->setDump(true);
        if (simulation->jointList()->find(m_outputList[i]) != simulation->jointList()->end()) (*simulation->jointList())[m_outputList[i]]->setDump(true);
        if (simulation->driverList()->find(m_outputList[i]) != simulation->driverList()->end()) (*simulation->driverList())[m_outputList[i]]->setDump(true);
        if (simulation->dataTargetList()->find(m_outputList[i]) != simulation->dataTargetList()->end()) (*simulation->dataTargetList())[m_outputList[i]]->setDump(true);
        if (simulation->reporterList()->find(m_outputList[i]) != simulation->reporterList()->end()) (*simulation->reporterList())[m_outputList[i]]->setDump(true);
    }

    while (simulation->shouldQuit() == false)
    {
        simulation->updateSimulation();
        if (simulation->testForCatastrophy()) break;
    }
    *score = simulation->calculateInstantaneousFitness();
    if (!m_quiet) std::cerr << "Simulation Time: " << simulation->simulationTime() << " Steps: " << simulation->stepCount() << " Score: " << *score <<
            " Mechanical Energy: " << simulation->mechanicalEnergy() << " Metabolic Energy: " << simulation->metabolicEnergy() << "\n";
    *computeTime += (GSUtil::systemTime() - startTime);
}

// this routine attemps to read the model specification and initialise the simulation
// it returns zero on success
int ObjectiveMainASIOAsync::readGenome(std::string host, uint16_t port, std::string *rawMessage)
{
    if (m_debug) std::cerr <<  "ReadGenome  host " << host << " port " << port << "\n";

    m_timeout = std::chrono::milliseconds(int(10000 * m_distrib(m_gen)));
    try
    {
        m_asioClient.connect(host, std::to_string(port), m_timeout);
    }
    catch (std::exception& e)
    {
        if (!m_quiet) std::cerr << __LINE__ << " " << e.what() << std::endl;
        return __LINE__;
    }
    if (m_debug) std::cerr <<  "ReadGenome m_asioClient.connect() OK\n";

    // request info from the server
    RequestMessage m_requestMessage = {};
    m_requestMessage.senderIP = m_asioClient.socket().local_endpoint().address().to_v4().to_uint();
    m_requestMessage.senderPort = m_asioClient.socket().local_endpoint().port();
    strncpy(m_requestMessage.text, "req_gen_", sizeof(m_requestMessage.text));
    try
    {
        std::string encodedLine = encode(std::string(reinterpret_cast<char *>(&m_requestMessage), sizeof(RequestMessage)));
        m_asioClient.writeBuffer(encodedLine.data(), encodedLine.size(), m_timeout);
    }
    catch (std::exception& e)
    {
        if (!m_quiet) std::cerr << __LINE__ << " " << e.what() << std::endl;
        return __LINE__;
    }
    if (m_debug) std::cerr <<  "ReadGenome req_gen_ sent\n";

    std::string reply;
    try
    {
        reply =  decode(m_asioClient.readLine(m_timeout, '\0'));
    }
    catch (std::exception& e)
    {
        if (!m_quiet) std::cerr << __LINE__ << " " << e.what() << std::endl;
        return __LINE__;
    }
    if (m_debug) std::cerr << "ReadGenome genome received " << reply.size() << " characters\n";
    if (reply.size() < sizeof(DataMessage))
    {
        if (!m_quiet) std::cerr << "ReadGenome reply.size() < sizeof(DataMessage)\n";
        return __LINE__;
    }
    if (strncmp(reply.data(), "genome", 16) != 0)
    {
        if (!m_quiet) std::cerr << "ReadGenome strncmp(reply.data(), \"genome\", 16) != 0\n";
        return __LINE__;
    }
    const DataMessage *dataMessagePtr = reinterpret_cast<const DataMessage *>(reply.data());
    if (m_debug) std::cerr << "ReadGenome " << dataMessagePtr->text << " received\n"
                           << "senderIP = " << dataMessagePtr->senderIP
                           << " senderPort = " << dataMessagePtr->senderPort
                           << " evolveIdentifier = " << dataMessagePtr->evolveIdentifier
                           << " runID = " << dataMessagePtr->runID
                           << " genomeLength = " << dataMessagePtr->genomeLength
                           << " xmlLength = " << dataMessagePtr->xmlLength
                           << " md5 = " << dataMessagePtr->md5[0] << " " << dataMessagePtr->md5[1] << " "
                           << dataMessagePtr->md5[2] << " " << dataMessagePtr->md5[3] << "\n";
    if (reply.size() < sizeof(DataMessage) + dataMessagePtr->genomeLength * sizeof(double))
    {
        if (!m_quiet) std::cerr << "ReadGenome reply.size() < sizeof(DataMessage) + dataMessagePtr->genomeLength * sizeof(double)\n";
        return __LINE__;
    }
    *rawMessage = reply;
    return 0;
}

int ObjectiveMainASIOAsync::readXML(std::string host, uint16_t port, std::string *rawMessage)
{
    if (m_debug) std::cerr <<  "ReadXML host " << host << " port " << port << "\n";

    m_timeout = std::chrono::milliseconds(int(10000 * m_distrib(m_gen)));
    try
    {
        m_asioClient.connect(host, std::to_string(port), m_timeout);
    }
    catch (std::exception& e)
    {
        if (!m_quiet) std::cerr << __LINE__ << " " << e.what() << std::endl;
        return __LINE__;
    }
    if (m_debug) std::cerr <<  "ReadXML m_asioClient.connect() OK\n";

    RequestMessage requestMessage = {};
    requestMessage.senderIP = m_asioClient.socket().local_endpoint().address().to_v4().to_uint();
    requestMessage.senderPort = m_asioClient.socket().local_endpoint().port();
    strncpy(requestMessage.text, "req_xml_", sizeof(requestMessage.text));
    try
    {
        std::string encodedLine = encode(std::string(reinterpret_cast<char *>(&requestMessage), sizeof(RequestMessage)));
        m_asioClient.writeBuffer(encodedLine.data(), encodedLine.size(), m_timeout);
    }
    catch (std::exception& e)
    {
        if (!m_quiet) std::cerr << __LINE__ << " " << e.what() << std::endl;
        return __LINE__;
    }
    if (m_debug) std::cerr << "ReadXML req_xml_ sent\n";

    std::string reply;
    try
    {
        reply =  decode(m_asioClient.readLine(m_timeout, '\0'));
    }
    catch (std::exception& e)
    {
        if (!m_quiet) std::cerr << __LINE__ << " " << e.what() << std::endl;
        return __LINE__;
    }
    if (m_debug) std::cerr << "ReadXML xml received " << reply.size() << " characters\n";
    if (reply.size() < sizeof(DataMessage))
    {
        if (!m_quiet) std::cerr << "ReadXML reply.size() < sizeof(DataMessage)\n";
        return __LINE__;
    }
    const DataMessage *dataMessagePtr = reinterpret_cast<const DataMessage *>(reply.data());
    if (reply.size() < sizeof(DataMessage) + dataMessagePtr->xmlLength * sizeof(char))
    {
        if (!m_quiet) std::cerr << "ReadXML reply.size() < sizeof(DataMessage) + dataMessagePtr->xmlLength * sizeof(char)\n";
        return __LINE__;
    }
    if (strncmp(reply.data(), "xml", 16) != 0)
    {
        if (!m_quiet) std::cerr << "ReadXML strncmp(reply.data(), \"xml\", 16) != 0\n";
        return __LINE__;
    }

    *rawMessage = reply;
    return 0;
}

// returns 0 if continuing
// returns 1 if exit requested
int ObjectiveMainASIOAsync::writeOutput(std::string host, uint16_t port, uint64_t evolveIdentifier, uint32_t runID, double score)
{
    m_timeout = std::chrono::milliseconds(int(100000 * m_distrib(m_gen)));
    try
    {
        m_asioClient.connect(host, std::to_string(port), m_timeout);
    }
    catch (std::exception& e)
    {
        if (!m_quiet) std::cerr << __LINE__ << " " << e.what() << std::endl;
        return __LINE__;
    }
    if (m_debug) std::cerr <<  "WriteOutput m_asioClient.connect() OK\n";

    RequestMessage requestMessage = {};
    requestMessage.senderIP = m_asioClient.socket().local_endpoint().address().to_v4().to_uint();
    requestMessage.senderPort = m_asioClient.socket().local_endpoint().port();
    strncpy(requestMessage.text, "score___", sizeof(requestMessage.text));
    requestMessage.score = score;
    requestMessage.runID = runID;
    requestMessage.evolveIdentifier = evolveIdentifier;
    try
    {
        std::string encodedString = encode(std::string(reinterpret_cast<char *>(&requestMessage), sizeof(RequestMessage)));
        m_asioClient.writeBuffer(encodedString.data(), encodedString.size(), m_timeout);
    }
    catch (std::exception& e)
    {
        if (!m_quiet) std::cerr << __LINE__ << " " << e.what() << std::endl;
        return __LINE__;
    }
    if (m_debug) std::cerr << "WriteOutput score = " << score << " runID = " << runID << " sent\n";

    return 0;
}

std::string ObjectiveMainASIOAsync::encode(const std::string &input)
{
    std::string output;
    output.reserve(input.size() * 2 + 1);
    for (size_t i = 0; i < input.size(); i++)
    {
        if (input[i] == '\0')
        {
            output.push_back('\xff');
            output.push_back('\x1');
            continue;
        }
        if (input[i] == '\xff')
        {
            output.push_back('\xff');
            output.push_back('\x2');
            continue;
        }
        output.push_back(input[i]);
    }
    output.push_back('\0');
    return output;
}

std::string ObjectiveMainASIOAsync::decode(const std::string &input)
{
    std::string output;
    output.reserve(input.size());
    const char *ptr = input.data();
    while (*ptr)
    {
        if (*ptr != '\xff')
        {
            output.push_back(*ptr);
            ptr++;
            continue;
        }
        ptr++;
        if (*ptr)
        {
            if (*ptr == '\x1')
            {
                output.push_back('\0');
                ptr++;
                continue;
            }
            if (*ptr == '\x2')
            {
                output.push_back('\xff');
                ptr++;
                continue;
            }
        }
    }
    return output;
}

bool ObjectiveMainASIOAsync::hashEqual(const uint32_t *hash1, const uint32_t *hash2, size_t hashSize)
{
    for (size_t i = 0; i < hashSize; i++)
    {
        if (hash1[i] != hash2[i]) return false;
    }
    return true;
}

} // namespace GaitSym
