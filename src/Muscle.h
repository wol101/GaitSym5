/*
 *  Muscle.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 29/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef Muscle_h
#define Muscle_h

#include "Drivable.h"
#include "Strap.h"
#include "Simulation.h"
#include "SmartEnum.h"

#include <string>

namespace GaitSym
{

class Muscle: public Drivable, public NamedObject
{
public:

    Muscle();
    virtual ~Muscle();

    double length() const;
    double velocity() const;
    double tension() const;
    double power() const;

    void calculateStrap();

    SMART_ENUM(StrapColourControl, strapColourControlStrings, strapColourControlCount, fixedColour, activationMap, strainMap, forceMap);
//    enum StrapColourControl { fixedColour, activationMap, strainMap, forceMap };

    virtual void updateActivation() = 0;
    virtual double activation() = 0;
    virtual double metabolicPower() = 0;
    virtual double elasticEnergy() = 0;

    std::vector<std::unique_ptr<PointForce >> *pointForceList() const;

    Strap *strap() const;
    void setStrap(Strap *strap);

//    virtual int sanityCheck(Muscle *otherMuscle, Simulation::AxisType axis, const std::string &sanityCheckLeft, const std::string &sanityCheckRight);
    virtual void lateInitialisation();
    virtual std::string *createFromAttributes();
    virtual void saveToAttributes();
    virtual void appendToAttributes();

    StrapColourControl strapColourControl() const;
    void setStrapColourControl(const Muscle::StrapColourControl &strapColourControl);

private:

    Strap *m_Strap = nullptr;
    enum StrapColourControl m_strapColourControl = fixedColour;
};

}

#endif

