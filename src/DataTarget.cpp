/*
 *  DataTarget.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on Sat May 22 2004.
 *  Copyright (c) 2004 Bill Sellers. All rights reserved.
 *
 */

#include "DataTarget.h"
#include "Simulation.h"
#include "GSUtil.h"

#include "pystring.h"

#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std::string_literals;
namespace GaitSym {

DataTarget::DataTarget()
{
}

std::vector<double> *DataTarget::targetTimeList()
{
    return &m_targetTimeList;
}

double DataTarget::positiveError() const
{
    return m_positiveError;
}

double DataTarget::rawError() const
{
    return m_rawError;
}

size_t DataTarget::index() const
{
    return m_index;
}

double DataTarget::value() const
{
    return m_value;
}

double DataTarget::positiveFunction(double v)
{
    switch (m_matchType)
    {
    case Linear:
        return std::abs(v);

    case Square:
        return v * v;

    case Raw:
        return v;
    }
    return 0;
}

// returns true when matchScore value is valid
bool DataTarget::calculateMatchValue(double time, double *matchScore)
{
    m_index = size_t(0.5 + time / simulation()->GetTimeIncrement());
    switch (m_interpolationType)
    {
    case Punctuated:
        {
            auto iter = std::lower_bound(m_targetTimeIndexList.begin(), m_targetTimeIndexList.end(), m_index);
            // if a searching element exists: std::lower_bound() returns iterator to the element itself
            if (iter != m_targetTimeIndexList.end() && *iter == m_index)
            {
                m_rawError = calculateError(size_t(std::distance(m_targetTimeIndexList.begin(), iter)));
                m_positiveError = positiveFunction(m_rawError);
                m_value = m_intercept + m_slope * m_positiveError;
                break;
            }
            m_value = std::numeric_limits<double>::quiet_NaN();
            m_rawError = std::numeric_limits<double>::quiet_NaN();
            m_positiveError = std::numeric_limits<double>::quiet_NaN();
            return false;
        }
    case Continuous:
        {
            size_t index1, index2;
            auto iter = std::lower_bound(m_targetTimeIndexList.begin(), m_targetTimeIndexList.end(), m_index);
            // lower_bound
            // if a searching element exists: std::lower_bound() returns iterator to the element itself
            // if a searching element doesn't exist:
            //    if all elements are greater than the searching element: lower_bound() returns an iterator to begin of the range
            //    if all elements are lower than the searching element: lower_bound() returns an iterator to end of the range
            //    otherwise, lower_bound() returns an iterator to the next greater element to the search element of the range
            while (true)
            {
                if (iter == m_targetTimeIndexList.begin())
                {
                    index1 = 0;
                    index2 = index1;
                    break;
                }
                if (iter == m_targetTimeIndexList.end())
                {
                    index1 = m_targetTimeList.size() - 1;
                    index2 = index1;
                    break;
                }
                if (*iter == m_index)
                {
                    index1 = size_t(std::distance(m_targetTimeIndexList.begin(), iter));
                    index2 = index1;
                    break;
                }
                // at this point there is no match
                index2 = size_t(std::distance(m_targetTimeIndexList.begin(), iter));
                if (index2 > 0) { index1 = index2 - 1; }
                else { index1 = index2; }
                break;
            }
            m_rawError = calculateError(index1, index2, time);
            m_positiveError = positiveFunction(m_rawError);
            m_value = m_intercept + m_slope * m_positiveError;
            break;
        }
    }
    if (m_value < m_abortBelow || m_value > m_abortAbove)
    {
        simulation()->SetDataTargetAbort(name());
        m_value += m_abortBonus;
    }
    *matchScore = m_value;
    return true;
}

// test for monotonicity
// returns 0 if not monotonic
// returns +1 if increasing monotonically
// returns -1 if decreasing monotonically
// returns +2 if does not vary at all
int DataTarget::monotonicTest(const std::vector<double> &data)
{
    if (data.size() < 2) return +2;
    double del = data[1] - data[0];
    if (del < 0) // test for decreasing monotonically
    {
        for (size_t i = 2; i < data.size(); ++i)
        {
            del = data[i] - data[i - 1];
            if (del >= 0) { return 0; }
        }
        return -1;
    }
    if (del > 0) // test for increasing monotonically
    {
        for (size_t i = 2; i < data.size(); ++i)
        {
            del = data[i] - data[i - 1];
            if (del <= 0) { return 0; }
        }
        return +1;
    }
    // test for unchanging
    for (size_t i = 2; i < data.size(); ++i)
    {
        del = data[i] - data[i - 1];
        if (del != 0) { return 0; }
    }
    return +2;
}

std::string DataTarget::dumpToString()
{
    std::string s;
    if (firstDump())
    {
        setFirstDump(false);
        s += dumpHelper({"time"s, "index"s, "raw_error", "positive_error", "score"s});
    }
    s += dumpHelper({simulation()->GetTime(), double(m_index), m_rawError, m_positiveError, m_value});
    return s;
}

// this function initialises the data in the object based on the contents
// of an xml_node node. It uses information from the simulation as required
// to satisfy dependencies
// it returns nullptr on success and a pointer to lastError() on failure
std::string *DataTarget::createFromAttributes()
{
    if (NamedObject::createFromAttributes()) return lastErrorPtr();

    std::string buf;
    buf.reserve(10000);

    if (findAttribute("Intercept"s, &buf) == nullptr) return lastErrorPtr();
    m_intercept = GSUtil::Double(buf);
    if (findAttribute("Slope"s, &buf) == nullptr) return lastErrorPtr();
    m_slope = GSUtil::Double(buf);

    if (findAttribute("AbortAbove"s, &buf)) m_abortAbove = GSUtil::Double(buf);
    if (findAttribute("AbortBelow"s, &buf)) m_abortBelow = GSUtil::Double(buf);
    if (findAttribute("AbortBonus"s, &buf)) m_abortBonus = GSUtil::Double(buf);

    if (findAttribute("TargetTimes"s, &buf) == nullptr) return lastErrorPtr();
    std::vector<std::string> targetTimesTokens;
    pystring::split(buf, targetTimesTokens);
    if (targetTimesTokens.size() == 0)
    {
        setLastError("DataTarget ID=\""s + name() +"\" No times found in TargetTimes"s);
        return lastErrorPtr();
    }
    m_targetTimeList.clear();
    m_targetTimeList.reserve(targetTimesTokens.size());
    for (auto &&token : targetTimesTokens) m_targetTimeList.push_back(GSUtil::Double(token));
    if (monotonicTest(m_targetTimeList) != 1)
    {
        setLastError("DataTarget ID=\""s + name() +"\" TargetTimes are not in ascending order"s);
        return lastErrorPtr();
    }
    m_targetTimeIndexList.clear();
    m_targetTimeIndexList.reserve(m_targetTimeList.size());
    for (auto &&iter : m_targetTimeList) m_targetTimeIndexList.push_back(size_t(0.5 + iter / simulation()->GetTimeIncrement()));

    if (findAttribute("MatchType"s, &buf) == nullptr) return lastErrorPtr();
    size_t matchTypeIndex;
    for (matchTypeIndex = 0; matchTypeIndex < matchTypeCount; matchTypeIndex++)
    {
        if (buf == matchTypeStrings(matchTypeIndex))
        {
            m_matchType = static_cast<MatchType>(matchTypeIndex);
            break;
        }
    }
    if (matchTypeIndex >= matchTypeCount)
    {
        setLastError("DataTarget ID=\""s + name() +"\" Unrecognised MatchType "s + buf);
        return lastErrorPtr();
    }

    if (findAttribute("InterpolationType"s, &buf) == nullptr) return lastErrorPtr();
    size_t interpolationTypeIndex;
    for (interpolationTypeIndex = 0; interpolationTypeIndex < interpolationTypeCount; interpolationTypeIndex++)
    {
        if (buf == interpolationTypeStrings(interpolationTypeIndex))
        {
            m_interpolationType = static_cast<InterpolationType>(interpolationTypeIndex);
            break;
        }
    }
    if (interpolationTypeIndex >= interpolationTypeCount)
    {
        setLastError("DataTarget ID=\""s + name() +"\" Unrecognised InterpolationType "s + buf);
        return lastErrorPtr();
    }

    return nullptr;
}

// this function copies the data in the object to an xml_node node that it creates internally.
// doc is used to allocate the memory so deletion should be automatic
void DataTarget::saveToAttributes()
{
    this->setTag("DATATARGET"s);
    this->clearAttributeMap();
    this->appendToAttributes();
}

// this function appends data to a pre-existing xml_node - often created by XMLSave
void DataTarget::appendToAttributes()
{
    NamedObject::appendToAttributes();
    std::string buf;
    buf.reserve(size_t(m_targetTimeList.size()) * 32);
    setAttribute("Intercept"s, *GSUtil::ToString(m_intercept, &buf));
    setAttribute("Slope"s, *GSUtil::ToString(m_slope, &buf));
    setAttribute("AbortAbove"s, *GSUtil::ToString(m_abortAbove, &buf));
    setAttribute("AbortBelow"s, *GSUtil::ToString(m_abortBelow, &buf));
    setAttribute("AbortBonus"s, *GSUtil::ToString(m_abortBonus, &buf));
    setAttribute("TargetTimes"s, *GSUtil::ToString(m_targetTimeList.data(), m_targetTimeList.size(), &buf));
    setAttribute("MatchType", matchTypeStrings(m_matchType));
    setAttribute("InterpolationType", interpolationTypeStrings(m_interpolationType));
}


} // namespace GaitSym


