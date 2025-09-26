/*
 *  CylinderWrapStrap.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 29/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef CylinderWrapStrap_h
#define CylinderWrapStrap_h

#include "Strap.h"
#include "PGDMath.h"

namespace GaitSym
{

class Marker;

class CylinderWrapStrap: public Strap
{
public:

    CylinderWrapStrap();
    virtual ~CylinderWrapStrap();

    void setOriginMarker(Marker *originMarker);
    void setInsertionMarker(Marker *insertionMarker);

    void setCylinderRadius(double cylinderRadius);
    void setCylinderMarker(Marker *cylinderMarker);
    void setNumWrapSegments(int numWrapSegments);

    virtual void calculate();

    Marker *originMarker() const;
    Marker *insertionMarker() const;
    Marker *cylinderMarker() const;

    const std::vector<pgd::Vector3> *pathCoordinates();
    int numWrapSegments();

//    virtual int sanityCheck(Strap *otherStrap, Simulation::AxisType axis, const std::string &sanityCheckLeft, const std::string &sanityCheckRight);

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    double cylinderRadius() const;

private:

    int cylinderWrap(pgd::Vector3 &origin, pgd::Vector3 &insertion, double radius, int nWrapSegments, double maxAngle,
                     pgd::Vector3 &originForce, pgd::Vector3 &insertionForce, pgd::Vector3 &cylinderForce, pgd::Vector3 &cylinderForcePosition,
                     double *pathLength, std::vector<pgd::Vector3> *pathCoordinates);

    double m_cylinderRadius = 1;
    int m_numWrapSegments = 0;

    int m_wrapStatus = -1;

    std::vector<pgd::Vector3> m_pathCoordinates;

    Marker *m_originMarker = nullptr;
    Marker *m_insertionMarker = nullptr;
    Marker *m_cylinderMarker = nullptr;

};

}

#endif

