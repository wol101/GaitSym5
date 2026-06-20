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

    if (m_unloadedLength > 0) // use the Youngs Modulus formulation
    {
        return 0.5 * m_springConstant * m_area * delLen * delLen / m_unloadedLength;
    }
    else // use the raw spring constant
    {
        return 0.5 * m_springConstant * delLen * delLen;
    }
}


// update the tension depending on length and velocit
// activation is used as a linear multiplier
void DampedSpringMuscle::updateActivation()
{
    if (m_overideActivation) { m_activation = 1.0; }
    else
    { m_activation = dataSum(); }

    double tension;
    if (m_unloadedLength > 0) // use the Youngs Modulus formulation
    {
        // calculate strain
        double elasticStrain = (strap()->length() - m_unloadedLength) / m_unloadedLength;

        // calculate stress
        double elasticStress = elasticStrain * m_springConstant;
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
    }
    else // use the raw spring constant
    {
        // calculate extension
        double extension = strap()->length() - m_unloadedLength;

        // calculate force
        double elasticForce = extension * m_springConstant;
        if (elasticForce <= 0) // if not stretching the spring then set tension to zero
        {
            tension = 0;
        }
        else
        {

            // calculate damping (+ve when lengthening)
            double dampingForce = strap()->velocity() * m_damping;

            // now calculate tension
            // NB. tension is negative when muscle shortening
            tension = (elasticForce + dampingForce) * m_activation;

            // stop any pushing
            if (tension < 0) tension = 0;
        }
    }
    strap()->setTension(tension);
}

bool DampedSpringMuscle::shouldBreak()
{
    if (m_breakingStrain <= 0 || m_unloadedLength <= 0) return false; // breaking only implmented for Young's Modulus based springs
    double elasticStrain = (strap()->length() - m_unloadedLength) / m_unloadedLength;
    if (elasticStrain > m_breakingStrain)
    {
        // std::cerr << "DampedSpringMuscle::ShouldBreak returns true\n";
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
    // note the formulation can use stress/strain or Force/distance depending on whether the unloaded length is zero (you cannot calculate strains for an unloaded length of zero)
    // spring_constant [N/m] = (Youngs_modulus [Pa] * area [m2]) / length [m]
    // damping_constant [N/ms] = stress_per_strain_rate [Pa/s] * area [m2] / length [m]
    if (Muscle::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("UnloadedLength"s, &buf) == nullptr) return lastErrorPtr(); // If the unloaded length is zero, use the spring constant rather than the Young's Modulus formulation
    this->setUnloadedLength(GSUtil::toDouble(buf.c_str()));
    if (findAttribute("SpringConstant"s, &buf) == nullptr) return lastErrorPtr(); // If the unloaded length is zero, this value is in N/m otherwise it is the Young's Modulus in Pa
    this->setSpringConstant(GSUtil::toDouble(buf.c_str())); // If the unloaded length is zero, this value is in N/ms otherwise it is the stress for unit rate of strain in Pa/s
    if (findAttribute("Area"s, &buf) == nullptr) return lastErrorPtr(); // If the unloaded length is zero, this value is only used for visualisation
    this->setArea(GSUtil::toDouble(buf.c_str()));
    if (findAttribute("DampingConstant"s, &buf) == nullptr) return lastErrorPtr();
    this->setDamping(GSUtil::toDouble(buf.c_str()));
    if (findAttribute("BreakingStrain"s, &buf) == nullptr) return lastErrorPtr(); // If the unloaded length is zero, this value is not used
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

bool DampedSpringMuscle::overideActivation() const
{
    return m_overideActivation;
}

void DampedSpringMuscle::setOverideActivation(bool newOverideActivation)
{
    m_overideActivation = newOverideActivation;
}

} // namespace GaitSym


