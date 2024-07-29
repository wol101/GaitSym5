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
    if (m_forceType == SimpleForce)
    {
        pgd::Vector3 p, v;
        for (size_t i = 0; i < m_markerList.size(); i++)
        {
            PointForce *pointForce = GetPointForceList()->at(i).get();
            pointForce->body = m_markerList[i]->GetBody();
            // the positions of the forces are always the world positions of the markers
            p = m_markerList[i]->GetWorldPosition();
            pointForce->point = p;
            // and the directions depend on the ForceDirection setting
            // it might appear a bit strange but body relative uses the actual axis currently
            // whereas world relative treats the specified axis as if it were world
            if (m_forceDirection == BodyRelative) { v = m_markerList[i]->GetWorldAxis(Marker::X); }
            if (m_forceDirection == WorldRelative) { v = m_markerList[i]->GetAxis(Marker::X); }
            pointForce->vector = v;
        }
        return;
    }
    if (m_forceType == TorqueCouple)
    {
        pgd::Vector3 p, x, y, z;
        for (size_t i = 0; i < m_markerList.size(); i++)
        {
            PointForce *pointForce = GetPointForceList()->at(i * 2).get();
            pointForce->body = m_markerList[i]->GetBody();
            // the positions of the forces are always the world positions of the markers
            p = m_markerList[i]->GetWorldPosition();
            // and the directions depend on the ForceDirection setting
            // it might appear a bit strange but body relative uses the actual axis currently
            // whereas world relative treats the specified axis as if it were world
            if (m_forceDirection == BodyRelative) { m_markerList[i]->GetWorldBasis(&x, &y, &z); }
            if (m_forceDirection == WorldRelative) { m_markerList[i]->GetBasis(&x, &y, &z); }
            // we are rotating around the x axis
            // so the position needs to be offset by 0.5 in the y axis
            // and the force applied in the z axis
            pointForce->point = p + (0.5 * y);
            pointForce->vector = z;
            // and now the second component of the couple
            pointForce = GetPointForceList()->at(i * 2 + 1).get();
            pointForce->body = m_markerList[i]->GetBody();
            pointForce->point = p + (-0.5 * y); // offset by -0.5 in the y axis
            pointForce->vector = -1 * z; // force applied in the -z axis
        }
        return;
    }
}

std::string *MagicStrap::createFromAttributes()
{
    if (Strap::createFromAttributes()) return lastErrorPtr();

    std::string buf;
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

    if (findAttribute("ForceType"s, &buf) == nullptr) return lastErrorPtr();
    size_t forceTypeIndex;
    for (forceTypeIndex = 0; forceTypeIndex < forceTypeCount; ++forceTypeIndex)
    {
        if (buf == forceTypeStrings(forceTypeIndex))
        {
            m_forceType = static_cast<ForceType>(forceTypeIndex);
            break;
        }
    }
    if (!(forceTypeIndex < forceTypeCount))
    {
        setLastError("STRAP ID=\""s + name() +"\" Unrecognised ForceType "s + buf);
        return lastErrorPtr();
    }


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
    if (m_forceType == SimpleForce) { GetPointForceList()->reserve(result.size()); }
    else { GetPointForceList()->reserve(result.size() * 2); }
    for (size_t i = 0; i < result.size(); i++)
    {
        auto marker = simulation()->GetMarker(result[i]);
        if (!marker)
        {
            setLastError("STRAP ID=\""s + name() +"\" marker \""s + result[i] +"\" not found"s);
            return lastErrorPtr();
        }
        m_markerList.push_back(marker);
        if (m_forceType == SimpleForce) { GetPointForceList()->push_back(std::make_unique<PointForce>()); }
        else  { GetPointForceList()->push_back(std::make_unique<PointForce>()); GetPointForceList()->push_back(std::make_unique<PointForce>()); }
    }

    std::vector<NamedObject *> upstreamObjects;
    upstreamObjects.reserve(m_markerList.size());
    for (auto &&it : m_markerList) upstreamObjects.push_back(it);
    setUpstreamObjects(std::move(upstreamObjects));


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
    setAttribute("ForceType", forceTypeStrings(m_forceType));
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
        }
        s += pystring::join("\t"s, items);
        s += "\n"s;
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
