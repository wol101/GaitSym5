/*
 *  MagicStrap.cpp
 *  GaitSym5
 *
 *  Created by Bill Sellers on 03/06/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#include "MagicStrap.h"

#include "GSUtil.h"
#include "Marker.h"

#include "pystring.h"

namespace GaitSym {

MagicStrap::MagicStrap() {}

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
    m_markerList.reserve(result.size());
    for (size_t i = 0; i < result.size(); i++)
    {
        auto viaPointMarker = simulation()->GetMarkerList()->find(result[i]);
        if (viaPointMarker == simulation()->GetMarkerList()->end())
        {
            setLastError("STRAP ID=\""s + name() +"\" marker \""s + result[i] +"\" not found"s);
            return lastErrorPtr();
        }
        m_markerList.push_back(viaPointMarker->second.get());
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
}

} // GaitSym
