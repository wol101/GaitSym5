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

pgd::Vector3 SliderJoint::sliderAxis() const
{
    return m_sliderAxis;
}

void SliderJoint::setSliderAxis(const pgd::Vector3 &newSliderAxis)
{
    m_sliderAxis = newSliderAxis;
}

pgd::Vector2 SliderJoint::jointStops() const
{
    return m_jointStops;
}

void SliderJoint::setJointStops(const pgd::Vector2 &newJointStops)
{
    m_jointStops = newJointStops;
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



