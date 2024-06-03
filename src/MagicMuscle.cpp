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
    return nullptr;
}

void MagicMuscle::appendToAttributes()
{
    Muscle::appendToAttributes();
    setAttribute("Type"s, "Magic"s);
    setAttribute("ForcePerUnitArea"s, GSUtil::ToString(m_forceMultiplier));
}

} // GaitSym
