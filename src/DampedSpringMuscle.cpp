/*
 *  DampedSpringMuscle.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 24/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

// DampedSpringMuscle - implementation of a damped spring strap force

#include "Strap.h"
#include "DampedSpringMuscle.h"
#include "Simulation.h"
#include "GSUtil.h"

#include <sstream>

using namespace std::string_literals;
namespace GaitSym {

// constructor

DampedSpringMuscle::DampedSpringMuscle(): Muscle()
{
}

double DampedSpringMuscle::elasticEnergy()
{
    double delLen = strap()->length() - m_unloadedLength;
    if (delLen < 0) return 0;

    // difference between these two values is the amount of energy lost by damping
    // std::cerr << 0.5 * GetStrap()->GetTension() * delLen << "\n";
    // std::cerr << 0.5 * m_SpringConstant * m_Area * delLen * delLen / m_UnloadedLength << "\n";

    return 0.5 * m_springConstant * m_area * delLen * delLen / m_unloadedLength;
}


// update the tension depending on length and velocity
// activation is used as a linear multiplier
void DampedSpringMuscle::updateActivation()
{
    m_activation = dataSum();

    // calculate strain
    double elasticStrain = (strap()->length() - m_unloadedLength) / m_unloadedLength;

    // calculate stress
    double elasticStress = elasticStrain * m_springConstant;
    double tension;
    if (elasticStress <= 0) // if not stretching the spring then set tension to zero
    {
        tension = 0;
    }
    else
    {

        // calculate damping (+ve when lengthening)
        double relativeVelocity = strap()->velocity() / m_unloadedLength;
        double dampingStress = relativeVelocity * m_damping;

        // now calculate tension
        // NB. tension is negative when muscle shortening
        tension = (elasticStress + dampingStress) * m_area * m_activation;

        // stop any pushing
        if (tension < 0) tension = 0;
    }
    strap()->setTension(tension);
}

bool DampedSpringMuscle::shouldBreak()
{
    if (m_breakingStrain <= 0) return false;
    double elasticStrain = (strap()->length() - m_unloadedLength) / m_unloadedLength;
    if (elasticStrain > m_breakingStrain)
    {
        std::cerr << "DampedSpringMuscle::ShouldBreak returns true\n";
        return true;
    }
    return false;
}

std::string DampedSpringMuscle::dumpToString()
{
    std::stringstream ss;
    ss.precision(17);
    ss.setf(std::ios::scientific);
    if (firstDump())
    {
        setFirstDump(false);
        ss << "Time\tact\ttension\tlength\tvelocity\tPMECH\n";
    }
    ss << simulation()->simulationTime() << "\t" << m_activation <<
          "\t" << strap()->tension() << "\t" << strap()->length() << "\t" << strap()->velocity() <<
          "\t" << strap()->velocity() * strap()->tension() <<
          "\n";
    return ss.str();
}

std::string *DampedSpringMuscle::createFromAttributes()
{
    if (Muscle::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("UnloadedLength"s, &buf) == nullptr) return lastErrorPtr();
    this->setUnloadedLength(GSUtil::toDouble(buf.c_str()));
    if (findAttribute("SpringConstant"s, &buf) == nullptr) return lastErrorPtr();
    this->setSpringConstant(GSUtil::toDouble(buf.c_str()));
    if (findAttribute("Area"s, &buf) == nullptr) return lastErrorPtr();
    this->setArea(GSUtil::toDouble(buf.c_str()));
    if (findAttribute("DampingConstant"s, &buf) == nullptr) return lastErrorPtr();
    this->setDamping(GSUtil::toDouble(buf.c_str()));
    if (findAttribute("BreakingStrain"s, &buf) == nullptr) return lastErrorPtr();
    this->setBreakingStrain(GSUtil::toDouble(buf.c_str()));
    return nullptr;
}

 void DampedSpringMuscle::appendToAttributes()
{
     Muscle::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "DampedSpring"s);
    setAttribute("UnloadedLength"s, *GSUtil::toString(m_unloadedLength, &buf));
    setAttribute("SpringConstant"s, *GSUtil::toString(m_springConstant, &buf));
    setAttribute("Area"s, *GSUtil::toString(m_area, &buf));
    setAttribute("DampingConstant"s, *GSUtil::toString(m_damping, &buf));
    setAttribute("BreakingStrain"s, *GSUtil::toString(m_breakingStrain, &buf));
}

} // namespace GaitSym


