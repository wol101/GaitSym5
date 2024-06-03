/*
 *  MagicStrap.h
 *  GaitSym5
 *
 *  Created by Bill Sellers on 03/06/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#ifndef MAGICSTRAP_H
#define MAGICSTRAP_H

#include "Strap.h"

#include "SmartEnum.h"

namespace GaitSym {

class Marker;

class MagicStrap : public Strap
{
public:
    MagicStrap();

    SMART_ENUM(ForceDirection, forceDirectionStrings, forceDirectionCount, WorldRelative, BodyRelative);

    virtual void Calculate();

    virtual std::string dumpToString();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

private:
    std::vector<Marker *> m_markerList;
    ForceDirection m_forceDirection = BodyRelative;
};

} // GaitSym

#endif // MAGICSTRAP_H
