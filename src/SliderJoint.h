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

namespace GaitSym
{

class SliderJoint: public Joint
{
public:
    SliderJoint();

    double sliderDistance() const;
    void setSliderDistance(double newSliderDistance);

    double stopSpring() const;
    void setStopSpring(double newStopSpring);

    double stopDamp() const;
    void setStopDamp(double newStopDamp);

    double stopBounce() const;
    void setStopBounce(double newStopBounce);

    pgd::Vector3 axis() const;
    void setAxis(const pgd::Vector3 &newAxis);

    pgd::Vector2 stops() const;
    void setStops(const pgd::Vector2 &newStops);

private:

    double m_sliderDistance = 0;
    pgd::Vector3 m_axis;
    pgd::Vector2 m_stops = {-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    double m_stopSpring = 0;
    double m_stopDamp = 0;
    double m_stopBounce = 0;
};

}

#endif // SLIDERJOINT_H
