/*
 *  OpenSimExporter.cpp
 *  GaitSym5
 *
 *  Created by Bill Sellers on 20/05/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#include "OpenSimExporter.h"

#include "GSUtil.h"
#include "Geom.h"
#include "Simulation.h"
#include "Body.h"
#include "Joint.h"
#include "HingeJoint.h"
#include "Marker.h"
#include "SphereGeom.h"
#include "PlaneGeom.h"

#include "pystring.h"

namespace GaitSym {

OpenSimExporter::OpenSimExporter()
{
    // we do not want an empty name
    setName("GaitSym5_generated_model"s);
}

void OpenSimExporter::Process(Simulation *simulation)
{
    m_simulation = simulation;
    m_xmlString.clear();

    // start building the XML
    XMLInitiateTag(&m_xmlString, "OpenSimDocument"s, {{"Version"s, "40000"s}});
    XMLInitiateTag(&m_xmlString, "Model"s, {{"name"s, name()}});

    // components
    XMLTagAndContent(&m_xmlString, "components"s, ""s);

    // ground
    XMLInitiateTag(&m_xmlString, "Ground"s, {{"name"s, "ground"s}});
    XMLInitiateTag(&m_xmlString, "FrameGeometry"s, {{"name"s, "frame_geometry"s}});
    XMLTagAndContent(&m_xmlString, "socket_frame"s, ".."s);
    XMLTagAndContent(&m_xmlString, "scale_factors"s, "1 1 1"s);
    XMLTerminateTag(&m_xmlString, "FrameGeometry"s);
    XMLTagAndContent(&m_xmlString, "attached_geometry"s, ""s);
    XMLInitiateTag(&m_xmlString, "WrapObjectSet"s, {{"name"s, "wrapobjectset"s}});
    XMLTagAndContent(&m_xmlString, "objects"s, ""s);
    XMLTagAndContent(&m_xmlString, "groups"s, ""s);
    XMLTerminateTag(&m_xmlString, "WrapObjectSet"s);
    XMLTerminateTag(&m_xmlString, "Ground"s);

    // put in placeholders for credist and publications
    XMLTagAndContent(&m_xmlString, "credits"s, "Add credits statement here"s);
    XMLTagAndContent(&m_xmlString, "publications"s, "Add publications statement here"s);

    // set some options
    XMLTagAndContent(&m_xmlString, "length_units"s, "metres"s);
    XMLTagAndContent(&m_xmlString, "force_units"s, "Newtons"s);
    XMLTagAndContent(&m_xmlString, "gravity"s, GSUtil::ToString(m_simulation->GetGlobal()->Gravity()));

    CreateBodySet();
    CreateJointSet();
    CreateControllerSet();
    CreateConstraintSet();
    CreateForceSet();
    CreateMarkerSet();
    CreateContactGeometrySet();

    XMLTerminateTag(&m_xmlString, "Model"s);
    XMLTerminateTag(&m_xmlString, "OpenSimDocument"s);
}

void OpenSimExporter::CreateBodySet()
{
    XMLInitiateTag(&m_xmlString, "BodySet"s, {{"name"s, "bodyset"s}});
    XMLInitiateTag(&m_xmlString, "objects"s);

    for (auto &&bodyIter : *m_simulation->GetBodyList())
    {
        XMLInitiateTag(&m_xmlString, "Body"s, {{"name"s, bodyIter.first}});

        // The geometry used to display the axes of this Frame
        XMLInitiateTag(&m_xmlString, "FrameGeometry"s, {{"name"s, "frame_geometry"s}});
        XMLTagAndContent(&m_xmlString, "socket_frame"s, ".."s);
        XMLTagAndContent(&m_xmlString, "scale_factors"s, "1 1 1"s);
        XMLTerminateTag(&m_xmlString, "FrameGeometry"s);
        // mesh
        XMLInitiateTag(&m_xmlString, "attached_geometry"s);
        if (bodyIter.second->GetGraphicFile1().size())
        {
            std::string root, ext;
            pystring::os::path::splitext(root, ext, bodyIter.second->GetGraphicFile1());
            std::string basename = pystring::os::path::basename(bodyIter.second->GetGraphicFile1());
            std::string mesh_path = pystring::os::path::join(m_pathToObjFiles, basename);
            XMLInitiateTag(&m_xmlString, "Mesh"s, {{"name"s, root}});
            XMLTagAndContent(&m_xmlString, "socket_frame"s, ".."s);
            XMLTagAndContent(&m_xmlString, "scale_factors"s, "1 1 1"s);
            XMLInitiateTag(&m_xmlString, "Appearance"s);
            XMLTagAndContent(&m_xmlString, "opacity"s, GSUtil::ToString(bodyIter.second->colour1().alpha()));
            XMLTagAndContent(&m_xmlString, "color"s, bodyIter.second->colour1().GetFloatColourRGB());
            XMLTerminateTag(&m_xmlString, "Appearance"s);
            XMLTagAndContent(&m_xmlString, "mesh_file"s, mesh_path);
            XMLTerminateTag(&m_xmlString, "Mesh"s);
        }
        XMLTerminateTag(&m_xmlString, "attached_geometry"s);
        // wrap objects
        XMLInitiateTag(&m_xmlString, "WrapObjectSet"s, {{"name"s, "wrapobjectset"s}});
        XMLInitiateTag(&m_xmlString, "objects"s);
        XMLTerminateTag(&m_xmlString, "objects"s);
        XMLTagAndContent(&m_xmlString, "groups"s, ""s);
        XMLTerminateTag(&m_xmlString, "WrapObjectSet"s);
        // mas sproperties
        double mass, ixx, iyy, izz, ixy, izx, iyz;
        bodyIter.second->GetMass(&mass, &ixx, &iyy, &izz, &ixy, &izx, &iyz);
        XMLTagAndContent(&m_xmlString, "mass"s, GSUtil::ToString(mass));
        XMLTagAndContent(&m_xmlString, "mass_center"s, "0 0 0"s); // The location of the mass center in the body frame
        XMLTagAndContent(&m_xmlString, "inertia"s, GSUtil::ToString(".17g %.17g %.17g %.17g %.17g %.17g", ixx, iyy, izz, ixy, izx, iyz)); // elements of the inertia tensor (Vec6) as [Ixx Iyy Izz Ixy Ixz Iyz] measured about the mass_center and not the body origin
        XMLTerminateTag(&m_xmlString, "Body"s);
    }

    XMLTerminateTag(&m_xmlString, "objects"s);
    XMLTagAndContent(&m_xmlString, "groups"s, ""s);
    XMLTerminateTag(&m_xmlString, "BodySet"s);

}

void OpenSimExporter::CreateJointSet()
{
    XMLInitiateTag(&m_xmlString, "JointSet"s, {{"name"s, "jointset"s}});
    XMLInitiateTag(&m_xmlString, "objects"s);
    XMLTerminateTag(&m_xmlString, "objects"s);
    XMLTagAndContent(&m_xmlString, "groups"s, ""s);
    XMLTerminateTag(&m_xmlString, "JointSet"s);
}

void OpenSimExporter::CreateControllerSet()
{
    XMLInitiateTag(&m_xmlString, "ControllerSet"s, {{"name"s, "controllerset"s}});
    XMLInitiateTag(&m_xmlString, "objects"s);
    XMLTerminateTag(&m_xmlString, "objects"s);
    XMLTagAndContent(&m_xmlString, "groups"s, ""s);
    XMLTerminateTag(&m_xmlString, "ControllerSet"s);
}

void OpenSimExporter::CreateConstraintSet()
{
    XMLInitiateTag(&m_xmlString, "ConstraintSet"s, {{"name"s, "constraintset"s}});
    XMLInitiateTag(&m_xmlString, "objects"s);
    XMLTerminateTag(&m_xmlString, "objects"s);
    XMLTagAndContent(&m_xmlString, "groups"s, ""s);
    XMLTerminateTag(&m_xmlString, "ConstraintSet"s);
}

void OpenSimExporter::CreateForceSet()
{
    XMLInitiateTag(&m_xmlString, "ForceSet"s, {{"name"s, "forceset"s}});
    XMLInitiateTag(&m_xmlString, "objects"s);
    XMLTerminateTag(&m_xmlString, "objects"s);
    XMLTagAndContent(&m_xmlString, "groups"s, ""s);
    XMLTerminateTag(&m_xmlString, "ForceSet"s);
}

void OpenSimExporter::CreateMarkerSet()
{
    XMLInitiateTag(&m_xmlString, "MarkerSet"s, {{"name"s, "markerset"s}});
    XMLInitiateTag(&m_xmlString, "objects"s);
    XMLTerminateTag(&m_xmlString, "objects"s);
    XMLTagAndContent(&m_xmlString, "groups"s, ""s);
    XMLTerminateTag(&m_xmlString, "MarkerSet"s);
}

void OpenSimExporter::CreateContactGeometrySet()
{
    XMLInitiateTag(&m_xmlString, "ContactGeometrySet"s, {{"name"s, "contactgeometryset"s}});
    XMLInitiateTag(&m_xmlString, "objects"s);
    XMLTerminateTag(&m_xmlString, "objects"s);
    XMLTagAndContent(&m_xmlString, "groups"s, ""s);
    XMLTerminateTag(&m_xmlString, "ContactGeometrySet"s);
}


void OpenSimExporter::XMLInitiateTag(std::string *xmlString, const std::string &tag, const std::map<std::string, std::string> &attributes, bool terminate)
{
    xmlString->append("<"s + tag + " "s);
    for (auto &&iter : attributes)
    {
        xmlString->append(iter.first + "=\""s + iter.second + "\" "s);
    }
    xmlString->pop_back();
    if (terminate) { xmlString->append("/>\n"s); }
    else { xmlString->append(">\n"s); }
}

void OpenSimExporter::XMLTerminateTag(std::string *xmlString, const std::string &tag)
{
    xmlString->append("</"s + tag + ">\n"s);
}

void OpenSimExporter::XMLTagAndContent(std::string *xmlString, const std::string &tag, const std::string &content)
{
    xmlString->append("<"s + tag + ">"s);
    xmlString->append(content);
    xmlString->append("</"s + tag + ">\n"s);
}

std::string OpenSimExporter::pathToObjFiles() const
{
    return m_pathToObjFiles;
}

void OpenSimExporter::setPathToObjFiles(const std::string &newPathToObjFiles)
{
    m_pathToObjFiles = newPathToObjFiles;
}

std::string *OpenSimExporter::xmlString()
{
    return &m_xmlString;
}



} // namespace GaitSym


