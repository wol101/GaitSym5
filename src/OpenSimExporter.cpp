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
#include "FixedJoint.h"
#include "Marker.h"
#include "MAMuscle.h"
#include "MAMuscleComplete.h"
#include "TwoPointStrap.h"
#include "NPointStrap.h"

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
    m_xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"s;

    // create the name mappings
    for (auto &&nameIter : m_simulation->GetNameList())
    {
        std::string legalName;
        char c = nameIter[0];
        if ((c >= 'A'  && c <= 'Z') || (c >= 'a'  && c <= 'z')) { legalName.push_back(c); }
        else
        {
            legalName.push_back('N');
            if (c >= '0'  && c <= '9') { legalName.push_back(c); }
            else { legalName.push_back('_'); }
        }
        for (size_t i = 1; i < nameIter.size(); i++)
        {
            c = nameIter[i];
            if ((c >= 'A'  && c <= 'Z') || (c >= 'a'  && c <= 'z') || (c >= '0'  && c <= '9')) { legalName.push_back(c); }
            else { legalName.push_back('_'); }
        }
        if (m_legalNameReverseMap.count(legalName) == 0)
        {
            m_legalNameReverseMap[legalName] = nameIter;
            m_legalNameMap[nameIter] = legalName;
        }
        else
        {
            int count = 0;
            while (true)
            {
                std::string newLegalName = legalName + std::to_string(count++);
                if (m_legalNameReverseMap.count(newLegalName) == 0)
                {
                    m_legalNameReverseMap[newLegalName] = nameIter;
                    m_legalNameMap[nameIter] = newLegalName;
                    break;
                }
                if (count > 99999)
                {
                    std::cerr << "OpenSimExporter::Process() error: Unable to find a sensible legal name for \"" << nameIter << "\"\n";
                    return;
                }
            }
        }
    }

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
    XMLTagAndContent(&m_xmlString, "length_units"s, "meters"s);
    XMLTagAndContent(&m_xmlString, "force_units"s, "N"s);
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
        XMLInitiateTag(&m_xmlString, "Body"s, {{"name"s, m_legalNameMap[bodyIter.second->name()]}});

        // The geometry used to display the axes of this Frame
        XMLInitiateTag(&m_xmlString, "FrameGeometry"s, {{"name"s, "frame_geometry"s}});
        XMLTagAndContent(&m_xmlString, "socket_frame"s, ".."s);
        XMLTagAndContent(&m_xmlString, "scale_factors"s, "1 1 1"s);
        XMLTerminateTag(&m_xmlString, "FrameGeometry"s);
        // mesh
        XMLInitiateTag(&m_xmlString, "attached_geometry"s);
        if (bodyIter.second->GetGraphicFile1().size())
        {
            std::string basename = pystring::os::path::basename(bodyIter.second->GetGraphicFile1());
            std::string mesh_path = pystring::os::path::join(m_pathToObjFiles, basename);
            XMLInitiateTag(&m_xmlString, "Mesh"s, {{"name"s, m_legalNameMap[bodyIter.second->name()] + "_mesh"s}});
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
        pgd::Vector3 referencePosition;
        for (auto &&jointIter : *m_simulation->GetJointList())
        {
            if (jointIter.second->body2() == bodyIter.second.get()) { referencePosition = jointIter.second->body2Marker()->GetPosition(); } // if a body is connected to a parent, then its reference is that joint
        }
        XMLTagAndContent(&m_xmlString, "mass_center"s, GSUtil::ToString(-referencePosition)); // The location of the mass center in the body frame which is based on the joint position
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

    for (auto &&jointIter : *m_simulation->GetJointList())
    {
        if (const HingeJoint *hingeJoint = dynamic_cast<const HingeJoint *>(jointIter.second.get()))
        {
            XMLInitiateTag(&m_xmlString, "PinJoint"s, {{"name"s, m_legalNameMap[jointIter.second->name()]}});

            XMLTagAndContent(&m_xmlString, "socket_parent_frame"s, m_legalNameMap[hingeJoint->body1()->name()] + "_offset"s);
            XMLTagAndContent(&m_xmlString, "socket_child_frame"s, m_legalNameMap[hingeJoint->body2()->name()] + "_offset"s);

            XMLInitiateTag(&m_xmlString, "coordinates"s);
            XMLInitiateTag(&m_xmlString, "Coordinate"s, {{"name"s, m_legalNameMap[jointIter.second->name()] + "_angle_r"s}});
            XMLTagAndContent(&m_xmlString, "default_value"s, "0"s);
            XMLTagAndContent(&m_xmlString, "default_speed_value"s, "0"s);
            XMLTagAndContent(&m_xmlString, "range"s, GSUtil::ToString(pgd::Vector2(-hingeJoint->stops()[1], -hingeJoint->stops()[0])));
            XMLTagAndContent(&m_xmlString, "clamped"s, "true"s);
            XMLTagAndContent(&m_xmlString, "locked"s, "false"s);
            XMLTagAndContent(&m_xmlString, "prescribed"s, "false"s);
            XMLTerminateTag(&m_xmlString, "Coordinate"s);
            XMLTerminateTag(&m_xmlString, "coordinates"s);

            XMLInitiateTag(&m_xmlString, "frames"s);

            XMLInitiateTag(&m_xmlString, "PhysicalOffsetFrame"s, {{"name"s, m_legalNameMap[hingeJoint->body1()->name()] + "_offset"s}});
            XMLInitiateTag(&m_xmlString, "FrameGeometry"s, {{"name"s, "frame_geometry"s}});
            XMLTagAndContent(&m_xmlString, "socket_frame"s, ".."s);
            XMLTagAndContent(&m_xmlString, "scale_factors"s, "1 1 1"s);
            XMLTerminateTag(&m_xmlString, "FrameGeometry"s);
            XMLTagAndContent(&m_xmlString, "socket_parent"s, "/bodyset/"s + m_legalNameMap[hingeJoint->body1()->name()]);
            XMLTagAndContent(&m_xmlString, "translation"s, GSUtil::ToString(hingeJoint->body1Marker()->GetPosition()));
            pgd::Vector3 axis = hingeJoint->body1Marker()->GetAxis(GaitSym::Marker::X);
            pgd::Vector3 zAxis(0, 0, 1);
            pgd::Quaternion rotation = pgd::FindRotation(zAxis, axis);
            pgd::Vector3 euler = pgd::MakeEulerAnglesFromQRadian(rotation);
            XMLTagAndContent(&m_xmlString, "orientation"s, GSUtil::ToString(euler));
            XMLTerminateTag(&m_xmlString, "PhysicalOffsetFrame"s);

            XMLInitiateTag(&m_xmlString, "PhysicalOffsetFrame"s, {{"name"s, m_legalNameMap[hingeJoint->body2()->name()] + "_offset"s}});
            XMLInitiateTag(&m_xmlString, "FrameGeometry"s, {{"name"s, "frame_geometry"s}});
            XMLTagAndContent(&m_xmlString, "socket_frame"s, ".."s);
            XMLTagAndContent(&m_xmlString, "scale_factors"s, "1 1 1"s);
            XMLTerminateTag(&m_xmlString, "FrameGeometry"s);
            XMLTagAndContent(&m_xmlString, "socket_parent"s, "/bodyset/"s + m_legalNameMap[hingeJoint->body2()->name()]);
            XMLTagAndContent(&m_xmlString, "translation"s, GSUtil::ToString(hingeJoint->body2Marker()->GetPosition()));
            axis = hingeJoint->body1Marker()->GetAxis(GaitSym::Marker::X);
            rotation = pgd::FindRotation(zAxis, axis);
            euler = pgd::MakeEulerAnglesFromQRadian(rotation);
            XMLTagAndContent(&m_xmlString, "orientation"s, GSUtil::ToString(euler));
            XMLTerminateTag(&m_xmlString, "PhysicalOffsetFrame"s);

            XMLTerminateTag(&m_xmlString, "frames"s);

            XMLTerminateTag(&m_xmlString, "PinJoint"s);
        }

        if (const FixedJoint *fixedJoint = dynamic_cast<const FixedJoint *>(jointIter.second.get()))
        {
            XMLInitiateTag(&m_xmlString, "WeldJoint"s, {{"name"s, m_legalNameMap[fixedJoint->name()]}});

            XMLTagAndContent(&m_xmlString, "socket_parent_frame"s, m_legalNameMap[fixedJoint->body1()->name()] + "_offset"s);
            XMLTagAndContent(&m_xmlString, "socket_child_frame"s, m_legalNameMap[fixedJoint->body2()->name()] + "_offset"s);

            XMLInitiateTag(&m_xmlString, "frames"s);

            XMLInitiateTag(&m_xmlString, "PhysicalOffsetFrame"s, {{"name"s, m_legalNameMap[fixedJoint->body1()->name()] + "_offset"s}});
            XMLInitiateTag(&m_xmlString, "FrameGeometry"s, {{"name"s, "frame_geometry"s}});
            XMLTagAndContent(&m_xmlString, "socket_frame"s, ".."s);
            XMLTagAndContent(&m_xmlString, "scale_factors"s, "1 1 1"s);
            XMLTerminateTag(&m_xmlString, "FrameGeometry"s);
            XMLTagAndContent(&m_xmlString, "socket_parent"s, "/bodyset/"s + m_legalNameMap[fixedJoint->body1()->name()]);
            XMLTagAndContent(&m_xmlString, "translation"s, GSUtil::ToString(fixedJoint->body1Marker()->GetPosition()));
            XMLTagAndContent(&m_xmlString, "orientation"s, "0 0 0"s);
            XMLTerminateTag(&m_xmlString, "PhysicalOffsetFrame"s);

            XMLInitiateTag(&m_xmlString, "PhysicalOffsetFrame"s, {{"name"s, m_legalNameMap[fixedJoint->body2()->name()] + "_offset"s}});
            XMLInitiateTag(&m_xmlString, "FrameGeometry"s, {{"name"s, "frame_geometry"s}});
            XMLTagAndContent(&m_xmlString, "socket_frame"s, ".."s);
            XMLTagAndContent(&m_xmlString, "scale_factors"s, "1 1 1"s);
            XMLTerminateTag(&m_xmlString, "FrameGeometry"s);
            XMLTagAndContent(&m_xmlString, "socket_parent"s, "/bodyset/"s + m_legalNameMap[fixedJoint->body2()->name()]);
            XMLTagAndContent(&m_xmlString, "translation"s, GSUtil::ToString(fixedJoint->body2Marker()->GetPosition()));
            XMLTagAndContent(&m_xmlString, "orientation"s, "0 0 0"s);
            XMLTerminateTag(&m_xmlString, "PhysicalOffsetFrame"s);

            XMLTerminateTag(&m_xmlString, "frames"s);

            XMLTerminateTag(&m_xmlString, "WeldJoint"s);
        }
    }

    // now handle ant free joints for parentless bodies
    for (auto &&bodyIter : *m_simulation->GetBodyList())
    {
        bool parentlessBody = true;
        for (auto &&jointIter : *m_simulation->GetJointList())
        {
            if (jointIter.second->body2() == bodyIter.second.get())
            {
                parentlessBody = false;
                break;
            }
        }
        if (parentlessBody)
        {
            pgd::Vector3 position = bodyIter.second->GetConstructionPosition();
            pgd::Vector3 euler(-1.5707963267948966, 0, 0); // all GaitSym bodies are constructed with no rotation, and rotating -90 degrees about the X axis converts from Z up to Y up
            pgd::Quaternion rotation = pgd::MakeQFromEulerAnglesRadian(euler.x, euler.y, euler.z);
            position = pgd::QVRotate(rotation, position);
            XMLInitiateTag(&m_xmlString, "FreeJoint"s, {{"name"s, "free_"s + m_legalNameMap[bodyIter.second->name()]}});

            XMLTagAndContent(&m_xmlString, "socket_parent_frame"s, "/ground"s);
            XMLTagAndContent(&m_xmlString, "socket_child_frame"s, "/bodyset/"s + m_legalNameMap[bodyIter.second->name()]);

            XMLInitiateTag(&m_xmlString, "coordinates"s);

            XMLInitiateTag(&m_xmlString, "Coordinate"s, {{"name"s, "free_"s + m_legalNameMap[bodyIter.second->name()] + "_coord_0"s}});
            XMLTagAndContent(&m_xmlString, "motion_type"s, "rotational"s);
            XMLTagAndContent(&m_xmlString, "default_value"s, GSUtil::ToString(euler.x));
            XMLTagAndContent(&m_xmlString, "default_speed_value"s, "0"s);
            XMLTagAndContent(&m_xmlString, "range"s, "-1.57079633 1.57079633"s);
            XMLTagAndContent(&m_xmlString, "clamped"s, "true"s);
            XMLTagAndContent(&m_xmlString, "locked"s, "false"s);
            XMLTagAndContent(&m_xmlString, "prescribed"s, "false"s);
            XMLTerminateTag(&m_xmlString, "Coordinate"s);

            XMLInitiateTag(&m_xmlString, "Coordinate"s, {{"name"s, "free_"s + m_legalNameMap[bodyIter.second->name()] + "_coord_1"s}});
            XMLTagAndContent(&m_xmlString, "motion_type"s, "rotational"s);
            XMLTagAndContent(&m_xmlString, "default_value"s, GSUtil::ToString(euler.y));
            XMLTagAndContent(&m_xmlString, "default_speed_value"s, "0"s);
            XMLTagAndContent(&m_xmlString, "range"s, "-1.57079633 1.57079633"s);
            XMLTagAndContent(&m_xmlString, "clamped"s, "true"s);
            XMLTagAndContent(&m_xmlString, "locked"s, "false"s);
            XMLTagAndContent(&m_xmlString, "prescribed"s, "false"s);
            XMLTerminateTag(&m_xmlString, "Coordinate"s);

            XMLInitiateTag(&m_xmlString, "Coordinate"s, {{"name"s, "free_"s + m_legalNameMap[bodyIter.second->name()] + "_coord_2"s}});
            XMLTagAndContent(&m_xmlString, "motion_type"s, "rotational"s);
            XMLTagAndContent(&m_xmlString, "default_value"s, GSUtil::ToString(euler.z));
            XMLTagAndContent(&m_xmlString, "default_speed_value"s, "0"s);
            XMLTagAndContent(&m_xmlString, "range"s, "-1.57079633 1.57079633"s);
            XMLTagAndContent(&m_xmlString, "clamped"s, "true"s);
            XMLTagAndContent(&m_xmlString, "locked"s, "false"s);
            XMLTagAndContent(&m_xmlString, "prescribed"s, "false"s);
            XMLTerminateTag(&m_xmlString, "Coordinate"s);

            XMLInitiateTag(&m_xmlString, "Coordinate"s, {{"name"s, "free_"s + m_legalNameMap[bodyIter.second->name()] + "_coord_3"s}});
            XMLTagAndContent(&m_xmlString, "motion_type"s, "translational"s);
            XMLTagAndContent(&m_xmlString, "default_value"s, GSUtil::ToString(position.x));
            XMLTagAndContent(&m_xmlString, "default_speed_value"s, "0"s);
            XMLTagAndContent(&m_xmlString, "range"s, "-99 99"s);
            XMLTagAndContent(&m_xmlString, "clamped"s, "true"s);
            XMLTagAndContent(&m_xmlString, "locked"s, "false"s);
            XMLTagAndContent(&m_xmlString, "prescribed"s, "false"s);
            XMLTerminateTag(&m_xmlString, "Coordinate"s);

            XMLInitiateTag(&m_xmlString, "Coordinate"s, {{"name"s, "free_"s + m_legalNameMap[bodyIter.second->name()] + "_coord_4"s}});
            XMLTagAndContent(&m_xmlString, "motion_type"s, "translational"s);
            XMLTagAndContent(&m_xmlString, "default_value"s, GSUtil::ToString(position.y));
            XMLTagAndContent(&m_xmlString, "default_speed_value"s, "0"s);
            XMLTagAndContent(&m_xmlString, "range"s, "-99 99"s);
            XMLTagAndContent(&m_xmlString, "clamped"s, "true"s);
            XMLTagAndContent(&m_xmlString, "locked"s, "false"s);
            XMLTagAndContent(&m_xmlString, "prescribed"s, "false"s);
            XMLTerminateTag(&m_xmlString, "Coordinate"s);

            XMLInitiateTag(&m_xmlString, "Coordinate"s, {{"name"s, "free_"s + m_legalNameMap[bodyIter.second->name()] + "_coord_5"s}});
            XMLTagAndContent(&m_xmlString, "motion_type"s, "translational"s);
            XMLTagAndContent(&m_xmlString, "default_value"s, GSUtil::ToString(position.z));
            XMLTagAndContent(&m_xmlString, "default_speed_value"s, "0"s);
            XMLTagAndContent(&m_xmlString, "range"s, "-99 99"s);
            XMLTagAndContent(&m_xmlString, "clamped"s, "true"s);
            XMLTagAndContent(&m_xmlString, "locked"s, "false"s);
            XMLTagAndContent(&m_xmlString, "prescribed"s, "false"s);
            XMLTerminateTag(&m_xmlString, "Coordinate"s);

            XMLTerminateTag(&m_xmlString, "coordinates"s);

            XMLTerminateTag(&m_xmlString, "FreeJoint"s);
        }
    }

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

    for (auto &&muscleIter : *m_simulation->GetMuscleList())
    {
        Muscle *muscle = muscleIter.second.get();
        Strap *strap = muscleIter.second->GetStrap();
        XMLInitiateTag(&m_xmlString, "Thelen2003Muscle"s, {{"name"s, m_legalNameMap[muscle->name()]}});
        XMLTagAndContent(&m_xmlString, "appliesForce"s, "true"s);

        XMLInitiateTag(&m_xmlString, "GeometryPath"s, {{"name"s, "path"s}});

        XMLInitiateTag(&m_xmlString, "Appearance"s);
        XMLTagAndContent(&m_xmlString, "opacity"s, GSUtil::ToString(muscle->colour1().alpha()));
        XMLTagAndContent(&m_xmlString, "color"s, muscle->colour1().GetFloatColourRGB());
        XMLTerminateTag(&m_xmlString, "Appearance"s);

        while (true)
        {
            if (TwoPointStrap *twoPointStrap = dynamic_cast<TwoPointStrap *>(strap))
            {
                CreatePathPointSet(muscle->name(), {twoPointStrap->GetOriginMarker(), twoPointStrap->GetInsertionMarker()});
                break;
            }
            if (NPointStrap *nPointStrap = dynamic_cast<NPointStrap *>(strap))
            {
                std::vector<const Marker *> markerList;
                markerList.push_back(nPointStrap->GetOriginMarker());
                for (auto &&markerIter : *nPointStrap->GetViaPointMarkers()) { markerList.push_back(markerIter); }
                markerList.push_back(nPointStrap->GetInsertionMarker());
                CreatePathPointSet(muscle->name(), markerList);
                break;
            }
            std::cerr << "OpenSimExporter::CreateForceSet() error: Unsupported strap type in Muscle ID=\"" << muscle->name() << "\"\n";
            break;
        }
        XMLInitiateTag(&m_xmlString, "PathWrapSet"s);
        XMLTagAndContent(&m_xmlString, "objects"s, ""s);
        XMLTagAndContent(&m_xmlString, "groups"s, ""s);
        XMLTerminateTag(&m_xmlString, "PathWrapSet"s);

        XMLTerminateTag(&m_xmlString, "GeometryPath"s);

        while (true)
        {
            if (MAMuscle *maMuscle = dynamic_cast<MAMuscle *>(muscle))
            {
                XMLTagAndContent(&m_xmlString, "optimal_force"s, "1"s);
                XMLTagAndContent(&m_xmlString, "max_isometric_force"s, GSUtil::ToString(maMuscle->pca() * maMuscle->forcePerUnitArea()));
                XMLTagAndContent(&m_xmlString, "optimal_fiber_length"s, GSUtil::ToString(maMuscle->fibreLength()));
                double tendonLength = strap->Length() - maMuscle->fibreLength();
                XMLTagAndContent(&m_xmlString, "tendon_slack_length"s, GSUtil::ToString(std::max(tendonLength, 0.001)));
                XMLTagAndContent(&m_xmlString, "pennation_angle_at_optimal"s, "0"s);
                XMLTagAndContent(&m_xmlString, "max_contraction_velocity"s, GSUtil::ToString(maMuscle->vMaxFactor()));
                XMLTagAndContent(&m_xmlString, "default_activation"s, "0.01"s);
                XMLTagAndContent(&m_xmlString, "minimum_activation"s, "0.01"s);
                break;
            }
            std::cerr << "OpenSimExporter::CreateForceSet() error: Unsupported muscle type in Muscle ID=\"" << muscle->name() << "\"\n";
            break;
        }

        XMLTerminateTag(&m_xmlString, "Thelen2003Muscle"s);
    }

    XMLTerminateTag(&m_xmlString, "objects"s);
    XMLTagAndContent(&m_xmlString, "groups"s, ""s);
    XMLTerminateTag(&m_xmlString, "ForceSet"s);
}

void OpenSimExporter::CreatePathPointSet(std::string name, const std::vector<const Marker *> &markerList)
{
    XMLInitiateTag(&m_xmlString, "PathPointSet"s);
    XMLInitiateTag(&m_xmlString, "objects"s);

    for (size_t i = 0; i < markerList.size(); i++)
    {
        const Marker *marker = markerList[i];
        XMLInitiateTag(&m_xmlString, "PathPoint"s, {{"name"s, m_legalNameMap[name] + "-P"s + GSUtil::ToString(i + 1)}});
        XMLTagAndContent(&m_xmlString, "socket_parent_frame"s, "/bodyset/"s + m_legalNameMap[marker->GetBody()->name()]);
        XMLTagAndContent(&m_xmlString, "location"s, GSUtil::ToString(marker->GetPosition()));
        XMLTerminateTag(&m_xmlString, "PathPoint"s);
    }

    XMLTerminateTag(&m_xmlString, "objects"s);
    XMLTagAndContent(&m_xmlString, "groups"s, ""s);
    XMLTerminateTag(&m_xmlString, "PathPointSet"s);
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


