/**
 *  @file DataTargetScalar.cpp
 *  @author Bill Sellers
 *  @date Tue July 14 2009.
 *  @copyright Copyright (c) 2009 Bill Sellers. All rights reserved.
 *  @brief Data target that produces a score based on a scalar value.
 *
 *  See the createFromAttributes description for details of the XML specification.
 *
 *  See the description of dumpToString for details of the output file format for this element.
 */

#include "DataTargetScalar.h"
#include "Body.h"
#include "HingeJoint.h"
#include "BallJoint.h"
#include "UniversalJoint.h"
#include "Marker.h"
#include "Geom.h"
#include "GSUtil.h"
#include "Simulation.h"
#include "TegotaeDriver.h"

#include "pystring.h"

#include <algorithm>

using namespace std::string_literals;
namespace GaitSym {

DataTargetScalar::DataTargetScalar()
{
}

DataTargetScalar::~DataTargetScalar()
{
}

// returns the difference between the target actual value and the desired value (actual - desired)
double DataTargetScalar::calculateError(size_t index)
{
    if (index >= m_valueList.size())
    {
        std::cerr << "Warning: DataTargetScalar::GetMatchValue index out of range\n";
        return 0;
    }

    double value = m_valueList[size_t(index)];
    return calculateErrorScore(value);
}


double DataTargetScalar::calculateErrorScore(double value)
{
    m_errorScore = 0;
    pgd::Vector3 r;
    pgd::Quaternion rq;
    pgd::Vector3 result;
    pgd::Quaternion q;
    pgd::Quaternion pq;
    pgd::Vector3 pv;
    std::vector<Contact *> *contactList;
    double force;

    while (true)
    {
        if (Body *body = dynamic_cast<Body *>(GetTarget()))
        {
            switch (m_dataType)
            {
            case Q0:
                rq = body->GetQuaternion();
                m_errorScore = (rq[0] - value);
                break;
            case Q1:
                rq = body->GetQuaternion();
                m_errorScore = (rq[1] - value);
                break;
            case Q2:
                rq = body->GetQuaternion();
                m_errorScore = (rq[2] - value);
                break;
            case Q3:
                rq = body->GetQuaternion();
                m_errorScore = (rq[3] - value);
                break;
            case XP:
                r = body->GetPosition();
                m_errorScore = (r[0] - value);
                break;
            case YP:
                r = body->GetPosition();
                m_errorScore = (r[1] - value);
                break;
            case ZP:
                r = body->GetPosition();
                m_errorScore = (r[2] - value);
                break;
            case XV:
                r = body->GetLinearVelocity();
                m_errorScore = (r[0] - value);
                break;
            case YV:
                r = body->GetLinearVelocity();
                m_errorScore = (r[1] - value);
                break;
            case ZV:
                r = body->GetLinearVelocity();
                m_errorScore = (r[2] - value);
                break;
            case XRV:
                r = body->GetAngularVelocity();
                m_errorScore = (r[0] - value);
                break;
            case YRV:
                r = body->GetAngularVelocity();
                m_errorScore = (r[1] - value);
                break;
            case ZRV:
                r = body->GetAngularVelocity();
                m_errorScore = (r[2] - value);
                break;
            default:
                std::cerr << "DataTargetScalar::GetMatchValue error in " << name() << " unknown DataType " << m_dataType << "\n";
            }
            break;
        }
        if (Marker *marker = dynamic_cast<Marker *>(GetTarget()))
        {
            switch (m_dataType)
            {
            case Q0:
                pq = marker->GetWorldQuaternion();
                m_errorScore = (pq.n - value);
                break;
            case Q1:
                pq = marker->GetWorldQuaternion();
                m_errorScore = (pq.x - value);
                break;
            case Q2:
                pq = marker->GetWorldQuaternion();
                m_errorScore = (pq.y - value);
                break;
            case Q3:
                pq = marker->GetWorldQuaternion();
                m_errorScore = (pq.z - value);
                break;
            case XP:
                pv = marker->GetWorldPosition();
                m_errorScore = (pv.x - value);
                break;
            case YP:
                pv = marker->GetWorldPosition();
                m_errorScore = (pv.y - value);
                break;
            case ZP:
                pv = marker->GetWorldPosition();
                m_errorScore = (pv.z - value);
                break;
            case XV:
                pv = marker->GetWorldLinearVelocity();
                m_errorScore = (pv.x - value);
                break;
            case YV:
                pv = marker->GetWorldLinearVelocity();
                m_errorScore = (pv.y - value);
                break;
            case ZV:
                pv = marker->GetWorldLinearVelocity();
                m_errorScore = (pv.z - value);
                break;
            default:
                std::cerr << "DataTargetScalar::GetMatchValue error in " << name() << " unknown DataType " << m_dataType << "\n";
            }
            break;
        }
        if (HingeJoint *hingeJoint = dynamic_cast<HingeJoint *>(GetTarget()))
        {
            result = hingeJoint->anchor();
            switch (m_dataType)
            {
            case XP:
                m_errorScore = (result[0] - value);
                break;
            case YP:
                m_errorScore = (result[1] - value);
                break;
            case ZP:
                m_errorScore = (result[2] - value);
                break;
            case Angle:
                m_errorScore = (hingeJoint->angle() - value);
                break;
#ifdef FIX_ME
            case XF:
                force = hingeJoint->GetFeedback()->f1[0];
                m_errorScore = (force - value);
                break;
            case YF:
                force = hingeJoint->GetFeedback()->f1[2];
                m_errorScore = (force - value);
                break;
            case ZF:
                force = hingeJoint->GetFeedback()->f1[3];
                m_errorScore = (force - value);
                break;
            case Force:
                pv.Set(hingeJoint->GetFeedback()->f1);
                force = pv.Magnitude();
                m_errorScore = (force - value);
                break;
#endif
            default:
                std::cerr << "DataTargetScalar::GetMatchValue error in " << name() << " unknown DataType " << m_dataType << "\n";
            }
            break;
        }
        if (BallJoint *ballJoint = dynamic_cast<BallJoint *>(GetTarget()))
        {
            result = ballJoint->anchor();
            switch (m_dataType)
            {
            case XP:
                m_errorScore = (result[0] - value);
                break;
            case YP:
                m_errorScore = (result[1] - value);
                break;
            case ZP:
                m_errorScore = (result[2] - value);
                break;
#ifdef FIX_ME
            case XF:
                force = ballJoint->GetFeedback()->f1[0];
                m_errorScore = (force - value);
                break;
            case YF:
                force = ballJoint->GetFeedback()->f1[2];
                m_errorScore = (force - value);
                break;
            case ZF:
                force = ballJoint->GetFeedback()->f1[3];
                m_errorScore = (force - value);
                break;
            case Force:
                pv.Set(ballJoint->GetFeedback()->f1);
                force = pv.Magnitude();
                m_errorScore = (force - value);
                break;
#endif
            default:
                std::cerr << "DataTargetScalar::GetMatchValue error in " << name() << " unknown DataType " << m_dataType << "\n";
            }
            break;
        }
        if (UniversalJoint *universalJoint = dynamic_cast<UniversalJoint *>(GetTarget()))
        {
            result = universalJoint->anchor();
            switch (m_dataType)
            {
            case XP:
                m_errorScore = (result[0] - value);
                break;
            case YP:
                m_errorScore = (result[1] - value);
                break;
            case ZP:
                m_errorScore = (result[2] - value);
                break;
#ifdef FIX_ME
            case XF:
                force = universalJoint->GetFeedback()->f1[0];
                m_errorScore = (force - value);
                break;
            case YF:
                force = universalJoint->GetFeedback()->f1[2];
                m_errorScore = (force - value);
                break;
            case ZF:
                force = universalJoint->GetFeedback()->f1[3];
                m_errorScore = (force - value);
                break;
            case Force:
                pv.Set(universalJoint->GetFeedback()->f1);
                force = pv.Magnitude();
                m_errorScore = (force - value);
                break;
#endif
            default:
                std::cerr << "DataTargetScalar::GetMatchValue error in " << name() << " unknown DataType " << m_dataType << "\n";
            }
            break;
        }
        if (Geom *geom = dynamic_cast<Geom *>(GetTarget()))
        {
            switch (m_dataType)
            {
            case Q0:
                q = geom->GetWorldQuaternion();
                m_errorScore = (q[0] - value);
                break;
            case Q1:
                q = geom->GetWorldQuaternion();
                m_errorScore = (q[1] - value);
                break;
            case Q2:
                q = geom->GetWorldQuaternion();
                m_errorScore = (q[2] - value);
                break;
            case Q3:
                q = geom->GetWorldQuaternion();
                m_errorScore = (q[3] - value);
                break;
            case XP:
                result = geom->GetWorldPosition();
                m_errorScore = (result[0] - value);
                break;
            case YP:
                result = geom->GetWorldPosition();
                m_errorScore = (result[1] - value);
                break;
            case ZP:
                result = geom->GetWorldPosition();
                m_errorScore = (result[2] - value);
                break;
#ifdef FIX_ME
            case XF:
                contactList = geom->GetContactList();
                force = 0;
                for (auto &&it : *contactList) force += it->GetJointFeedback()->f1[0];
                m_errorScore = (force - value);
                break;
            case YF:
                contactList = geom->GetContactList();
                force = 0;
                for (auto &&it : *contactList) force += it->GetJointFeedback()->f1[1];
                m_errorScore = (force - value);
                break;
            case ZF:
                contactList = geom->GetContactList();
                force = 0;
                for (auto &&it : *contactList) force += it->GetJointFeedback()->f1[2];
                m_errorScore = (force - value);
                break;
            case Force:
                contactList = geom->GetContactList();
                force = 0;
                for (auto &&it : *contactList) { pv.Set(it->GetJointFeedback()->f1); force += pv.Magnitude(); }
                m_errorScore = (force - value);
                break;
#endif
            default:
                std::cerr << "DataTargetScalar::GetMatchValue error in " << name() << " unknown DataType " << m_dataType << "\n";
            }
            break;
        }
        if (TegotaeDriver *tegotaeDriver = dynamic_cast<TegotaeDriver *>(GetTarget()))
        {
            pgd::Vector3 errorVector;
            switch (m_dataType)
            {
            case DriverError:
                errorVector = tegotaeDriver->localErrorVector();
                m_errorScore = errorVector.Magnitude() - value;
                break;
            default:
                std::cerr << "DataTargetScalar::GetMatchValue error in " << name() << " unknown DataType " << m_dataType << "\n";
            }
            break;
        }
        if (Driver *driver = dynamic_cast<Driver *>(GetTarget()))
        {
            switch (m_dataType)
            {
            case DriverValue:
                m_errorScore = driver->value() - value;
                break;
            default:
                std::cerr << "DataTargetScalar::GetMatchValue error in " << name() << " unknown DataType " << m_dataType << "\n";
            }
            break;
        }
        if (GetTarget() == nullptr)
        {
            switch(m_dataType)
            {
            case MetabolicEnergy:
                m_errorScore = (simulation()->GetMetabolicEnergy() - value);
                break;
            case MechanicalEnergy:
                m_errorScore = (simulation()->GetMechanicalEnergy() - value);
                break;
            case Time:
                m_errorScore = (simulation()->GetTime() - value);
                break;
            case DeltaTime:
                m_errorScore = (simulation()->GetTimeIncrement() - value);
                break;
            default:
                std::cerr << "DataTargetScalar::GetMatchValue error in " << name() << " unknown DataType " << m_dataType << "\n";
            }
            break;
        }
        std::cerr << "DataTargetScalar::GetMatchValue error in " << name() << " unknown DataTarget " << m_dataType << "\n";
        break;
    }

    switch(std::fpclassify(m_errorScore))
    {
    case FP_NORMAL: break;
    case FP_ZERO: break;
    case FP_INFINITE:
        std::cerr << "Warning: m_errorScore is FP_INFINITE\n";
        m_errorScore = 0;
        break;
    case FP_NAN:
        std::cerr << "Warning: m_errorScore is FP_NAN\n";
        m_errorScore = 0;
        break;
    case FP_SUBNORMAL:
        std::cerr << "Warning: m_errorScore is FP_SUBNORMAL\n";
        m_errorScore = 0;
        break;
    default:
        std::cerr << "Warning: m_errorScore is unclassified\n";
        m_errorScore = 0;
        break;
    }

    return m_errorScore;
}



// returns the difference between the target actual value and the desired value (actual - desired)
double DataTargetScalar::calculateError(size_t index, size_t indexNext, double time)
{
    if (index >= m_valueList.size()) { std::cerr << "Warning: DataTargetVector::calculateError index out of range\n"; return 0; }
    if (indexNext >= m_valueList.size()) { std::cerr << "Warning: DataTargetVector::calculateError index out of range\n"; return 0; }

    double value = GSUtil::Interpolate((*targetTimeList())[size_t(index)], m_valueList[size_t(index)], (*targetTimeList())[indexNext], m_valueList[indexNext], time);
    return calculateErrorScore(value);
}

/**
 * @brief DataTargetScalar::createFromAttributes
 * @return nullptr on success and a pointer to lastError() on failure
 *
 * This function initialises the data in the object based on the contents
 * of an xml_node node. It uses information from the simulation as required
 * to satisfy dependencies
 *
 * Attributes in addition to standard DATATARGET:
 *
 * - Type="Scalar"
 * - DataType="_data type identifier_"
 *   - XP, YP, ZP - position in world coordinates
 *   - Q0, Q1, Q2, Q3 - quaternion in world coodinates
 *   - XV, YV, ZV - linear velocities
 *   - XRV, YRV, ZRV - angular velocities
 *   - Angle - hinge joint angle
 *   - MetabolicEnergy - metabolic energy use
 *   - MechanicalEnergy - mechanical energy
 *   - DriverError - driver error term
 *   - Time - simulation time
 *   - DeltaTime - simulation change of time
 *   - XF, YF, ZF - individual force components
 *   - Force - magnitude of force
 *   - DriverValue - value of driver
 * - TargetID="_ID name_"
 *   - Where _ID name_ is the ID of the target which can be a body, driver, geom, joint, marker or empty depending on the DataType
 *   - Not all data types make sense for all targets and will generate a warning when the simulation is running
 * - TargetValues="_list of numbers_"
 *   - List of numeric values that are used to calculate the error term. The number of values must match the number of TargetTimes.
 */

std::string *DataTargetScalar::createFromAttributes()
{
    if (DataTarget::createFromAttributes()) return lastErrorPtr();

    std::string buf;
    if (findAttribute("DataType"s, &buf) == nullptr) return lastErrorPtr();
    size_t dataTypeIndex;
    for (dataTypeIndex = 0; dataTypeIndex < dataTypeCount; dataTypeIndex++)
    {
        if (buf == dataTypeStrings(dataTypeIndex))
        {
            m_dataType = static_cast<DataType>(dataTypeIndex);
            break;
        }
    }
    if (dataTypeIndex >= dataTypeCount)
    {
        setLastError("DataTargetScalar ID=\""s + name() +"\" Unrecognised MatchType "s + buf);
        return lastErrorPtr();
    }
    m_target = nullptr;
    if (m_noTargetList.count(m_dataType) == 0)
    {
        if (findAttribute("TargetID"s, &buf) == nullptr) return lastErrorPtr();
        for (bool once = true; once; once = false)
        {
            auto iterBody = simulation()->GetBodyList()->find(buf);
            if (iterBody != simulation()->GetBodyList()->end()) { m_target = iterBody->second.get(); break; }
            auto iterJoint = simulation()->GetJointList()->find(buf);
            if (iterJoint != simulation()->GetJointList()->end()) { m_target = iterJoint->second.get(); break; }
            auto iterGeom = simulation()->GetGeomList()->find(buf);
            if (iterGeom != simulation()->GetGeomList()->end()) { m_target = iterGeom->second.get(); break; }
            auto iterMarker = simulation()->GetMarkerList()->find(buf);
            if (iterMarker != simulation()->GetMarkerList()->end()) { m_target = iterMarker->second.get(); break; }
            auto iterDriver = simulation()->GetDriverList()->find(buf);
            if (iterDriver != simulation()->GetDriverList()->end()) { m_target = iterDriver->second.get(); break; }
        }
        if (!m_target)
        {
            setLastError("DataTargetScalar ID=\""s + name() +"\" TargetID not found "s + buf);
            return lastErrorPtr();
        }
    }

    if (findAttribute("TargetValues"s, &buf) == nullptr) return lastErrorPtr();
    std::vector<std::string> targetValuesTokens;
    pystring::split(buf, targetValuesTokens);
    if (targetValuesTokens.size() == 0)
    {
        setLastError("DataTarget ID=\""s + name() +"\" No values found in TargetValues"s);
        return lastErrorPtr();
    }
    if (targetValuesTokens.size() != targetTimeList()->size())
    {
        setLastError("DataTargetScalar ID=\""s + name() +"\" Number of values in TargetValues does not match TargetTimes"s);
        return lastErrorPtr();
    }
    m_valueList.clear();
    m_valueList.reserve(targetValuesTokens.size());
    for (auto &&token : targetValuesTokens) m_valueList.push_back(GSUtil::Double(token));

    if (m_target) setUpstreamObjects({m_target});
    return nullptr;
}

// this function appends data to a pre-existing xml_node - often created by XMLSave
void DataTargetScalar::appendToAttributes()
{
    DataTarget::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "Scalar"s);
    setAttribute("TargetValues"s, *GSUtil::ToString(m_valueList.data(), m_valueList.size(), &buf));
    setAttribute("DataType", dataTypeStrings(m_dataType));
    if (m_noTargetList.count(m_dataType) == 0) setAttribute("TargetID"s, m_target->name());
}

void DataTargetScalar::SetTarget(NamedObject *target)
{
    m_target = target;
}

NamedObject *DataTargetScalar::GetTarget()
{
    return m_target;
}



} // namespace GaitSym


