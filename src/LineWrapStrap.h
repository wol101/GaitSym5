/*
 *  LineWrapStrap.h
 *  GaitSym5
 *
 *  Created by Bill Sellers on 19/06/2026.
 *  Copyright 2026 Bill Sellers. All rights reserved.
 *
 */

#ifndef LineWrapStrap_h
#define LineWrapStrap_h

#include "Strap.h"
#include "PGDMath.h"

namespace GaitSym
{

class Marker;

class LineWrapStrap: public Strap
{
public:

    LineWrapStrap();
    virtual ~LineWrapStrap();

    void setOriginMarker(Marker *originMarker);
    void setInsertionMarker(Marker *insertionMarker);

    void setLineMarkers(Marker *lineStartMarker, Marker *lineEndMarker);

    virtual void calculate();

    Marker *originMarker() const;
    Marker *insertionMarker() const;
    Marker *lineStartMarker() const;
    Marker *lineEndMarker() const;

    const std::vector<pgd::Vector3> *pathCoordinates();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

private:

    static int wrapPath(const pgd::Vector3 &A, const pgd::Vector3 &B, const pgd::Vector3 &P, const pgd::Vector3 &Q,
                        pgd::Vector3 *W, double *total_length, double *t);

    std::vector<pgd::Vector3> m_pathCoordinates;

    Marker *m_originMarker = nullptr;
    Marker *m_insertionMarker = nullptr;
    Marker *m_lineStartMarker = nullptr;
    Marker *m_lineEndMarker = nullptr;

    int m_wrapStatus = 0;
};

}

#endif

