/*
 *  PlaybackPhysicsEngine.cpp
 *  GaitSym5
 *
 *  Created by Bill Sellers on 20/03/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#include "PlaybackPhysicsEngine.h"
#include "Simulation.h"
#include "Body.h"
#include "Euler.h"
#include "Marker.h"
#include "Joint.h"

#include "pystring.h"

#include <filesystem>
#include <sstream>

namespace GaitSym {

PlaybackPhysicsEngine::PlaybackPhysicsEngine()
{
}

PlaybackPhysicsEngine::~PlaybackPhysicsEngine()
{
}

std::string *PlaybackPhysicsEngine::Initialise(Simulation *theSimulation)
{
    std::string *err = PhysicsEngine::Initialise(theSimulation);
    if (err) { return err; }

    err = ReadSourceFile();
    if (err) { return err; }

    return nullptr;
}

std::string *PlaybackPhysicsEngine::ReadSourceFile()
{
    if (simulation()->kinematicsFile().size() == 0)
    {
        setLastError("PlaybackPhysicsEngine::ReadSourceFile filename not set"s);
        return lastErrorPtr();
    }

    std::error_code ec;
    if (!std::filesystem::is_regular_file(simulation()->kinematicsFile(), ec ))
    {
        setLastError("PlaybackPhysicsEngine::ReadSourceFile \""s + simulation()->kinematicsFile() + "\" is not a file"s);
        return lastErrorPtr();
    }

    std::string *err = ReadOSIMBodyKinematicsFile();
    if (err) return err;

    return nullptr;
}

std::string *PlaybackPhysicsEngine::ReadOSIMBodyKinematicsFile()
{
    std::vector<std::string> columnHeadings;
    std::vector<std::vector<std::string>> data;
    std::vector<std::string> header = {"endheader"s};
    std::string kinematicsFile = simulation()->kinematicsFile();
    readTabDelimitedFile(kinematicsFile, &columnHeadings, &data, &header);
    if (data.size() == 0 || data[0].size() == 0)
    {
        setLastError("PlaybackPhysicsEngine::ReadOSIMBodyKinematicsFile \""s + kinematicsFile + "\" contains no recognisable data"s);
        return lastErrorPtr();
    }
    if (columnHeadings.size() < 2)
    {
        setLastError("PlaybackPhysicsEngine::ReadOSIMBodyKinematicsFile \""s + kinematicsFile + "\" has no body data"s);
        return lastErrorPtr();
    }
    // check the header
    if (header[0] != "Positions"s)
    {
        setLastError("PlaybackPhysicsEngine::ReadOSIMBodyKinematicsFile \""s + kinematicsFile + "\" unrecognised header"s);
        return lastErrorPtr();
    }
    bool inDegrees = true;
    const static std::vector<std::string> falseWords = {"false", "off", "no", "0"};
    for (auto &&line : header)
    {
        auto tokens = pystring::split(line, "="s);
        if (tokens.size() == 2 && tokens[0] == "inDegrees"s && std::find(falseWords.begin(), falseWords.end(), pystring::lower(tokens[1])) != falseWords.end()) inDegrees = false;
    }
    // read the times
    if (columnHeadings[0] != "time"s)
    {
        setLastError("PlaybackPhysicsEngine::ReadOSIMBodyKinematicsFile \""s + kinematicsFile + "\" missing time"s);
        return lastErrorPtr();
    }
    size_t nTimes = data[0].size();
    m_times.clear();
    m_times.reserve(nTimes);
    for (size_t i = 0; i < nTimes; i++)
    {
        m_times.push_back(std::strtod(data[0][i].c_str(), nullptr));
    }
    // create a data map to make lookup a bit quicker
    std::map<std::string, std::vector<double>> dataMap;
    for (size_t j = 1; j < columnHeadings.size(); j++)
    {
        std::vector<double> values;
        values.reserve(nTimes);
        for (size_t i =0; i < nTimes; i++) { values.push_back(std::strtod(data[j][i].c_str(), nullptr)); }
        dataMap[columnHeadings[j]] = std::move(values);
    }
    // now get the pose data
    m_poses.clear();
    // // dependencies are worked out from the joints with body 2 dependent on body 1
    // std::map<std::string, std::string> dependencyMap;
    // std::vector<std::string> dependencyOrder;
    // for (auto &&jointIt : *simulation()->GetJointList()) { dependencyMap[jointIt.second->body2()->name()] = jointIt.second->body1()->name(); }
    // for (auto &&bodyIt : *simulation()->GetBodyList()) { if (dependencyMap.find(bodyIt.first) == dependencyMap.end()) { dependencyMap[bodyIt.first] = std::string(); /* these bodies have no dependencies */ } }
    // while (dependencyOrder.size() < dependencyMap.size())
    // {
    //     for (auto &&bodyIt : dependencyMap)
    //     {
    //         if (std::find(dependencyOrder.begin(), dependencyOrder.end(), bodyIt.first) != dependencyOrder.end()) { continue; } // already processed
    //         if (bodyIt.second.size() == 0 || std::find(dependencyOrder.begin(), dependencyOrder.end(), bodyIt.second) != dependencyOrder.end()) { dependencyOrder.push_back(bodyIt.first); continue; } // dependency satisfied so insert
    //     }
    // }

    pgd::Vector3 euler(1.5707963267948966, 0, 0); // rotating +90 degrees about the X axis converts from Y up to Z up
    pgd::Quaternion rotation = pgd::MakeQFromEulerAnglesRadian(euler.x, euler.y, euler.z);
    std::vector<double> encodings{EulOrdXYZs,EulOrdXYXs,EulOrdXZYs,EulOrdXZXs,EulOrdYZXs,EulOrdYZYs,EulOrdYXZs,EulOrdYXYs,EulOrdZXYs,EulOrdZXZs,EulOrdZYXs,EulOrdZYZs,
                                  EulOrdZYXr,EulOrdXYXr,EulOrdYZXr,EulOrdXZXr,EulOrdXZYr,EulOrdYZYr,EulOrdZXYr,EulOrdYXYr,EulOrdYXZr,EulOrdZXZr,EulOrdXYZr,EulOrdZYZr};
    std::vector<std::string> encodingNames{"EulOrdXYZs","EulOrdXYXs","EulOrdXZYs","EulOrdXZXs","EulOrdYZXs","EulOrdYZYs","EulOrdYXZs","EulOrdYXYs","EulOrdZXYs","EulOrdZXZs","EulOrdZYXs","EulOrdZYZs",
                                           "EulOrdZYXr","EulOrdXYXr","EulOrdYZXr","EulOrdXZXr","EulOrdXZYr","EulOrdYZYr","EulOrdZXYr","EulOrdYXYr","EulOrdYXZr","EulOrdZXZr","EulOrdXYZr","EulOrdZYZr"};
    for (auto &&bodyIt : *simulation()->GetBodyList())
    {
        // check the names are OK
        std::string body = bodyIt.first;
        std::vector<std::string> names = {body + "_X"s, body + "_Y"s, body + "_Z"s, body + "_Ox"s, body + "_Oy"s, body + "_Oz"s};
        for (auto &&name : names)
        {
            if (dataMap.find(name) == dataMap.end())
            {
                setLastError("PlaybackPhysicsEngine::ReadOSIMBodyKinematicsFile \""s + kinematicsFile + "\" missing body label \""s + name + "\"");
                return lastErrorPtr();
            }
        }
        // get the data from the dataMap
        std::vector<double> &x = dataMap[names[0]];
        std::vector<double> &y = dataMap[names[1]];
        std::vector<double> &z = dataMap[names[2]];
        std::vector<double> &ox = dataMap[names[3]];
        std::vector<double> &oy = dataMap[names[4]];
        std::vector<double> &oz = dataMap[names[5]];
        std::vector<Pose> poses;
        poses.reserve(nTimes);
        for (size_t i = 0; i < nTimes; i++)
        {
            Pose p;
            p.p.Set(x[i], y[i], z[i]);
            Euler::EulerAngles ea;
            if (inDegrees) { ea.x = pgd::DegToRad(ox[i]); ea.y = pgd::DegToRad(oy[i]); ea.z = pgd::DegToRad(oz[i]); }
            else { ea.x = ox[i]; ea.y = oy[i]; ea.z = oz[i]; }
            Euler::Quat quat;
            std::ofstream log("c:/Scratch/output.txt");
            for (size_t j = 0; j < encodings.size(); j++)
            {
                ea.w = encodings[j];
                quat = Eul_ToQuat(ea);
                log << encodingNames[j] << " ea.w " << ea.w << " ea.x " << ea.x << " ea.y " << ea.y << " ea.z " << ea.z << " quat " << quat.w << " " << quat.x << " " << quat.y << " " << quat.z << "\n";
            }
            exit(1);
            p.q.Set(quat.w, quat.x, quat.y, quat.z);
            pgd::Quaternion q = pgd::MakeQFromEulerAngles(ox[i], oy[i], oz[i]);
            p.q = q;
            // p.p = pgd::QVRotate(rotation, p.p); // correct for Z up
            // p.q = rotation * p.q; // correct for Z up
            poses.push_back(std::move(p));
        }
        m_poses[body] = std::move(poses);
        // // there must be a parent that has already been calculated

        // std::vector<Pose> &parentPoses = m_poses[parent];
        // std::vector<Pose> poses;
        // poses.reserve(nTimes);
        // for (size_t i =0; i < nTimes; i++)
        // {
        //     Pose p;
        //     p.p.Set(x[i], y[i], z[i]);
        //     pgd::Quaternion parentQ = parentPoses[i].q;
        //     pgd::Vector3 xa, ya, za;
        //     pgd::QGetBasis(parentQ, &xa, &ya, &za);
        //     pgd::Vector3 ea;
        //     if (inDegrees) { ea.x = pgd::DegToRad(ox[i]); ea.y = pgd::DegToRad(oy[i]); ea.z = pgd::DegToRad(oz[i]); }
        //     else { ea.x = ox[i]; ea.y = oy[i]; ea.z = oz[i]; }
        //     pgd::Quaternion childQ = pgd::MakeQFromAxisAngle(za, ea.z) * (pgd::MakeQFromAxisAngle(ya, ea.y) * (pgd::MakeQFromAxisAngle(xa, ea.x)));
        //     p.q = childQ;
        //     poses.push_back(std::move(p));
        // }
        // m_poses[body] = std::move(poses);
    }
    return nullptr;
}

std::string *PlaybackPhysicsEngine::Step()
{
    // start by sorting out the time
    double time = simulation()->GetTime();
    // gaitsym always starts from zero but recorded kinematic data might not
    double playbackTime = m_times[0] + time;
    // now get the index
    size_t index = std::lower_bound(m_times.begin(), m_times.end(), playbackTime) - m_times.begin(); // this is an index for the time >= playbackTime
    // size_t index = std::upper_bound(m_times.begin(), m_times.end(), playbackTime) - m_times.begin(); // this is an index for the time > than playbackTime
    if (index >= m_times.size()) { index = m_times.size() - 1; }

    // update the objects with the new data
    for (auto &&bodyIter : *simulation()->GetBodyList())
    {
        // currently only setting positions
        Pose *pose = &m_poses[bodyIter.first][index];
        bodyIter.second->SetPosition(pose->p);
        bodyIter.second->SetQuaternion(pose->q);
    }

    return nullptr;
}

void PlaybackPhysicsEngine::readLineStructuredFile(const std::string &filename, std::vector<std::string> *lines, std::vector<std::vector<std::string>> *tokensByLine)
{
    lines->clear();
    std::ostringstream buffer;
    try {
        std::ifstream file(filename, std::ios::binary);
        if (!file) return;
        buffer << file.rdbuf();
        file.close();
    }
    catch (...)
    {
        return;
    }
    *lines = pystring::splitlines(buffer.str());
    if (lines->size() == 0) return;
    if (!tokensByLine) return;
    tokensByLine->clear();
    tokensByLine->reserve(lines->size());
    for (auto &&line : *lines)
    {
        tokensByLine->push_back(pystring::split(line));
    }
}

void PlaybackPhysicsEngine::readTabDelimitedFile(const std::string &filename, std::vector<std::string> *columnHeadings, std::vector<std::vector<std::string>> *data, std::vector<std::string> *header)
{
    columnHeadings->clear();
    data->clear();
    std::ostringstream buffer;
    try {
        std::ifstream file(filename, std::ios::binary);
        if (!file) return;
        buffer << file.rdbuf();
        file.close();
    }
    catch (...)
    {
        return;
    }
    std::vector<std::string> lines = pystring::splitlines(buffer.str());
    if (lines.size() == 0) return;
    size_t lineIndex = 0;
    if (header && header->size() > 0) // if available the first line of the header is used as an end of header delimiter
    {
        std::string endOfHeader = pystring::strip(header->at(0));
        header->clear();
        for (auto &&line : lines)
        {
            ++lineIndex;
            if (pystring::strip(line) == endOfHeader) { break; }
            header->push_back(line);
        }
    }
    if (lineIndex >= lines.size()) return;
    *columnHeadings = pystring::split(lines[lineIndex++], "\t");
    if (columnHeadings->size() == 0) return;
    for (size_t i = 0; i < columnHeadings->size(); i++)
    {
        data->push_back(std::vector<std::string>());
    }
    std::vector<std::string> tokens;
    while (lineIndex < lines.size())
    {
        tokens = pystring::split(lines[lineIndex++], "\t");
        if (tokens.size() == 0) continue; // skip blank lines
        for (size_t j = 0; j < columnHeadings->size(); j++)
        {
            if (j < tokens.size()) { (*data)[j].push_back(tokens[j]); }
            else { (*data)[j].push_back(""); } // pad any lines that are incomplete
        }
    }
}


} // namespace GaitSym


