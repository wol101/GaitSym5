/*
 *  DampedSpringMuscle.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 24/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

// DampedSpringMuscle - implementation of a damped spring strap force

#ifndef DampedSpringMuscle_h
#define DampedSpringMuscle_h

#include "Muscle.h"

namespace GaitSym
{

class Strap;

class DampedSpringMuscle : public Muscle
{
public:

    DampedSpringMuscle();

    void setDamping(double d) { m_damping = d; } // value is in N/(m2 s) (Stress per Strain rate)
    void setSpringConstant(double k) { m_springConstant = k; } // value is in N/m2 (this is the Young's Modulus)
    void setUnloadedLength(double l) { m_unloadedLength = l; } // value is in m
    void setArea(double a) { m_area = a; } // value is in m2
    void setBreakingStrain(double breakingStrain) { m_breakingStrain = breakingStrain; } // value is strain
    double damping() { return m_damping; } // value is in N/m2
    double springConstant() { return m_springConstant; } // value is in N/m2
    double unloadedLength() { return m_unloadedLength; } // value is in m
    double area() { return m_area; } // value is in m2
    double breakingStrain() { return m_breakingStrain; } // value is strain
    double elasticEnergy();

    virtual void updateActivation();
    virtual double activation() { return m_activation; }
    virtual double metabolicPower() { return 0; }

    bool shouldBreak();

    virtual std::string dumpToString();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

private:

    double m_damping = 0;
    double m_springConstant = 0;
    double m_unloadedLength = 0;
    double m_area = 1;
    double m_breakingStrain = 0;
    double m_activation = 0;
};

}

#endif
