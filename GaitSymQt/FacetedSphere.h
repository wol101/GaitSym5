/*
 *  FacetedSphere.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 03/01/2006.
 *  Copyright 2006 Bill Sellers. All rights reserved.
 *
 */

#ifndef FacetedSphere_h
#define FacetedSphere_h

#include <sstream>

#include "FacetedObject.h"

class FacetedSphere: public FacetedObject
{
public:
    FacetedSphere(double radius, size_t maxlevels, const QColor &blendColour, double blendFraction);

    static size_t estimateLevel(size_t requestedFaces, size_t *actualFaces = nullptr);

    virtual void writePOVRay(std::ostringstream &theString);

private:
    size_t m_level;
    double m_radius;
};

#endif
