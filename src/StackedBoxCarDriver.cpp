/*
 *  StackedBoxCarDriver.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on Thu Feb 21 2013.
 *  Copyright (c) 2013 Bill Sellers. All rights reserved.
 *
 *  Uses a cyclic boxcar function to return a the value at a given time
 *
 */

#include "StackedBoxCarDriver.h"
#include "GSUtil.h"
#include "Simulation.h"

#include <algorithm>

using namespace std::string_literals;
namespace GaitSym {

StackedBoxcarDriver::StackedBoxcarDriver()
{
}

StackedBoxcarDriver::~StackedBoxcarDriver()
{
}

void StackedBoxcarDriver::setStackSize(size_t stackSize)
{
    m_stackSize = stackSize;
    m_delays.resize(m_stackSize);
    m_widths.resize(m_stackSize);
    m_heights.resize(m_stackSize);
}

// these parameters control the shape of the box car and when it occurs
// CycleTime - this is the period of the boxcar
// Delay     - value from 0 to 1 used to control the phase of the function
// Width     - value from 0 to 1 used to control the width of the function
// Height    - value when the box car is active (otherwise the output is zero)
// Note: Delay and Width values are subtracted from the floor value to guarantee a value from 0 to 1

void StackedBoxcarDriver::setCycleTime(double cycleTime)
{
    m_cycleTime = cycleTime;
}

void StackedBoxcarDriver::setDelays(double *delays)
{
    for (size_t i = 0; i < m_stackSize; i++) m_delays[i] = delays[i] - floor(delays[i]);
}

void StackedBoxcarDriver::setWidths(double *widths)
{
    for (size_t i = 0; i < m_stackSize; i++) m_widths[i] = widths[i] - floor(widths[i]);
}

void StackedBoxcarDriver::setHeights(double *heights)
{
    for (size_t i = 0; i < m_stackSize; i++) m_heights[i] = heights[i];
}


void StackedBoxcarDriver::update()
{
    assert(simulation()->stepCount() == lastStepCount() + 1);
    setLastStepCount(simulation()->stepCount());

    double output = 0;
    double offTime;
    // get a normalised cycle time (value from 0 to 1)
    double time = simulation()->simulationTime();
    double normalisedCycleTime = (time / m_cycleTime) - floor(time / m_cycleTime);

    for (size_t i = 0; i < m_stackSize; i++)
    {
        offTime = m_delays[i] + m_widths[i];
        if (offTime < 1) // no wrap case
        {
            if (normalisedCycleTime > m_delays[i] && normalisedCycleTime < offTime) output += m_heights[i];
        }
        else // wrap case
        {
            if (normalisedCycleTime < offTime - 1 || normalisedCycleTime > m_delays[i]) output += m_heights[i];
        }
    }

    setValue(output);
}

// this function initialises the data in the object based on the contents
// of an xml_node node. It uses information from the simulation as required
// to satisfy dependencies
// it returns nullptr on success and a pointer to lastError() on failure
std::string *StackedBoxcarDriver::createFromAttributes()
{
    if (Driver::createFromAttributes()) return lastErrorPtr();

    std::string buf;
    if (findAttribute("StackSize"s, &buf) == nullptr) return lastErrorPtr();
    this->setStackSize(size_t(GSUtil::toInt(buf)));

    buf.reserve(m_stackSize * 32);
    std::vector<double> doubleList;
    doubleList.reserve(m_stackSize);

    if (findAttribute("CycleTime"s, &buf) == nullptr) return lastErrorPtr();
    this->setCycleTime(GSUtil::toDouble(buf));
    if (findAttribute("Delays"s, &buf) == nullptr) return lastErrorPtr();
    this->setDelays(GSUtil::toDouble(buf, int(m_stackSize), doubleList.data()));
    if (findAttribute("Widths"s, &buf) == nullptr) return lastErrorPtr();
    this->setWidths(GSUtil::toDouble(buf, int(m_stackSize), doubleList.data()));
    if (findAttribute("Heights"s, &buf) == nullptr) return lastErrorPtr();
    this->setHeights(GSUtil::toDouble(buf, int(m_stackSize), doubleList.data()));

    return nullptr;
}

// this function appends data to a pre-existing xml_node - often created by XMLSave
void StackedBoxcarDriver::appendToAttributes()
{
    Driver::appendToAttributes();
    std::string buf;
    buf.reserve(m_stackSize * 32); // should be big enough but it will grow if necessary anyway
    setAttribute("Type"s, "StackedBoxcar"s);
    setAttribute("StackSize", *GSUtil::toString(m_stackSize, &buf));
    setAttribute("CycleTime", *GSUtil::toString(m_cycleTime, &buf));
    setAttribute("Delays", *GSUtil::toString(m_delays.data(), m_stackSize, &buf));
    setAttribute("Widths", *GSUtil::toString(m_widths.data(), m_stackSize, &buf));
    setAttribute("Heights", *GSUtil::toString(m_heights.data(), m_stackSize, &buf));
}


} // namespace GaitSym


