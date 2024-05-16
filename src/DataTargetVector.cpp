/*
 *  DataTargetVector.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on Tue July 14 2009.
 *  Copyright (c) 1009 Bill Sellers. All rights reserved.
 *
 */

#include "DataTargetVector.h"
#include "Body.h"
#include "PGDMath.h"
#include "GSUtil.h"
#include "HingeJoint.h"
#include "BallJoint.h"
#include "UniversalJoint.h"
#include "Marker.h"
#include "Geom.h"
#include "Simulation.h"

#include "pystring.h"

#include <sstream>
#include <algorithm>

using namespace std::string_literals;

DataTargetVector::DataTargetVector()
{
}


// returns the degree of match to the stored values
// in this case this is the euclidean distance between the two vectors
double DataTargetVector::calculateError(size_t valueListIndex)
{
    pgd::Vector3 r;
    double err = 0;
    pgd::Vector3 v;
    if (valueListIndex >= m_VValueList.size())
    {
        std::cerr << "Warning: DataTargetVector::GetMatchValue valueListIndex out of range\n";
        return 0;
    }

    while (true)
    {
        if (Body *body = dynamic_cast<Body *>(GetTarget()))
        {
            r = body->GetPosition();
            err = (pgd::Vector3(r[0], r[1], r[2]) - m_VValueList[size_t(valueListIndex)]).Magnitude();
            break;
        }
        if (Geom *geom = dynamic_cast<Geom *>(GetTarget()))
        {
            v = geom->GetWorldPosition();
            err = (pgd::Vector3(v[0], v[1], v[2]) - m_VValueList[size_t(valueListIndex)]).Magnitude();
            break;
        }
        if (HingeJoint *hingeJoint = dynamic_cast<HingeJoint *>(GetTarget()))
        {
            v = hingeJoint->anchor();
            err = (pgd::Vector3(v[0], v[1], v[2]) - m_VValueList[size_t(valueListIndex)]).Magnitude();
            break;
        }
        if (BallJoint *ballJoint = dynamic_cast<BallJoint *>(GetTarget()))
        {
            v = ballJoint->anchor();
            err = (pgd::Vector3(v[0], v[1], v[2]) - m_VValueList[size_t(valueListIndex)]).Magnitude();
            break;
        }
        if (UniversalJoint *universalJoint = dynamic_cast<UniversalJoint *>(GetTarget()))
        {
            v = universalJoint->anchor();
            err = (pgd::Vector3(v[0], v[1], v[2]) - m_VValueList[size_t(valueListIndex)]).Magnitude();
            break;
        }
        if (Marker *marker = dynamic_cast<Marker *>(GetTarget()))
        {
            pgd::Vector3 vec = marker->GetWorldPosition();
            err = (vec - m_VValueList[size_t(valueListIndex)]).Magnitude();
            break;
        }
        std::cerr << "DataTargetVector target missing error " << name() << "\n";
        break;
    }
    return err;
}

// returns the degree of match to the stored values
// in this case this is the euclidean distance between the two vectors
double DataTargetVector::calculateError(double time)
{
    pgd::Vector3 r;
    double err = 0;
    pgd::Vector3 v;

    size_t index, indexNext;
    auto lowerBound = std::lower_bound(targetTimeList()->begin(), targetTimeList()->end(), time);
    auto upperBound = std::upper_bound(targetTimeList()->begin(), targetTimeList()->end(), time);
    if (lowerBound == targetTimeList()->begin()) // time <= lowest value in the list
    {
        index = 0;
        indexNext = index;
    }
    else if (upperBound == targetTimeList()->end()) // time > highest value in the list
    {
        index = targetTimeList()->size() - 1;
        indexNext = index;
    }
    else
    {
        index = std::distance(targetTimeList()->begin(), lowerBound) - 1; // subtracting 1 because lower bound gives an index 1 higher than expected (IMO)
        indexNext = std::min(index + 1, targetTimeList()->size() - 1);
    }

    double interpX = GSUtil::Interpolate((*targetTimeList())[size_t(index)], m_VValueList[size_t(index)].x, (*targetTimeList())[size_t(indexNext)], m_VValueList[size_t(indexNext)].x, time);
    double interpY = GSUtil::Interpolate((*targetTimeList())[size_t(index)], m_VValueList[size_t(index)].y, (*targetTimeList())[size_t(indexNext)], m_VValueList[size_t(indexNext)].y, time);
    double interpZ = GSUtil::Interpolate((*targetTimeList())[size_t(index)], m_VValueList[size_t(index)].z, (*targetTimeList())[size_t(indexNext)], m_VValueList[size_t(indexNext)].z, time);
    pgd::Vector3 interpolatedTarget(interpX, interpY, interpZ);

    while (true)
    {
        if (Body *body = dynamic_cast<Body *>(GetTarget()))
        {
            r = body->GetPosition();
            err = (pgd::Vector3(r[0], r[1], r[2]) - interpolatedTarget).Magnitude();
            break;
        }
        if (Geom *geom = dynamic_cast<Geom *>(GetTarget()))
        {
            v = geom->GetWorldPosition();
            err = (pgd::Vector3(v[0], v[1], v[2]) - interpolatedTarget).Magnitude();
            break;
        }
        if (HingeJoint *hingeJoint = dynamic_cast<HingeJoint *>(GetTarget()))
        {
            v = hingeJoint->anchor();
            err = (pgd::Vector3(v[0], v[1], v[2]) - interpolatedTarget).Magnitude();
            break;
        }
        if (BallJoint *ballJoint = dynamic_cast<BallJoint *>(GetTarget()))
        {
            v = ballJoint->anchor();
            err = (pgd::Vector3(v[0], v[1], v[2]) - interpolatedTarget).Magnitude();
            break;
        }
        if (UniversalJoint *universalJoint = dynamic_cast<UniversalJoint *>(GetTarget()))
        {
            v = universalJoint->anchor();
            err = (pgd::Vector3(v[0], v[1], v[2]) - interpolatedTarget).Magnitude();
            break;
        }
        if (Marker *marker = dynamic_cast<Marker *>(GetTarget()))
        {
            pgd::Vector3 vec = marker->GetWorldPosition();
            err = (vec - interpolatedTarget).Magnitude();
            break;
        }
        std::cerr << "DataTargetVector target missing error " << name() << "\n";
        break;
    }
    return err;
}

std::string DataTargetVector::dumpToString()
{
    std::stringstream ss;
    ss.precision(17);
    ss.setf(std::ios::scientific);
    if (firstDump())
    {
        setFirstDump(false);
        ss << "Time\tTargetX\tTargetY\tTargetZ\tActualX\tActualY\tActualZ\tDistance\n";
    }
    pgd::Vector3 r;
    double err = 0;
    pgd::Vector3 v;

    size_t valueListIndex = 0;
    auto lowerBounds = std::lower_bound(targetTimeList()->begin(), targetTimeList()->end(), simulation()->GetTime());
    if (lowerBounds != targetTimeList()->end()) valueListIndex = std::distance(targetTimeList()->begin(), lowerBounds);

    while (true)
    {
        if (Body *body = dynamic_cast<Body *>(GetTarget()))
        {
            r = body->GetPosition();
            err = (pgd::Vector3(r[0], r[1], r[2]) - m_VValueList[size_t(valueListIndex)]).Magnitude();
            break;
        }
        if (Geom *geom = dynamic_cast<Geom *>(GetTarget()))
        {
            v = geom->GetWorldPosition();
            err = (pgd::Vector3(v[0], v[1], v[2]) - m_VValueList[size_t(valueListIndex)]).Magnitude();
            r = v;
            break;
        }
        if (HingeJoint *hingeJoint = dynamic_cast<HingeJoint *>(GetTarget()))
        {
            v = hingeJoint->anchor();
            err = (pgd::Vector3(v[0], v[1], v[2]) - m_VValueList[size_t(valueListIndex)]).Magnitude();
            r = v;
            break;
        }
        if (BallJoint *ballJoint = dynamic_cast<BallJoint *>(GetTarget()))
        {
            v = ballJoint->anchor();
            err = (pgd::Vector3(v[0], v[1], v[2]) - m_VValueList[size_t(valueListIndex)]).Magnitude();
            r = v;
            break;
        }
        if (UniversalJoint *universalJoint = dynamic_cast<UniversalJoint *>(GetTarget()))
        {
            v = universalJoint->anchor();
            err = (pgd::Vector3(v[0], v[1], v[2]) - m_VValueList[size_t(valueListIndex)]).Magnitude();
            r = v;
            break;
        }
        if (Marker *marker = dynamic_cast<Marker *>(GetTarget()))
        {
            pgd::Vector3 vec = marker->GetWorldPosition();
            v[0] = vec.x; v[1] = vec.y; v[2] = vec.z;
            err = (pgd::Vector3(v[0], v[1], v[2]) - m_VValueList[size_t(valueListIndex)]).Magnitude();
            r = v;
            break;
        }
    }

    ss << simulation()->GetTime() <<
          "\t" << m_VValueList[size_t(valueListIndex)].x << "\t" << m_VValueList[size_t(valueListIndex)].y << "\t" << m_VValueList[size_t(valueListIndex)].z <<
          "\t" << r[0] << "\t" << r[1] << "\t" << r[2] <<
          "\t" << err <<
          "\n";
    return ss.str();
}

void DataTargetVector::SetTarget(NamedObject *target)
{
    m_Target = target;
}

NamedObject *DataTargetVector::GetTarget()
{
    return m_Target;
}

// this function initialises the data in the object based on the contents
// of an xml_node node. It uses information from the simulation as required
// to satisfy dependencies
// it returns nullptr on success and a pointer to lastError() on failure
std::string *DataTargetVector::createFromAttributes()
{
    if (DataTarget::createFromAttributes()) return lastErrorPtr();

    std::string buf;
    if (findAttribute("DataType"s, &buf) == nullptr) return lastErrorPtr();
    m_Target = nullptr;
    if (findAttribute("TargetID"s, &buf) == nullptr) return lastErrorPtr();
    for (bool once = true; once; once = false)
    {
        auto iterBody = simulation()->GetBodyList()->find(buf);
        if (iterBody != simulation()->GetBodyList()->end()) { m_Target = iterBody->second.get(); break; }
        auto iterJoint = simulation()->GetJointList()->find(buf);
        if (iterJoint != simulation()->GetJointList()->end()) { m_Target = iterJoint->second.get(); break; }
        auto iterGeom = simulation()->GetGeomList()->find(buf);
        if (iterGeom != simulation()->GetGeomList()->end()) { m_Target = iterGeom->second.get(); break; }
        auto iterMarker = simulation()->GetMarkerList()->find(buf);
        if (iterMarker != simulation()->GetMarkerList()->end()) { m_Target = iterMarker->second.get(); break; }
    }
    if (!m_Target)
    {
        setLastError("DataTargetVector ID=\""s + name() +"\" TargetID not found "s + buf);
        return lastErrorPtr();
    }

    if (findAttribute("TargetValues"s, &buf) == nullptr) return lastErrorPtr();
    std::vector<std::string> targetValuesTokens;
    pystring::split(buf, targetValuesTokens);
    if (targetValuesTokens.size() == 0)
    {
        setLastError("DataTargetVector ID=\""s + name() +"\" No values found in TargetValues"s);
        return lastErrorPtr();
    }
    if (targetValuesTokens.size() != targetTimeList()->size() * 3)
    {
        setLastError("DataTargetVector ID=\""s + name() +"\" Number of values in TargetValues does not match 3 * TargetTimes"s);
        return lastErrorPtr();
    }
    m_VValueList.clear();
    m_VValueList.reserve(targetTimeList()->size());
    for (size_t i = 0; i < targetTimeList()->size(); i++)
    {
        pgd::Vector3 v(GSUtil::Double(targetValuesTokens[i * 3]), GSUtil::Double(targetValuesTokens[i * 3 + 1]), GSUtil::Double(targetValuesTokens[i * 3 + 2]));
        m_VValueList.push_back(v);
    }

    if (m_Target) setUpstreamObjects({m_Target});
    return nullptr;
}

// this function appends data to a pre-existing xml_node - often created by XMLSave
void DataTargetVector::appendToAttributes()
{
    DataTarget::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Scalar"s);
    std::vector<double> valueList;
    valueList.reserve(m_VValueList.size() * 3);
    for (size_t i = 0; i < m_VValueList.size(); i++)
    {
        valueList.push_back(m_VValueList[i].x);
        valueList.push_back(m_VValueList[i].y);
        valueList.push_back(m_VValueList[i].z);
    }
    setAttribute("TargetValues"s, *GSUtil::ToString(valueList.data(), valueList.size(), &buf));
    setAttribute("TargetID"s, m_Target->name());
}

