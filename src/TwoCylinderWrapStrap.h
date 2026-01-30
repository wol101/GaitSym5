/*
 *  TwoCylinderWrapStrap.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 19/12/2012.
 *  Copyright 2012 Bill Sellers. All rights reserved.
 *
 */

#ifndef TwoCylinderWrapStrap_h
#define TwoCylinderWrapStrap_h

#include "Strap.h"
#include "PGDMath.h"

namespace GaitSym
{

class TwoCylinderWrapStrap: public Strap
{
public:

    TwoCylinderWrapStrap();
    virtual ~TwoCylinderWrapStrap();

    void setOrigin(Marker *originMarker);
    void setInsertion(Marker *insertionMarker);

    void setCylinder1Radius(double radius);
    void setCylinder1Marker(Marker *cylinder1Marker);
    void setCylinder2Radius(double radius);
    void setCylinder2Marker(Marker *cylinder2Marker);
    void setNumWrapSegments(int numWrapSegments);

    virtual void calculate();

    Marker *originMarker() const;
    Marker *insertionMarker() const;
    Marker *cylinder1Marker() const;
    Marker *cylinder2Marker() const;

    const std::vector<pgd::Vector3> *pathCoordinates();
    int numWrapSegments();

//    virtual int sanityCheck(Strap *otherStrap, Simulation::AxisType axis, const std::string &sanityCheckLeft, const std::string &sanityCheckRight);

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    double cylinder1Radius() const;
    double cylinder2Radius() const;

private:

    void twoCylinderWrap(pgd::Vector3 &origin, pgd::Vector3 &insertion, pgd::Vector3 &cylinderPosition1, double radius1,
                         pgd::Vector3 &cylinderPosition2, double radius2, double tension, int nPointsPerCylinderArc, double maxAngle,
                         pgd::Vector3 &originForce, pgd::Vector3 &insertionForce, pgd::Vector3 &cylinderForce1, pgd::Vector3 &cylinderForcePosition1,
                         pgd::Vector3 &cylinderForce2, pgd::Vector3 &cylinderForcePosition2, double *pathLength,
                         std::vector<pgd::Vector3> *pathCoordinates, int *wrapOK);
    void findCircleCircleTangents(pgd::Vector3 &c1, double radius1, pgd::Vector3 &c2, double radius2,
                                  pgd::Vector3 &outer1_p1, pgd::Vector3 &outer1_p2, pgd::Vector3 &outer2_p1, pgd::Vector3 &outer2_p2,
                                  pgd::Vector3 &inner1_p1, pgd::Vector3 &inner1_p2, pgd::Vector3 &inner2_p1, pgd::Vector3 &inner2_p2, int *number_of_tangents);
    void findTangents(pgd::Vector3 &center, double radius, pgd::Vector3 &external_point, pgd::Vector3 &pt1, pgd::Vector3 &pt2, int *number_of_tangents);
    void findCircleCircleIntersections(double cx0, double cy0, double radius0, double cx1, double cy1, double radius1,
                                       pgd::Vector3 &intersection1, pgd::Vector3 &intersection2, int *number_of_intersections);

    double vectorDistance2D(pgd::Vector3 &v1, pgd::Vector3 v2);
    double vectorDistance3D(pgd::Vector3 &v1, pgd::Vector3 &v2);
    void vectorWithMagnitude(pgd::Vector3 &v1, pgd::Vector3 &v2, double magnitude, pgd::Vector3 &v);

    double m_cylinder1Radius = 1;
    double m_cylinder2Radius = 1;
    int m_numWrapSegments = 0;

    int m_wrapStatus = -1;

    std::vector<pgd::Vector3> m_pathCoordinates;

    Marker *m_originMarker = nullptr;
    Marker *m_insertionMarker = nullptr;
    Marker *m_cylinder1Marker = nullptr;
    Marker *m_cylinder2Marker = nullptr;
};

}

#endif

