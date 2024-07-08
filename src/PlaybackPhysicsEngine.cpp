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
    if (m_sourceFile.size() == 0)
    {
        setLastError("PlaybackPhysicsEngine::ReadSourceFile filename not set"s);
        return lastErrorPtr();
    }

    std::error_code ec;
    if (!std::filesystem::is_regular_file(m_sourceFile, ec ))
    {
        setLastError("PlaybackPhysicsEngine::ReadSourceFile \""s + m_sourceFile + "\" is not a file"s);
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
    readTabDelimitedFile(m_sourceFile, &columnHeadings, &data, &header);
    if (data.size() == 0 || data[0].size() == 0)
    {
        setLastError("PlaybackPhysicsEngine::ReadOSIMBodyKinematicsFile \""s + m_sourceFile + "\" contains no recognisable data"s);
        return lastErrorPtr();
    }
    if (columnHeadings.size() < 2)
    {
        setLastError("PlaybackPhysicsEngine::ReadOSIMBodyKinematicsFile \""s + m_sourceFile + "\" has no body data"s);
        return lastErrorPtr();
    }
    // check the header
    if (header[0] != "Positions"s)
    {
        setLastError("PlaybackPhysicsEngine::ReadOSIMBodyKinematicsFile \""s + m_sourceFile + "\" unrecognised header"s);
        return lastErrorPtr();
    }
    bool inDegrees = true;
    const static std::vector<std::string> falseWords = {"false", "off", "no", "0"};
    for (auto &&line : header)
    {
        auto tokens = pystring::split(line, "="s);
        if (tokens.size() == 2 && tokens[0] == "inDegrees"s && std::find(falseWords.begin(), falseWords.end(), pystring::lower(line)) != falseWords.end()) inDegrees = false;
    }
    // read the times
    if (columnHeadings[0] != "time"s)
    {
        setLastError("PlaybackPhysicsEngine::ReadOSIMBodyKinematicsFile \""s + m_sourceFile + "\" missing time"s);
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
    for (auto &&body : *simulation()->GetBodyList())
    {
        std::vector<Pose> poses;
        poses.reserve(nTimes);
        for (size_t i =0; i < nTimes; i++)
        {
            std::vector<std::string> names = {body.first + "_X"s, body.first + "_Y"s, body.first + "_Z"s, body.first + "_Ox"s, body.first + "_Oy"s, body.first + "_Oz"s};
            for (auto &&name : names)
            {
                if (dataMap.find(name) == dataMap.end())
                {
                    setLastError("PlaybackPhysicsEngine::ReadOSIMBodyKinematicsFile \""s + m_sourceFile + "\" missing body label \""s + name + "\"");
                    return lastErrorPtr();
                }
            }
            Pose p;
            p.p.Set(dataMap[names[0]][i], dataMap[names[1]][i], dataMap[names[2]][i]);
            if (inDegrees) p.q = pgd::MakeQFromEulerAngles(dataMap[names[3]][i], dataMap[names[4]][i], dataMap[names[5]][i]);
            else p.q = pgd::MakeQFromEulerAnglesRadian(dataMap[names[3]][i], dataMap[names[4]][i], dataMap[names[5]][i]);
            poses.push_back(std::move(p));
        }
        m_poses[body.first] = std::move(poses);
    }

    return nullptr;
}

std::string *PlaybackPhysicsEngine::Step()
{

    // update the objects with the new data
    for (auto &&body : *simulation()->GetBodyList())
    {
    }

    return nullptr;
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

std::string PlaybackPhysicsEngine::sourceFile() const
{
    return m_sourceFile;
}

void PlaybackPhysicsEngine::setSourceFile(const std::string &newSourceFile)
{
    m_sourceFile = newSourceFile;
}


} // namespace GaitSym


