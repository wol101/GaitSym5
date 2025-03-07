/*
 *  PlaybackPhysicsEngine.h
 *  GaitSym5
 *
 *  Created by Bill Sellers on 20/03/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#ifndef PLAYBACKPHYSICSENGINE_H
#define PLAYBACKPHYSICSENGINE_H

#include "PhysicsEngine.h"
#include "PGDMath.h"

namespace GaitSym
{

class Simulation;

class PlaybackPhysicsEngine : public PhysicsEngine
{
public:
    PlaybackPhysicsEngine();
    virtual ~PlaybackPhysicsEngine();

    virtual std::string *Initialise(Simulation *simulation);
    virtual std::string *Step();

private:
    std::string *ReadSourceFile();
    std::string *ReadOSIMBodyKinematicsFile();

    static void readLineStructuredFile(const std::string &filename, std::vector<std::string> *lines, std::vector<std::vector<std::string>> *tokensByLine = nullptr);
    static void readTabDelimitedFile(const std::string &filename, std::vector<std::string> *columnHeadings, std::vector<std::vector<std::string>> *data, std::vector<std::string> *header = nullptr);

    struct Pose
    {
        pgd::Vector3 p;
        pgd::Quaternion q;
    };

    std::vector<double> m_times;
    std::map<std::string, std::vector<Pose>> m_poses;
};

}

#endif // PLAYBACKPHYSICSENGINE_H
