/*
 *  TwoPointStrap.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 29/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef TwoPointStrap_h
#define TwoPointStrap_h

#include "Strap.h"

namespace GaitSym
{

class Body;
class Marker;

class TwoPointStrap:public Strap
{
public:

    TwoPointStrap();

    void setOrigin(Marker *originMarker);
    void setInsertion(Marker *insertionMarker);

    virtual void calculate();

//    virtual int sanityCheck(Strap *otherStrap, Simulation::AxisType axis, const std::string &sanityCheckLeft, const std::string &sanityCheckRight);

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    Marker *originMarker() const;
    Marker *insertionMarker() const;

private:

    Marker *m_originMarker = nullptr;
    Marker *m_insertionMarker = nullptr;
};

}

#endif
