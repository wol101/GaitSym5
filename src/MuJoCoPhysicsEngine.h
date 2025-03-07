/*
 *  MuJoCoPhysicsEngine.h
 *  GaitSym5
 *
 *  Created by Bill Sellers on 20/03/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#ifndef MuJoCoPhysicsEngine_H
#define MuJoCoPhysicsEngine_H

#include "PhysicsEngine.h"

#include "mujoco/mujoco.h"

#include <memory>
#include <set>

namespace GaitSym
{

class Body;
class Joint;
class Geom;

class MuJoCoPhysicsEngine : public PhysicsEngine
{
public:
    MuJoCoPhysicsEngine();
    virtual ~MuJoCoPhysicsEngine();

    virtual std::string *Initialise(Simulation *theSimulation);
    virtual std::string *Step();

    std::string mjXML() const;

private:
    struct TreeBody
    {
        Body *body = nullptr;
        TreeBody *parent = nullptr;
        Joint *jointToParent = nullptr;
        int bodyID = 0;
        std::vector<std::unique_ptr<TreeBody>> childList;
    };

    std::string *CreateConnectedGroups();
    std::string *CreateTree();
    void InsertMJBodyIDs(TreeBody *treeBody);

    std::string *CreateBody(const TreeBody &treeBody);
    std::string *CreateJoint(const Joint *joint);
    std::string *CreateGeom(const Geom *geom);

    std::string *MoveBodies();

    // utility functions
    static void XMLInitiateTag(std::string *xmlString, const std::string &tag, const std::map<std::string, std::string> &attributes = std::map<std::string, std::string>(), bool terminate = false);
    static void XMLTerminateTag(std::string *xmlString, const std::string &tag);

    // Returns a newly-allocated mjModel, loaded from the contents of xml.
    // On failure returns nullptr and populates the error array if present.
    mjModel* LoadModelFromString(const std::string &xml, char* error = nullptr, int error_size = 0, mjVFS* vfs = nullptr);

    // Simulation variables
    // MuJoCo model and data
    mjModel *m_mjModel = nullptr;
    mjData *m_mjData = nullptr;

    std::string m_mjXML;
    std::string m_mjXMLSensors;

    std::vector<TreeBody> m_rootTreeBodyList;
    std::multiset<Body *> m_jointLoopDetector;
    std::vector<Joint *> m_jointsLeftToInclude;
    std::vector<TreeBody *> m_bodiesLeftToInclude;
    std::vector<TreeBody *> m_flatTreeBodyList;

    std::vector<std::unique_ptr<std::map<std::string, Body *>>> m_connectedGroups;

    int m_freeJointCount = 0;
};

}

#endif // MuJoCoPhysicsEngine_H
