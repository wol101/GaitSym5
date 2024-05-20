/*
 *  SliderJoint.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 25/05/2012.
 *  Copyright 2012 Bill Sellers. All rights reserved.
 *
 */

#include "SliderJoint.h"
#include "Simulation.h"

namespace GaitSym {

SliderJoint::SliderJoint() : Joint()
{
}

double SliderJoint::sliderDistance() const
{
    return m_sliderDistance;
}

void SliderJoint::setSliderDistance(double newSliderDistance)
{
    m_sliderDistance = newSliderDistance;
}

double SliderJoint::stopSpring() const
{
    return m_stopSpring;
}

void SliderJoint::setStopSpring(double newStopSpring)
{
    m_stopSpring = newStopSpring;
}

double SliderJoint::stopDamp() const
{
    return m_stopDamp;
}

void SliderJoint::setStopDamp(double newStopDamp)
{
    m_stopDamp = newStopDamp;
}

double SliderJoint::stopBounce() const
{
    return m_stopBounce;
}

void SliderJoint::setStopBounce(double newStopBounce)
{
    m_stopBounce = newStopBounce;
}

pgd::Vector3 SliderJoint::axis() const
{
    return m_axis;
}

void SliderJoint::setAxis(const pgd::Vector3 &newAxis)
{
    m_axis = newAxis;
}

pgd::Vector2 SliderJoint::stops() const
{
    return m_stops;
}

void SliderJoint::setStops(const pgd::Vector2 &newStops)
{
    m_stops = newStops;
}



} // namespace GaitSym


