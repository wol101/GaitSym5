/*
 *  DataTarget.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on Sat May 22 2004.
 *  Copyright (c) 2004 Bill Sellers. All rights reserved.
 *
 */

#ifndef DataTarget_h
#define DataTarget_h

#include "NamedObject.h"
#include "SmartEnum.h"

#include <limits>

namespace GaitSym
{

class Body;
class SimulationWindow;


class DataTarget: public NamedObject
{
public:
    DataTarget();

    SMART_ENUM(MatchType, matchTypeStrings, matchTypeCount, Linear, Square, Raw);
    SMART_ENUM(InterpolationType, interpolationTypeStrings, interpolationTypeCount, Punctuated, Continuous);

    bool calculateMatchValue(double time, double *matchScore);
    double positiveFunction(double v);

    virtual std::string dumpToString();
    virtual std::string *createFromAttributes();
    virtual void saveToAttributes();
    virtual void appendToAttributes();

    virtual double calculateError(double time) = 0;
    virtual double calculateError(size_t index) = 0;

    static int monotonicTest(const std::vector<double> &data);

    double value() const;
    size_t index() const;
    double rawError() const;
    double positiveError() const;

protected:
   std::vector<double> *targetTimeList();

private:
    double m_intercept = 0;
    double m_slope = 0;
    MatchType m_matchType = MatchType::Linear;
    InterpolationType m_interpolationType = InterpolationType::Punctuated;
    double m_abortBelow = -std::numeric_limits<double>::infinity();
    double m_abortAbove = std::numeric_limits<double>::infinity();
    std::vector<double> m_targetTimeList;
    std::vector<size_t> m_targetTimeIndexList;
    size_t m_index = std::numeric_limits<size_t>::max();
    double m_value = std::numeric_limits<double>::quiet_NaN();
    double m_rawError = std::numeric_limits<double>::quiet_NaN();
    double m_positiveError = std::numeric_limits<double>::quiet_NaN();
    double m_abortBonus = 0;
};

}

#endif

