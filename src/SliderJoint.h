/*
 *  SliderJoint.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 25/05/2012.
 *  Copyright 2012 Bill Sellers. All rights reserved.
 *
 */

#ifndef SLIDERJOINT_H
#define SLIDERJOINT_H

#include "Joint.h"

class SliderJoint: public Joint
{
public:
    SliderJoint();


    double sliderDistance() const;
    void setSliderDistance(double newSliderDistance);

    pgd::Vector3 sliderAxis() const;
    void setSliderAxis(const pgd::Vector3 &newSliderAxis);

    pgd::Vector2 jointStops() const;
    void setJointStops(const pgd::Vector2 &newJointStops);

    double stopSpring() const;
    void setStopSpring(double newStopSpring);

    double stopDamp() const;
    void setStopDamp(double newStopDamp);

    double stopBounce() const;
    void setStopBounce(double newStopBounce);

private:

    double m_sliderDistance = 0;
    pgd::Vector3 m_sliderAxis;
    pgd::Vector2 m_jointStops = {-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    double m_stopSpring = 0;
    double m_stopDamp = 0;
    double m_stopBounce = 0;
};

#endif // SLIDERJOINT_H
