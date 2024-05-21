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

namespace GaitSym {

class Body;
class Joint;
class Simulation;
class Geom;
class Marker;

class OpenSimExporter : public NamedObject
{
public:
    OpenSimExporter();

    void Process(Simulation *simulation);

    std::string *xmlString();

    std::string pathToObjFiles() const;
    void setPathToObjFiles(const std::string &newPathToObjFiles);

private:

    void CreateBodySet();
    void CreateJointSet();
    void CreateControllerSet();
    void CreateConstraintSet();
    void CreateForceSet();
    void CreateMarkerSet();
    void CreateContactGeometrySet();

    void CreatePathPointSet(std::string name, const std::vector<const Marker *> &markerList);

    // utility functions
    static void XMLInitiateTag(std::string *xmlString, const std::string &tag, const std::map<std::string, std::string> &attributes = std::map<std::string, std::string>(), bool terminate = false);
    static void XMLTerminateTag(std::string *xmlString, const std::string &tag);
    static void XMLTagAndContent(std::string *xmlString, const std::string &tag, const std::string &content);

    Simulation *m_simulation = nullptr;
    std::string m_xmlString;
    std::string m_pathToObjFiles;
};

} // namespace GaitSym

#endif // OPENSIMEXPORTER_H
