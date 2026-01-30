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
    virtual ~OpenSimExporter();

    void process(Simulation *simulation);

    std::string *xmlString();

    std::string pathToObjFiles() const;
    void setPathToObjFiles(const std::string &newPathToObjFiles);

    bool mocoExport() const;
    void setMocoExport(bool newMocoExport);

private:

    void createBodySet();
    void createJointSet();
    void createControllerSet();
    void createConstraintSet();
    void createForceSet();
    void createMarkerSet();
    void createContactGeometrySet();

    void createPathPointSet(std::string name, const std::vector<const Marker *> &markerList);

    // utility functions
    void xmlInitiateTag(std::string *xmlString, const std::string &tag, const std::map<std::string, std::string> &attributes = std::map<std::string, std::string>());
    void xmlTerminateTag(std::string *xmlString, const std::string &tag);
    void xmlTagAndContent(std::string *xmlString, const std::string &tag, const std::string &content);
    void xmlTagAndAttributes(std::string *xmlString, const std::string &tag, const std::map<std::string, std::string> &attributes);

    Simulation *m_simulation = nullptr;
    std::string m_xmlString;
    std::string m_pathToObjFiles;
    std::map<std::string, std::string> m_legalNameMap;
    std::map<std::string, std::string> m_legalNameReverseMap;

    bool m_mocoExport = false;
    int m_currentIndent = 0;
    size_t m_nameLengthHint = 0;
};

} // namespace GaitSym

#endif // OPENSIMEXPORTER_H
