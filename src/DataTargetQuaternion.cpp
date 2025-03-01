/*
 *  DataTargetQuaternion.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on Tue July 14 2009.
 *  Copyright (c) 1009 Bill Sellers. All rights reserved.
 *
 */

#include "DataTargetQuaternion.h"
#include "Body.h"
#include "PGDMath.h"
#include "GSUtil.h"
#include "Simulation.h"
#include "Geom.h"
#include "Marker.h"

#include "pystring.h"

#include <sstream>
#include <algorithm>

using namespace std::string_literals;
namespace GaitSym {

DataTargetQuaternion::DataTargetQuaternion()
{
}

// returns the degree of match to the stored values
// in this case this is the angle between the two quaternions
double DataTargetQuaternion::calculateError(size_t valueListIndex)
{
    pgd::Quaternion q;
    double angle = 0;
    if (valueListIndex >= m_qValueList.size())
    {
        std::cerr << "Warning: DataTargetQuaternion::GetMatchValue valueListIndex out of range\n";
        return 0;
    }

    while (true)
    {
        if (Body *body = dynamic_cast<Body *>(GetTarget()))
        {
            q = body->GetQuaternion();
            angle = pgd::FindAngle(m_qValueList[size_t(valueListIndex)], q);
            break;
        }
        if (Geom *geom = dynamic_cast<Geom *>(GetTarget()))
        {
            q = geom->GetWorldQuaternion();
            angle = pgd::FindAngle(m_qValueList[size_t(valueListIndex)], q);
            break;
        }
        std::cerr << "DataTargetQuaternion target missing error " << name() << "\n";
        break;
    }
    return angle;
}

// returns the degree of match to the stored values
// in this case this is the angle between the two quaternions
double DataTargetQuaternion::calculateError(size_t index, size_t indexNext, double time)
{
    pgd::Quaternion q;
    double angle = 0;

    // do a slerp interpolation between the target quaternions
    double delTime = (*targetTimeList())[size_t(indexNext)] - (*targetTimeList())[size_t(index)];
    double interpolationFraction;
    if (delTime < std::numeric_limits<double>::epsilon()) interpolationFraction = 0;
    else interpolationFraction = (time - (*targetTimeList())[size_t(index)]) / delTime;
    pgd::Quaternion interpolatedTarget = pgd::slerp(m_qValueList[size_t(index)], m_qValueList[size_t(indexNext)], interpolationFraction);

    while (true)
    {
        if (Body *body = dynamic_cast<Body *>(GetTarget()))
        {
            q = body->GetQuaternion();
            angle = pgd::FindAngle(interpolatedTarget, q);
            break;
        }
        if (Geom *geom = dynamic_cast<Geom *>(GetTarget()))
        {
            q = geom->GetWorldQuaternion();
            angle = pgd::FindAngle(interpolatedTarget, q);
            break;
        }
        std::cerr << "DataTargetQuaternion target missing error " << name() << "\n";
        break;
    }
    return angle;
}

std::string DataTargetQuaternion::dumpToString()
{
    std::stringstream ss;
    ss.precision(17);
    ss.setf(std::ios::scientific);
    if (firstDump())
    {
        setFirstDump(false);
        ss << "Time\tTargetQW\tTargetQX\tTargetQY\tTargetQZ\tActualQW\tActualQX\tActualQY\tActualQZ\tAngle\n";
    }
    Body *body;
    Geom *geom;
    double angle = 0;
    pgd::Quaternion q;

    size_t valueListIndex = 0;
    auto lowerBounds = std::lower_bound(targetTimeList()->begin(), targetTimeList()->end(), simulation()->GetTime());
    if (lowerBounds != targetTimeList()->end()) valueListIndex = std::distance(targetTimeList()->begin(), lowerBounds);

    if ((body = dynamic_cast<Body *>(GetTarget())) != nullptr)
    {
        q = body->GetQuaternion();
        angle = pgd::FindAngle(m_qValueList[size_t(valueListIndex)], q);
    }
    else if ((geom = dynamic_cast<Geom *>(GetTarget())) != nullptr)
    {
        q = geom->GetWorldQuaternion();
        angle = pgd::FindAngle(m_qValueList[size_t(valueListIndex)], q);
    }

    ss << simulation()->GetTime() <<
          "\t" << m_qValueList[size_t(valueListIndex)].n << "\t" << m_qValueList[size_t(valueListIndex)].x << "\t" << m_qValueList[size_t(valueListIndex)].y << "\t" << m_qValueList[size_t(valueListIndex)].z <<
          "\t" << q[0] << "\t" << q[1] << "\t" << q[2] << "\t" << q[3] <<
          "\t" << angle <<
          "\n";
    return ss.str();
}

void DataTargetQuaternion::SetTarget(NamedObject *target)
{
    m_target = target;
}

NamedObject *DataTargetQuaternion::GetTarget()
{
    return m_target;
}

// this function initialises the data in the object based on the contents
// of an xml_node node. It uses information from the simulation as required
// to satisfy dependencies
// it returns nullptr on success and a pointer to lastError() on failure
std::string *DataTargetQuaternion::createFromAttributes()
{
    if (DataTarget::createFromAttributes()) return lastErrorPtr();

    std::string buf;
    // if (findAttribute("DataType"s, &buf) == nullptr) return lastErrorPtr();
    m_target = nullptr;
    if (findAttribute("TargetID"s, &buf) == nullptr) return lastErrorPtr();
    for (bool once = true; once; once = false)
    {
        auto iterBody = simulation()->GetBodyList()->find(buf);
        if (iterBody != simulation()->GetBodyList()->end()) { m_target = iterBody->second.get(); break; }
        auto iterGeom = simulation()->GetGeomList()->find(buf);
        if (iterGeom != simulation()->GetGeomList()->end()) { m_target = iterGeom->second.get(); break; }
        auto iterMarker = simulation()->GetMarkerList()->find(buf);
        if (iterMarker != simulation()->GetMarkerList()->end()) { m_target = iterMarker->second.get(); break; }
    }
    if (!m_target)
    {
        setLastError("DataTargetQuaternion ID=\""s + name() +"\" TargetID not found "s + buf);
        return lastErrorPtr();
    }

    if (findAttribute("TargetValues"s, &buf) == nullptr) return lastErrorPtr();
    std::vector<std::string> targetValuesTokens;
    pystring::split(buf, targetValuesTokens);
    if (targetValuesTokens.size() == 0)
    {
        setLastError("DataTargetQuaternion ID=\""s + name() +"\" No values found in TargetValues"s);
        return lastErrorPtr();
    }
    if (targetValuesTokens.size() != targetTimeList()->size() * 4)
    {
        setLastError("DataTargetQuaternion ID=\""s + name() +"\" Number of values in TargetValues does not match 4 * TargetTimes"s);
        return lastErrorPtr();
    }
    m_qValueList.clear();
    m_qValueList.reserve(targetTimeList()->size());
    for (size_t i = 0; i < targetTimeList()->size(); i++)
    {
        pgd::Quaternion q(GSUtil::Double(targetValuesTokens[i * 4]), GSUtil::Double(targetValuesTokens[i * 4 + 1]),
                          GSUtil::Double(targetValuesTokens[i * 4 + 2]), GSUtil::Double(targetValuesTokens[i * 4 + 3]));
        m_qValueList.push_back(q);
    }

    if (m_target) setUpstreamObjects({m_target});
    return nullptr;
}

// this function appends data to a pre-existing xml_node - often created by XMLSave
void DataTargetQuaternion::appendToAttributes()
{
    DataTarget::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Quaternion"s);
    std::vector<double> valueList;
    valueList.reserve(m_qValueList.size() * 4);
    for (size_t i = 0; i < m_qValueList.size(); i++)
    {
        valueList.push_back(m_qValueList[i].n);
        valueList.push_back(m_qValueList[i].x);
        valueList.push_back(m_qValueList[i].y);
        valueList.push_back(m_qValueList[i].z);
    }
    setAttribute("TargetValues"s, *GSUtil::ToString(valueList.data(), valueList.size(), &buf));
    setAttribute("TargetID"s, m_target->name());
}

} // namespace GaitSym


