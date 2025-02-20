/*
 *  DataTargetMarkerCompare.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on Thu Dec 24 2020.
 *  Copyright (c) 2020 Bill Sellers. All rights reserved.
 *
 */

#ifndef DATATARGETMARKERCOMPARE_H
#define DATATARGETMARKERCOMPARE_H

#include "DataTarget.h"
#include "SmartEnum.h"

namespace GaitSym
{

class Marker;

class DataTargetMarkerCompare : public DataTarget
{
public:
    DataTargetMarkerCompare();

    SMART_ENUM(Comparison, comparisonStrings, comparisonCount, XWP, YWP, ZWP, XAD, YAD, ZAD, Distance, Angle, LinearVelocity, AngularVelocity);

    virtual std::string *createFromAttributes() override;
    virtual void appendToAttributes() override;

    virtual double calculateError(size_t index, size_t indexNext, double time) override;
    virtual double calculateError(size_t index) override;

private:
    Marker *m_marker1 = nullptr;
    Marker *m_marker2 = nullptr;
    Comparison m_marker1Comparison = XWP;
    Comparison m_marker2Comparison = XWP;

    std::vector<double> m_valueList;
    double m_errorScore = 0;
};

}

#endif // DATATARGETMARKERCOMPARE_H
