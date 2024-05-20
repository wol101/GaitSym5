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

namespace GaitSym {

OpenSimExporter::OpenSimExporter()
{
}

std::string *OpenSimExporter::Process(Simulation *simulation)
{
    std::string *err = nullptr;
    m_simulation = simulation;

    // create the MuJoCo xml versions of the main elements
    err = CreateConnectedGroups();
    if (err) { return err; }

    err = CreateTree();
    if (err) { return err; }

    return err;
}

std::string *OpenSimExporter::CreateConnectedGroups()
{
    std::map<std::string, Body *> untestedBodies;
    for (auto &&iter : *simulation()->GetBodyList()) { untestedBodies[iter.first] = iter.second.get(); }
    std::map<std::string, Joint *> untestedJoints;
    for (auto &&iter : *simulation()->GetJointList()) { untestedJoints[iter.first] = iter.second.get(); }

    while (untestedBodies.size())
    {
        std::map<std::string, Body *> bodiesToTest;
        auto bodyHandle = untestedBodies.extract(untestedBodies.begin());
        bodiesToTest[bodyHandle.key()] = bodyHandle.mapped();
        auto currentConnectedGroup = std::make_unique<std::map<std::string, Body *>>();
        (*currentConnectedGroup)[bodyHandle.key()] = bodyHandle.mapped();
        while (bodiesToTest.size() && untestedJoints.size())
        {
            bodyHandle = bodiesToTest.extract(bodiesToTest.begin());
            for (auto jointIter = untestedJoints.begin(); jointIter != untestedJoints.end(); /* no increment */)
            {
                if (jointIter->second->body1() == bodyHandle.mapped() || jointIter->second->body2() == bodyHandle.mapped())
                {
                    Body *linkedBody = jointIter->second->body1() == bodyHandle.mapped() ? jointIter->second->body2() : jointIter->second->body1();
                    auto linkedBodyIter = untestedBodies.find(linkedBody->name());
                    if (linkedBodyIter != untestedBodies.end()) // needed because we might have loops
                    {
                        bodiesToTest[linkedBody->name()] = linkedBody;
                        (*currentConnectedGroup)[linkedBody->name()] = linkedBody;
                        untestedBodies.erase(linkedBodyIter);
                    }
                    else
                    {
                        std::cerr << "Warning: OpenSimExporter::CreateConnectedGroups possible loop detected\n";
                    }
                    jointIter = untestedJoints.erase(jointIter);
                }
                else
                {
                    jointIter++;
                }
            }
        }
// #define DEBUG_OPENSIM_EXPORTER_CREATE_CONNECTED_GROUPS
#ifdef DEBUG_OPENSIM_EXPORTER_CREATE_CONNECTED_GROUPS
        std::cerr << "New Group\n";
        for (auto && iter : *currentConnectedGroup)
        {
            std::cerr << "currentConnectedGroup[" << iter.first << "] = " << iter.second->name() << "\n";
        }
#endif
        m_connectedGroups.push_back(std::move(currentConnectedGroup));
    }
    return nullptr;
}



std::string *OpenSimExporter::CreateTree()
{
    // with no hint just assume the biggest body is the root
    for (auto &&groupIter : m_connectedGroups)
    {
        Body *maxMassBody = nullptr;
        double maxMass = -std::numeric_limits<double>::max();
        for (auto &&bodyIter : *groupIter)
        {
            Body *body = bodyIter.second;
            double mass = body->GetMass();
            if (mass > maxMass)
            {
                maxMass = mass;
                maxMassBody = body;
            }
        }
        m_rootTreeBodyList.emplace_back();
        m_rootTreeBodyList.back().body = maxMassBody;
    }

    for (size_t i = 0; i < m_rootTreeBodyList.size(); i++)
    {
        m_jointLoopDetector.clear();
        m_jointLoopDetector.insert(m_rootTreeBodyList[i].body);
        m_jointsLeftToInclude.clear();
        m_jointsLeftToInclude.reserve(simulation()->GetJointList()->size());
        for (auto &&iter : *simulation()->GetJointList()) { m_jointsLeftToInclude.push_back(iter.second.get()); }
        m_bodiesLeftToInclude.clear();
        m_bodiesLeftToInclude.reserve(simulation()->GetBodyList()->size());
        m_bodiesLeftToInclude.push_back(&m_rootTreeBodyList[i]);
        while (m_bodiesLeftToInclude.size())
        {
// #define DEBUG_OPENSIM_EXPORTER_CREATE_TREE
#ifdef DEBUG_OPENSIM_EXPORTER_CREATE_TREE
            for (auto &&iter : m_bodiesLeftToInclude)
            {
                std::cerr << "TreeBodies left " << m_bodiesLeftToInclude.size() << "\n";
                std::cerr << "TreeBody " << iter->body->name() << "\n";
                for (auto &&iter2 : iter->childList)
                {
                    std::cerr << "Child " << iter2->body->name() << "\n";
                }
            }
            for (auto &&iter : m_jointLoopDetector)
            {
                std::cerr << "Loop detector bodies left " << m_jointLoopDetector.size() << "\n";
                std::cerr << "Body " << iter->name() << "\n";
            }
#endif
            TreeBody *currentBody = m_bodiesLeftToInclude.back();
            m_bodiesLeftToInclude.pop_back();
            auto iter = m_jointsLeftToInclude.begin();
            while (iter != m_jointsLeftToInclude.end())
            {
                if ((*iter)->body1() != currentBody->body && (*iter)->body2() != currentBody->body)
                {
                    iter++;
                    continue;
                }
                // we have found a link to a child
                if (m_jointLoopDetector.count(currentBody->body) > 1)
                {
                    if (currentBody->body) setLastError(GSUtil::ToString("Error: OpenSimExporter error in CreateTree. Trying to add \"%s\" twice", currentBody->body->name().c_str()));
                    else setLastError(GSUtil::ToString("Error: OpenSimExporter error in CreateTree. Trying to add \"%s\" twice", "World"));
                    return lastErrorPtr();
                }
                std::unique_ptr<TreeBody> newTreeBody = std::make_unique<TreeBody>();
                newTreeBody->body = ((*iter)->body1() == currentBody->body) ? (*iter)->body2() : (*iter)->body1();
                newTreeBody->parent = currentBody;
                newTreeBody->jointToParent = *iter;
                m_bodiesLeftToInclude.push_back(newTreeBody.get());
                m_jointLoopDetector.insert(newTreeBody->body);
                currentBody->childList.push_back(std::move(newTreeBody));
                iter = m_jointsLeftToInclude.erase(iter);
            }
        }
    }

#ifdef DEBUG_OPENSIM_EXPORTER_CREATE_TREE
    for (auto &&iter : m_rootTreeBodyList)
    {
        std::deque<TreeBody *> toProcess;
        toProcess.push_back(&iter);
        while (toProcess.size())
        {
            std::cerr << "TreeBody " << toProcess.front()->body->name() << "\n";
            for (auto &&iter2 : toProcess.front()->childList)
            {
                std::cerr << "Child " << iter2->body->name() << "\n";
                toProcess.push_back(iter2.get());
            }
            toProcess.pop_front();
        }
    }
#endif

    // start building the XML
    XMLInitiateTag(&m_xmlString, "mujoco"s, {{"model"s, "GaitSym"s}});

    // set some options
    XMLInitiateTag(&m_xmlString, "compiler"s, {{"angle"s, "radian"s}, {"autolimits"s, "true"s}}, true);
    XMLInitiateTag(&m_xmlString, "option"s, {{"timestep"s, GSUtil::ToString(simulation()->GetGlobal()->StepSize())}}, true);

    // create the world body
    XMLInitiateTag(&m_xmlString, "worldbody"s);

    // create any geoms attached to world
    for (auto &&iter : *simulation()->GetGeomList())
    {
        if (iter.second->GetBody() == nullptr)
        {
            std::string *err = CreateGeom(iter.second.get());
            if (err) return err;
        }
    }

    // this creates the whole model recursively
    for (auto &&iter : m_rootTreeBodyList)
    {
        CreateBody(iter);
    }

    XMLTerminateTag(&m_xmlString, "worldbody"s);

    XMLTerminateTag(&m_xmlString, "mujoco"s);

    return nullptr;
}

std::string *OpenSimExporter::CreateBody(const TreeBody &treeBody)
{
    Body *body = treeBody.body;
    pgd::Vector3 position = body->GetConstructionPosition();
    pgd::Quaternion quaternion(true);
    if (treeBody.parent)
    {
        pgd::Vector3 parentConstructionPosition = treeBody.parent->body->GetConstructionPosition(); // only need position because construction rotation is always zero
        position = position - parentConstructionPosition;
    }
    std::map<std::string, std::string> attributes;
    attributes["name"s] = body->name();
    attributes["pos"s] = GSUtil::ToString(position);
    attributes["quat"s] = GSUtil::ToString(quaternion);
    XMLInitiateTag(&m_xmlString, "body", attributes);
    XMLInitiateTag(&m_xmlString, "geom", {{"type"s, "sphere"s}, {"size"s, ".1"s}}, true); // this is just a CM marker for debugging

    double mass, ixx, iyy, izz, ixy, izx, iyz;
    body->GetMass(&mass, &ixx, &iyy, &izz, &ixy, &izx, &iyz);
    attributes.clear();
    attributes["pos"s] = GSUtil::ToString(pgd::Vector3());
    attributes["mass"s] = GSUtil::ToString(mass);
    attributes["fullinertia"s] = GSUtil::ToString("%.17g %.17g %.17g %.17g %.17g %.17g", ixx, iyy, izz, ixy, izx, iyz);
    XMLInitiateTag(&m_xmlString, "inertial", attributes, true);

    std::string *err = CreateJoint(treeBody.jointToParent);
    if (err) return err;

    for (auto &&iter : *simulation()->GetGeomList())
    {
        if (iter.second->GetBody() == body)
        {
            err = CreateGeom(iter.second.get());
            if (err) return err;
        }
    }

    for (auto &&iter : treeBody.childList)
    {
        err = CreateBody(*iter);
        if (err) return err;
    }

    XMLTerminateTag(&m_xmlString, "body");
    return nullptr;
}

std::string *OpenSimExporter::CreateJoint(const Joint *joint)
{
    std::map<std::string, std::string> attributes;
    if (!joint)
    {
        attributes["name"s] = GSUtil::ToString("root%02d", m_freeJointCount);
        m_freeJointCount++;
        XMLInitiateTag(&m_xmlString, "freejoint"s, attributes, true);
        return nullptr;
    }

    while (true)
    {
        if (const HingeJoint *hingeJoint = dynamic_cast<const HingeJoint *>(joint))
        {
            // Marker *marker1 = hingeJoint->body1Marker();
            // pgd::Vector3 p1 = marker1->GetPosition();
            // pgd::Vector3 axis1 = marker1->GetAxis(Marker::X);
            // attributes["axis"s] = GSUtil::ToString(axis1);
            // attributes["pos"s] = GSUtil::ToString(p1);
            Marker *marker2 = hingeJoint->body2Marker();
            pgd::Vector3 p2 = marker2->GetPosition();
            pgd::Vector3 axis2 = marker2->GetAxis(Marker::X);
            pgd::Vector2 stops = hingeJoint->stops();
            // double springConstant = hingeJoint->stopSpring();
            // double dampingConstant = hingeJoint->stopDamp();
            attributes["name"s] = hingeJoint->name();
            attributes["axis"s] = GSUtil::ToString(axis2);
            attributes["pos"s] = GSUtil::ToString(p2);
            attributes["limited"s] = GSUtil::ToString(true);
            pgd::Vector2 reversedStops(-stops[1], -stops[0]);
            attributes["range"s] = GSUtil::ToString(reversedStops);
            XMLInitiateTag(&m_xmlString, "joint"s, attributes, true);
            // put a site on the joint
            attributes.clear();
            attributes["name"s] = hingeJoint->name() + "_site"s;
            attributes["pos"s] = GSUtil::ToString(p2);
            XMLInitiateTag(&m_xmlString, "site"s, attributes, true);
            break;
        }
        break;
    }
    return nullptr;
}


std::string *OpenSimExporter::CreateGeom(const Geom *geom)
{
    std::map<std::string, std::string> attributes;
    while (true)
    {
        if (const SphereGeom *sphereGeom = dynamic_cast<const SphereGeom *>(geom))
        {
            double radius = sphereGeom->radius();
            pgd::Vector3 position = sphereGeom->GetPosition();
            pgd::Quaternion quaternion = sphereGeom->GetQuaternion();
            // double staticFriction = sphereGeom->GetContactMu();
            // double dynamicFriction = staticFriction;
            // double contactBounce = sphereGeom->GetContactBounce();
            attributes["name"s] = sphereGeom->name();
            attributes["type"s] = "sphere"s;
            attributes["size"s] = GSUtil::ToString(radius);
            attributes["pos"s] = GSUtil::ToString(position);
            attributes["quat"s] = GSUtil::ToString(quaternion);
            XMLInitiateTag(&m_xmlString, "geom"s, attributes, true);
            break;
        }
        if (const PlaneGeom *planeGeom = dynamic_cast<const PlaneGeom *>(geom))
        {
            Marker *marker = planeGeom->geomMarker();
            pgd::Vector3 position = marker->GetPosition();
            pgd::Vector3 zAxis = marker->GetAxis(Marker::Z);
            attributes["name"s] = planeGeom->name();
            attributes["type"s] = "plane"s;
            attributes["pos"s] = GSUtil::ToString(position);
            attributes["zaxis"s] = GSUtil::ToString(zAxis);
            attributes["size"s] = GSUtil::ToString(pgd::Vector3(1, 1, 1));
            XMLInitiateTag(&m_xmlString, "geom"s, attributes, true);
            break;
        }
        break;
    }
    return nullptr;
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


} // namespace GaitSym


