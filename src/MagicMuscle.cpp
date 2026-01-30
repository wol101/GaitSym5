/*
 *  MagicMuscle.h
 *  GaitSym5
 *
 *  Created by Bill Sellers on 03/06/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#include "MagicMuscle.h"

#include "GSUtil.h"

namespace GaitSym {

MagicMuscle::MagicMuscle() {}

void MagicMuscle::updateActivation()
{
    m_activation = dataSum();
    strap()->setTension(m_activation * m_forceMultiplier + m_forceOffset);
}

double MagicMuscle::activation()
{
    return m_activation;
}

double MagicMuscle::metabolicPower()
{
    return 0;
}

double MagicMuscle::elasticEnergy()
{
    return 0;
}


std::string *MagicMuscle::createFromAttributes()
{
    if (Muscle::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("ForceMultiplier"s, &buf) == nullptr) return lastErrorPtr();
    m_forceMultiplier = GSUtil::toDouble(buf);
    if (findAttribute("ForceOffset"s, &buf) == nullptr) return lastErrorPtr();
    m_forceOffset = GSUtil::toDouble(buf);
    return nullptr;
}

void MagicMuscle::appendToAttributes()
{
    Muscle::appendToAttributes();
    setAttribute("Type"s, "Magic"s);
    setAttribute("ForceMultiplier"s, GSUtil::toString(m_forceMultiplier));
    setAttribute("ForceOffset"s, GSUtil::toString(m_forceOffset));
}

std::string MagicMuscle::dumpToString()
{
    std::string s;
    if (firstDump())
    {
        setFirstDump(false);
        s += dumpHelper({"time"s, "activation"s, "forceMultiplier"s, "forceOffset"s, "tension"s});
    }
    s += dumpHelper({simulation()->simulationTime(), m_activation, m_forceMultiplier, m_forceOffset, strap()->tension()});
    return s;
}


} // GaitSym
