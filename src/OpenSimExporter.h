/*
 *  OpenSimExporter.h
 *  GaitSym5
 *
 *  Created by Bill Sellers on 20/05/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#ifndef OPENSIMEXPORTER_H
#define OPENSIMEXPORTER_H

#include "NamedObject.h"

#include <memory>
#include <set>

namespace GaitSym {

class Body;
class Joint;
class Simulation;
class Geom;

class OpenSimExporter : public NamedObject
{
public:
    OpenSimExporter();

    std::string *Process(Simulation *simulation);

private:
    struct TreeBody
    {
        Body *body = nullptr;
        TreeBody *parent = nullptr;
        Joint *jointToParent = nullptr;
        std::vector<std::unique_ptr<TreeBody>> childList;
    };

    std::string *CreateConnectedGroups();
    std::string *CreateTree();

    std::string *CreateBody(const TreeBody &treeBody);
    std::string *CreateJoint(const Joint *joint);
    std::string *CreateGeom(const Geom *geom);

    // utility functions
    static void XMLInitiateTag(std::string *xmlString, const std::string &tag, const std::map<std::string, std::string> &attributes = std::map<std::string, std::string>(), bool terminate = false);
    static void XMLTerminateTag(std::string *xmlString, const std::string &tag);

    Simulation *m_simulation = nullptr;

    std::vector<TreeBody> m_rootTreeBodyList;
    std::multiset<Body *> m_jointLoopDetector;
    std::vector<Joint *> m_jointsLeftToInclude;
    std::vector<TreeBody *> m_bodiesLeftToInclude;
    std::vector<TreeBody *> m_flatTreeBodyList;
    std::vector<std::unique_ptr<std::map<std::string, Body *>>> m_connectedGroups;
    int m_freeJointCount = 0;
    std::string m_xmlString;
};

} // namespace GaitSym

#endif // OPENSIMEXPORTER_H
