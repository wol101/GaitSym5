/*
 *  MagicMuscle.h
 *  GaitSym5
 *
 *  Created by Bill Sellers on 03/06/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#ifndef MAGICMUSCLE_H
#define MAGICMUSCLE_H

#include "Muscle.h"

namespace GaitSym {

class MagicMuscle : public Muscle
{
public:
    MagicMuscle();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    virtual std::string dumpToString();

    virtual void SetActivation();
    virtual double GetActivation();
    virtual double GetMetabolicPower();
    virtual double GetElasticEnergy();

private:
    double m_activation = 0;
    double m_forceMultiplier = 2.0;  // GetStrap()->setTension(m_activation * m_forceMultiplier + m_forceOffset)
    double m_forceOffset = -1.0;
};

} // GaitSym

#endif // MAGICMUSCLE_H
