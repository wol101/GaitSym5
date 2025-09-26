/*
 *  MAMuscle.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 24/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

// MAMuscle - implementation of an Minetti & Alexander style
// muscle based on the StrapForce class

// Minetti & Alexander, J. theor Biol (1997) 186, 467-476

// Added extra terms to allow a parallel spring element

#include "Strap.h"
#include "MAMuscle.h"
#include "Simulation.h"
#include "GSUtil.h"
#include "TwoPointStrap.h"
#include "NPointStrap.h"
#include "CylinderWrapStrap.h"
#include "TwoCylinderWrapStrap.h"

#include <sstream>

using namespace std::string_literals;
namespace GaitSym {

// constructor

MAMuscle::MAMuscle(): Muscle()
{
}

// destructor
MAMuscle::~MAMuscle()
{
}

void MAMuscle::setVMax(double vMax)
{
    m_vMax = vMax;
}

void MAMuscle::setF0(double f0)
{
    m_f0 = f0;
}

void MAMuscle::setK(double k)
{
    m_k = k;
}

void MAMuscle::updateActivation()
{
    SetAlpha(dataSum());
}

double MAMuscle::activation()
{
    return m_alpha;
}

double MAMuscle::elasticEnergy()
{
    return 0;
}

// set the proportion of muscle fibres that are active
// calculates the tension in the strap

void MAMuscle::SetAlpha(double alpha)
{
    double fCE;
    double v, fFull;

    if (alpha < 0) m_alpha = 0;
    else
    {
        if (alpha > 1.0) m_alpha = 1.0;
        else m_alpha = alpha;
    }

    // m_Velocity is negative when muscle shortening
    // we need the sign the other way round
    v = -strap()->Velocity();

    // limit v
    if (v > m_vMax) v = m_vMax;
    else if (v < -m_vMax) v = -m_vMax;

    if (v < 0)
    {
        fFull = m_f0 * (1.8 - 0.8 * ((m_vMax + v) / (m_vMax - (7.56 / m_k) * v)));
    }
    else
    {
        fFull = m_f0 * (m_vMax - v) / (m_vMax + (v / m_k));
    }

    // now set the tension as a proportion of fFull
    fCE = m_alpha * fFull;
    strap()->setTension(fCE);
}

void MAMuscle::setFibreLength(double fibreLength)
{
    m_fibreLength = fibreLength;
}

void MAMuscle::setPca(double pca)
{
    m_pca = pca;
}

double MAMuscle::vMaxFactor() const
{
    return m_vMaxFactor;
}

void MAMuscle::setVMaxFactor(double vMaxFactor)
{
    m_vMaxFactor = vMaxFactor;
}

void MAMuscle::setForcePerUnitArea(double forcePerUnitArea)
{
    m_forcePerUnitArea = forcePerUnitArea;
}

double MAMuscle::forcePerUnitArea() const
{
    return m_forcePerUnitArea;
}

double MAMuscle::pca() const
{
    return m_pca;
}

double MAMuscle::fibreLength() const
{
    return m_fibreLength;
}

// calculate the metabolic power of the muscle

double MAMuscle::metabolicPower()
{
    // m_Velocity is negative when muscle shortening
    // we need the sign the other way round
    double relV = -strap()->Velocity() / m_vMax;

    // limit relV
    if (relV > 1) relV = 1;
    else if (relV < -1) relV = -1;

    double relVSquared = relV * relV;
    double relVCubed = relVSquared * relV;

    double sigma = (0.054 + 0.506 * relV + 2.46 * relVSquared) /
        (1 - 1.13 * relV + 12.8 * relVSquared - 1.64 * relVCubed);

    return (m_alpha * m_f0 * m_vMax * sigma);
}

std::string *MAMuscle::createFromAttributes()
{
    if (Muscle::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("ForcePerUnitArea"s, &buf) == nullptr) return lastErrorPtr();
    m_forcePerUnitArea = GSUtil::toDouble(buf);
    if (findAttribute("VMaxFactor"s, &buf) == nullptr) return lastErrorPtr();
    m_vMaxFactor = GSUtil::toDouble(buf);
    if (findAttribute("PCA"s, &buf) == nullptr) return lastErrorPtr();
    m_pca = GSUtil::toDouble(buf);
    this->setF0(m_pca * m_forcePerUnitArea);
    if (findAttribute("FibreLength"s, &buf) == nullptr) return lastErrorPtr();
    m_fibreLength = GSUtil::toDouble(buf);
    this->setVMax(m_fibreLength * m_vMaxFactor);
    if (findAttribute("ActivationK"s, &buf) == nullptr) return lastErrorPtr();
    m_k = GSUtil::toDouble(buf);
    return nullptr;
}

void MAMuscle::appendToAttributes()
{
    Muscle::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "MinettiAlexander"s);
    setAttribute("ForcePerUnitArea"s, *GSUtil::toString(m_forcePerUnitArea, &buf));
    setAttribute("VMaxFactor"s, *GSUtil::toString(m_vMaxFactor, &buf));
    setAttribute("PCA"s, *GSUtil::toString(m_pca, &buf));
    setAttribute("FibreLength"s, *GSUtil::toString(m_fibreLength, &buf));
    setAttribute("ActivationK"s, *GSUtil::toString(m_k, &buf));
}

std::string MAMuscle::dumpToString()
{
    std::stringstream ss;
    ss.precision(17);
    ss.setf(std::ios::scientific);
    if (firstDump())
    {
        setFirstDump(false);
        ss << "Time\tVMax\tF0\tK\tAlpha\tFCE\tLCE\tVCE\tPMECH\tPMET\n";
    }
    ss << simulation()->simulationTime() << "\t" << m_vMax << "\t" << m_f0 << "\t" << m_k << "\t" << m_alpha <<
          "\t" << strap()->Tension() << "\t" << strap()->Length() << "\t" << strap()->Velocity() <<
          "\t" << strap()->Velocity() * strap()->Tension() << "\t" << metabolicPower() <<
          "\n";
    return ss.str();
}



} // namespace GaitSym


