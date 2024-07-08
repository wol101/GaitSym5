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

void MagicMuscle::SetActivation()
{
    m_activation = dataSum();
    GetStrap()->setTension(m_activation * m_forceMultiplier + m_forceOffset);
}

double MagicMuscle::GetActivation()
{
    return m_activation;
}

double MagicMuscle::GetMetabolicPower()
{
    return 0;
}

double MagicMuscle::GetElasticEnergy()
{
    return 0;
}


std::string *MagicMuscle::createFromAttributes()
{
    if (Muscle::createFromAttributes()) return lastErrorPtr();
    std::string buf;
    if (findAttribute("ForceMultiplier"s, &buf) == nullptr) return lastErrorPtr();
    m_forceMultiplier = GSUtil::Double(buf);
    if (findAttribute("ForceOffset"s, &buf) == nullptr) return lastErrorPtr();
    m_forceOffset = GSUtil::Double(buf);
    return nullptr;
}

void MagicMuscle::appendToAttributes()
{
    Muscle::appendToAttributes();
    setAttribute("Type"s, "Magic"s);
    setAttribute("ForceMultiplier"s, GSUtil::ToString(m_forceMultiplier));
    setAttribute("ForceOffset"s, GSUtil::ToString(m_forceMultiplier));
}

std::string MagicMuscle::dumpToString()
{
    std::string s;
    if (firstDump())
    {
        setFirstDump(false);
        s += dumpHelper({"time"s, "activation"s, "forceMultiplier"s, "tension"s});
    }
    s += dumpHelper({simulation()->GetTime(), m_activation, m_forceMultiplier, GetStrap()->Tension()});
    return s;
}


} // GaitSym
