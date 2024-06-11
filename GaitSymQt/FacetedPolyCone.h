/*
 *  FacetedPolyCone.h
 *  GaitSym5
 *
 *  Created by Bill Sellers on 10/06/2024
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#ifndef FACETEDPOLYCONE_H
#define FACETEDPOLYCONE_H

#include "FacetedObject.h"

#include "PGDMath.h"

#include <vector>
#include <array>

class FacetedPolyCone : public FacetedObject
{
public:
    FacetedPolyCone(const std::vector<pgd::Vector3> &vertexList, const std::vector<std::array<float, 3>> &vertexColours, const std::vector<double> &radiusList, size_t nSides, const QColor &blendColour, double blendFraction);
};

#endif // FACETEDPOLYCONE_H
