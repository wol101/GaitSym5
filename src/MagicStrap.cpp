/*
 *  MagicStrap.cpp
 *  GaitSym5
 *
 *  Created by Bill Sellers on 03/06/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#include "MagicStrap.h"

#include "Marker.h"
#include "GSUtil.h"
#include "Body.h"

#include "pystring.h"

namespace GaitSym {

MagicStrap::MagicStrap()
{
}


void MagicStrap::Calculate()
{
    pgd::Vector3 p, v;
    for (size_t i = 0; i < m_markerList.size(); i++)
    {
        // the positions of the forces are always the world positions of the markers
        p = m_markerList[i]->GetWorldPosition();
        (*GetPointForceList())[i]->point = p;
        // and the directions depend on the ForceDirection setting
        if (m_forceDirection == WorldRelative) { v = m_markerList[i]->GetWorldAxis(Marker::X); }
        if (m_forceDirection == BodyRelative) { v = m_markerList[i]->GetAxis(Marker::X); }
        (*GetPointForceList())[i]->vector = v;
    }
}

std::string *MagicStrap::createFromAttributes()
{
    if (Strap::createFromAttributes()) return lastErrorPtr();

    std::string buf;
    if (findAttribute("MarkerIDList"s, &buf) == nullptr) return lastErrorPtr();
    std::vector<std::string> result;
    pystring::split(buf, result);
    if (result.empty())
    {
        setLastError("STRAP ID=\""s + name() +"\" MarkerIDList list empty"s);
        return lastErrorPtr();
    }
    m_markerList.clear();
    m_markerList.reserve(result.size());
    GetPointForceList()->clear();
    GetPointForceList()->reserve(result.size());
    for (size_t i = 0; i < result.size(); i++)
    {
        auto marker = simulation()->GetMarker(result[i]);
        if (!marker)
        {
            setLastError("STRAP ID=\""s + name() +"\" marker \""s + result[i] +"\" not found"s);
            return lastErrorPtr();
        }
        m_markerList.push_back(marker);
        auto pointForce = std::make_unique<PointForce>();
        pointForce->body = marker->GetBody();
        pointForce->point = marker->GetWorldPosition();
        if (m_forceDirection == WorldRelative) { pointForce->vector = marker->GetWorldAxis(Marker::X); }
        if (m_forceDirection == BodyRelative) { pointForce->vector = marker->GetAxis(Marker::X); }
        GetPointForceList()->push_back(std::move(pointForce));
    }

    std::vector<NamedObject *> upstreamObjects;
    upstreamObjects.reserve(m_markerList.size());
    for (auto &&it : m_markerList) upstreamObjects.push_back(it);
    setUpstreamObjects(std::move(upstreamObjects));

    if (findAttribute("ForceDirection"s, &buf) == nullptr) return lastErrorPtr();
    size_t forceDirectionIndex;
    for (forceDirectionIndex = 0; forceDirectionIndex < forceDirectionCount; ++forceDirectionIndex)
    {
        if (buf == forceDirectionStrings(forceDirectionIndex))
        {
            m_forceDirection = static_cast<ForceDirection>(forceDirectionIndex);
            break;
        }
    }
    if (!(forceDirectionIndex < forceDirectionCount))
    {
        setLastError("STRAP ID=\""s + name() +"\" Unrecognised ForceDirection "s + buf);
        return lastErrorPtr();
    }

    return nullptr;
}

void MagicStrap::appendToAttributes()
{
    Strap::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Magic"s);
    std::vector<std::string> markerNames;
    markerNames.reserve(m_markerList.size());
    for (auto &&it : m_markerList) markerNames.push_back(it->name());
    std::string markerIDList = pystring::join(" "s, markerNames);
    setAttribute("MarkerIDList"s, markerIDList);
    setAttribute("ForceDirection", forceDirectionStrings(m_forceDirection));
}

std::string MagicStrap::dumpToString()
{
    std::string s;
    std::vector<std::string> items;
    if (firstDump())
    {
        setFirstDump(false);
        items.push_back("time"s);
        for (size_t i = 0; i < GetPointForceList()->size(); ++i)
        {
            items.push_back(GSUtil::ToString("body%zu", i));
            items.push_back(GSUtil::ToString("px%zu", i));
            items.push_back(GSUtil::ToString("py%zu", i));
            items.push_back(GSUtil::ToString("pz%zu", i));
            items.push_back(GSUtil::ToString("vx%zu", i));
            items.push_back(GSUtil::ToString("vy%zu", i));
            items.push_back(GSUtil::ToString("vz%zu", i));
            s += pystring::join("\t"s, items);
            s += "\n"s;
        }
    }
    items.clear();
    items.push_back(GSUtil::ToString(simulation()->GetTime()));
    for (auto &&it : *GetPointForceList())
    {
        if (it->body) { items.push_back(it->body->name()); }
        else { items.push_back("World"s); }
        items.push_back(GSUtil::ToString(it->point.x));
        items.push_back(GSUtil::ToString(it->point.y));
        items.push_back(GSUtil::ToString(it->point.z));
        items.push_back(GSUtil::ToString(it->vector.x));
        items.push_back(GSUtil::ToString(it->vector.y));
        items.push_back(GSUtil::ToString(it->vector.z));
    }
    s += pystring::join("\t"s, items);
    s += "\n"s;
    return s;
}

} // GaitSym
