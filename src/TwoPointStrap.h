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

//    void SetOrigin(Body *body, const pgd::Vector3 point);
//    void SetInsertion(Body *body, const pgd::Vector3 point);
    void SetOrigin(Marker *originMarker);
    void SetInsertion(Marker *insertionMarker);

//    void GetOrigin(const Body **body, pgd::Vector3 origin) const;
//    void GetInsertion(const Body **body, pgd::Vector3 insertion) const;

    virtual void Calculate();

//    virtual int SanityCheck(Strap *otherStrap, Simulation::AxisType axis, const std::string &sanityCheckLeft, const std::string &sanityCheckRight);

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    Marker *GetOriginMarker() const;
    Marker *GetInsertionMarker() const;

private:

//    Body *m_originBody = nullptr;
//    pgd::Vector3 m_origin;
//    Body *m_insertionBody = nullptr;
//    pgd::Vector3 m_insertion;

    Marker *m_originMarker = nullptr;
    Marker *m_insertionMarker = nullptr;
};

}

#endif
