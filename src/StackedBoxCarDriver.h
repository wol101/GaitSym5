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

#ifndef STACKEDBOXCARDRIVER_H
#define STACKEDBOXCARDRIVER_H

#include <vector>
#include "Driver.h"

namespace GaitSym
{

class StackedBoxcarDriver : public Driver
{
public:
    StackedBoxcarDriver();
    virtual ~StackedBoxcarDriver();

    void setStackSize(size_t stackSize);
    void setCycleTime(double cycleTime);
    void setDelays(double *delays);
    void setWidths(double *widths);
    void setHeights(double *heights);

    double cycleTime() { return m_cycleTime; }

    virtual void update();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

private:
    double m_cycleTime;
    std::vector<double> m_delays;
    std::vector<double> m_widths;
    std::vector<double> m_heights;
    size_t m_stackSize = 0;
};

}

#endif // STACKEDBOXCARDRIVER_H

