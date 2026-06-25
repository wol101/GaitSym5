/*
 *  Driver.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on Sat May 22 2004.
 *  Copyright (c) 2004 Bill Sellers. All rights reserved.
 *
 * Virtual class that all drivers descend from
 */

#ifndef Driver_h
#define Driver_h

#include "NamedObject.h"

#include <map>
#include <string>
#include <cstdint>

namespace GaitSym
{

class Drivable;

class Driver : public NamedObject
{
public:
    Driver();
    virtual ~Driver() ;

    int addTarget(Drivable *target);
    Drivable *getTarget(const std::string & name);
    double clamp(double value);

    virtual void update() = 0;
    virtual void sendData();

    virtual std::string dumpToString();
    virtual std::string *createFromAttributes();
    virtual void saveToAttributes();
    virtual void appendToAttributes();

    double minValue() const;
    void setMinValue(double MinValue);

    double maxValue() const;
    void setMaxValue(double MaxValue);

    bool interp() const;
    void setInterp(bool Interp);

    const std::map<std::string, Drivable *> *targetList() const;

    int64_t lastStepCount() const;
    void setLastStepCount(const int64_t &lastStepCount);

    double value() const;
    void setValue(double value);

private:

    std::map<std::string, Drivable *> m_targetList;
    double m_minValue = 0;
    double m_maxValue = 1;
    bool m_interp = false;
    int64_t m_lastStepCount = -1;
    double m_value = 0;
};

}

#endif
