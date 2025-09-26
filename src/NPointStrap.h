/*
 *  NPointStrap.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 27/10/2007.
 *  Copyright 2007 Bill Sellers. All rights reserved.
 *
 */

#ifndef NPointStrap_h
#define NPointStrap_h

#include "Strap.h"
#include "PGDMath.h"

namespace GaitSym
{

class Body;
class Marker;

class NPointStrap: public Strap
{
public:

    NPointStrap();

    void setOrigin(Marker *originMarker);
    void setInsertion(Marker *insertionMarker);

    void setViaPoints(std::vector<Marker *> *viaPointMarkerList);

    const std::vector<pgd::Vector3> *viaPoints() const;
    const std::vector<Body *> *viaPointBodies() const;
    const std::vector<Marker *> *viaPointMarkers() const;

    Marker *originMarker() const;
    Marker *insertionMarker() const;


    virtual void calculate();

//    virtual int sanityCheck(Strap *otherStrap, Simulation::AxisType axis, const std::string &sanityCheckLeft, const std::string &sanityCheckRight);

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

private:

//    Body *m_originBody = nullptr;
//    pgd::Vector3 m_origin;
//    Body *m_insertionBody = nullptr;
//    pgd::Vector3 m_insertion;

    Marker *m_originMarker = nullptr;
    Marker *m_insertionMarker = nullptr;

    std::vector<Body *> m_viaBodyList;
    std::vector<pgd::Vector3> m_viaPointList;
    std::vector<Marker *> m_viaPointMarkerList;
};

}

#endif
