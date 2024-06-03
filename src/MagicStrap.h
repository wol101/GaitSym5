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

namespace GaitSym {

class Marker;

class MagicStrap : public Strap
{
public:
    MagicStrap();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

private:
    std::vector<Marker *> m_markerList;
};

} // GaitSym

#endif // MAGICSTRAP_H
