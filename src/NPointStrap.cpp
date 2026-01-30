/*
 *  NPointStrap.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 27/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#include "NPointStrap.h"
#include "Body.h"
#include "PGDMath.h"
#include "Simulation.h"
#include "Marker.h"

#include "pystring.h"

#include <cmath>
#include <string.h>
#include <iostream>
#include <vector>

using namespace std::string_literals;
namespace GaitSym {

NPointStrap::NPointStrap(): Strap()
{
}

void NPointStrap::setOrigin(Marker *originMarker)
{
    m_originMarker = originMarker;
//    this->SetOrigin(originMarker->GetBody(), originMarker->GetPosition().data());
    if (pointForceList()->size() == 0)
    {
        std::unique_ptr<PointForce> origin = std::make_unique<PointForce>();
        origin->body = m_originMarker->body();
        pointForceList()->push_back(std::move(origin));
    }
    else
    {
        pointForceList()->at(0)->body = m_originMarker->body();
    }
}

void NPointStrap::setInsertion(Marker *insertionMarker)
{
    m_insertionMarker = insertionMarker;
//    this->SetInsertion(insertionMarker->GetBody(), insertionMarker->GetPosition().data());
    if (pointForceList()->size() <= 1)
    {
        std::unique_ptr<PointForce> insertion = std::make_unique<PointForce>();
        insertion->body = m_insertionMarker->body();
        pointForceList()->push_back(std::move(insertion));
    }
    else
    {
        pointForceList()->at(1)->body =  m_insertionMarker->body();
    }
}

void NPointStrap::setViaPoints(std::vector<Marker *> *viaPointMarkerList)
{
    m_viaBodyList.clear();
    m_viaPointList.clear();
    m_viaPointMarkerList.clear();
    pointForceList()->reserve(viaPointMarkerList->size() + 2);
    m_viaBodyList.reserve(viaPointMarkerList->size());
    m_viaPointList.reserve(viaPointMarkerList->size());
    m_viaPointMarkerList.reserve(viaPointMarkerList->size());
    for (size_t i = 0; i < viaPointMarkerList->size(); i++)
    {
        std::unique_ptr<PointForce> viaPointForce = std::make_unique<PointForce>();
        viaPointForce->body = viaPointMarkerList->at(i)->body();
        m_viaBodyList.push_back(viaPointForce->body);
        m_viaPointList.push_back(viaPointMarkerList->at(i)->position());
        m_viaPointMarkerList.push_back(viaPointMarkerList->at(i));
        pointForceList()->push_back(std::move(viaPointForce));
    }
}

const std::vector<pgd::Vector3> *NPointStrap::viaPoints() const
{
    return &m_viaPointList;
}

const std::vector<Body *> *NPointStrap::viaPointBodies() const
{
    return &m_viaBodyList;
}

const std::vector<Marker *> *NPointStrap::viaPointMarkers() const
{
    return &m_viaPointMarkerList;
}

Marker *NPointStrap::originMarker() const
{
    return m_originMarker;
}

Marker *NPointStrap::insertionMarker() const
{
    return m_insertionMarker;
}

void NPointStrap::calculate()
{
    PointForce *theOrigin = (*pointForceList())[0].get();
    PointForce *theInsertion = (*pointForceList())[1].get();
    unsigned int i;
    pgd::Vector3 v;

    // calculate the world positions
    pgd::Vector3 origin = m_originMarker->worldPosition();
    theOrigin->point[0] = origin.x;
    theOrigin->point[1] = origin.y;
    theOrigin->point[2] = origin.z;
    pgd::Vector3 insertion = m_insertionMarker->worldPosition();
    theInsertion->point[0] = insertion.x;
    theInsertion->point[1] = insertion.y;
    theInsertion->point[2] = insertion.z;
    for (i = 0; i < m_viaPointMarkerList.size(); i++)
    {
        v = m_viaPointMarkerList[i]->worldPosition();
        (*pointForceList())[i + 2]->point[0] = v.x;
        (*pointForceList())[i + 2]->point[1] = v.y;
        (*pointForceList())[i + 2]->point[2] = v.z;
    }

    std::unique_ptr<unsigned int[]> mapping = std::make_unique<unsigned int[]>(pointForceList()->size());
    for (i = 0; i < pointForceList()->size(); i++)
    {
        if (i == 0)
        {
            mapping[i] = 0;
        }
        else
        {
            if (i == pointForceList()->size() - 1) mapping[i] = 1;
            else mapping[i] = i + 1;
        }
    }

    pgd::Vector3 line, line2;
    double totalLength = 0;
    double len;
    for (i = 0; i < pointForceList()->size(); i++)
    {
        if (i == 0)
        {
            line.x = (*pointForceList())[mapping[i + 1]]->point[0] - (*pointForceList())[mapping[i]]->point[0];
            line.y = (*pointForceList())[mapping[i + 1]]->point[1] - (*pointForceList())[mapping[i]]->point[1];
            line.z = (*pointForceList())[mapping[i + 1]]->point[2] - (*pointForceList())[mapping[i]]->point[2];
            len = line.magnitude();
            totalLength += len;
            line /= len;
        }
        else if (i == pointForceList()->size() - 1)
        {
            line.x = (*pointForceList())[mapping[i - 1]]->point[0] - (*pointForceList())[mapping[i]]->point[0];
            line.y = (*pointForceList())[mapping[i - 1]]->point[1] - (*pointForceList())[mapping[i]]->point[1];
            line.z = (*pointForceList())[mapping[i - 1]]->point[2] - (*pointForceList())[mapping[i]]->point[2];
            line.normalize();
        }
        else
        {
            line.x = (*pointForceList())[mapping[i + 1]]->point[0] - (*pointForceList())[mapping[i]]->point[0];
            line.y = (*pointForceList())[mapping[i + 1]]->point[1] - (*pointForceList())[mapping[i]]->point[1];
            line.z = (*pointForceList())[mapping[i + 1]]->point[2] - (*pointForceList())[mapping[i]]->point[2];
            len = line.magnitude();
            totalLength += len;
            line /= len;
            line2.x = (*pointForceList())[mapping[i - 1]]->point[0] - (*pointForceList())[mapping[i]]->point[0];
            line2.y = (*pointForceList())[mapping[i - 1]]->point[1] - (*pointForceList())[mapping[i]]->point[1];
            line2.z = (*pointForceList())[mapping[i - 1]]->point[2] - (*pointForceList())[mapping[i]]->point[2];
            line2.normalize();
            line += line2;
        }

        (*pointForceList())[mapping[i]]->vector[0] = line.x;
        (*pointForceList())[mapping[i]]->vector[1] = line.y;
        (*pointForceList())[mapping[i]]->vector[2] = line.z;
    }

    if (length() >= 0 && simulation() && simulation()->global()->stepSize() > 0) setVelocity((totalLength - length()) / simulation()->global()->stepSize());
    else setVelocity(0);
    setLength(totalLength);

    // check that we don't have any non-normal values for directions which can occur if points co-locate
    for (size_t i = 0; i < pointForceList()->size(); i++)
    {
        if ((std::isfinite((*pointForceList())[i]->vector[0]) && std::isfinite((*pointForceList())[i]->vector[1]) && std::isfinite((*pointForceList())[i]->vector[2])) == false)
        {
            (*pointForceList())[i]->vector[0] = 0.0;
            (*pointForceList())[i]->vector[1] = 0.0;
            (*pointForceList())[i]->vector[2] = 0.0;
            std::cerr << "Warning: point force direction in \"" << name() << "\" is invalid so applying standard fixup\n";
        }
    }
}

std::string *NPointStrap::createFromAttributes()
{
    if (Strap::createFromAttributes()) return lastErrorPtr();

    std::string buf;

    if (findAttribute("OriginMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto originMarker = simulation()->markerList()->find(buf);
    if (originMarker == simulation()->markerList()->end())
    {
        setLastError("STRAP ID=\""s + name() +"\" OriginMarker not found"s);
        return lastErrorPtr();
    }
    this->setOrigin(originMarker->second.get());
    if (findAttribute("InsertionMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto insertionMarker = simulation()->markerList()->find(buf);
    if (insertionMarker == simulation()->markerList()->end())
    {
        setLastError("STRAP ID=\""s + name() +"\" InsertionMarker not found"s);
        return lastErrorPtr();
    }
    this->setInsertion(insertionMarker->second.get());

    if (findAttribute("ViaPointMarkerIDList"s, &buf) == nullptr) return lastErrorPtr();
    std::vector<std::string> result;
    pystring::split(buf, result);
    if (result.empty())
    {
        setLastError("STRAP ID=\""s + name() +"\" ViaPointMarkerID list empty"s);
        return lastErrorPtr();
    }
    std::vector<Marker *> viaPointMarkerList;
    viaPointMarkerList.reserve(result.size());
    for (size_t i = 0; i < result.size(); i++)
    {
        auto viaPointMarker = simulation()->markerList()->find(result[i]);
        if (viaPointMarker == simulation()->markerList()->end())
        {
            setLastError("STRAP ID=\""s + name() +"\" via point marker \""s + result[i] +"\" not found"s);
            return lastErrorPtr();
        }
        viaPointMarkerList.push_back(viaPointMarker->second.get());
    }
    this->setViaPoints(&viaPointMarkerList);

    std::vector<NamedObject *> upstreamObjects;
    upstreamObjects.reserve(viaPointMarkerList.size() + 2);
    upstreamObjects.push_back(m_originMarker);
    upstreamObjects.push_back(m_insertionMarker);
    for (auto &&it : viaPointMarkerList) upstreamObjects.push_back(it);
    setUpstreamObjects(std::move(upstreamObjects));
    return nullptr;
}

void NPointStrap::appendToAttributes()
{
    Strap::appendToAttributes();
    setAttribute("Type"s, "NPoint"s);
    setAttribute("OriginMarkerID"s, m_originMarker->name());
    setAttribute("InsertionMarkerID"s, m_insertionMarker->name());
    std::vector<std::string> markerNames;
    markerNames.reserve(m_viaPointMarkerList.size());
    for (size_t i = 0; i < m_viaPointMarkerList.size(); i++) markerNames.push_back(m_viaPointMarkerList[i]->name());
    std::string viaPointMarkerList = pystring::join(" "s, markerNames);
    setAttribute("ViaPointMarkerIDList"s, viaPointMarkerList);
}


} // namespace GaitSym


