/*
 *  FacetedPolyCone.h
 *  GaitSym5
 *
 *  Created by Bill Sellers on 10/06/2024
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#include "FacetedPolyCone.h"

#include "gle.h"
#include "GLEmulator.h"

extern GLEmulator glEmulator;

FacetedPolyCone::FacetedPolyCone(const std::vector<pgd::Vector3> &vertexList, const vector<std::array<float, 3>> &vertexColours, const std::vector<double> &radiusList, size_t nFacets, const QColor &blendColour, double blendFraction)
{
    setBlendColour(blendColour, blendFraction);

    glEmulator.clear();
    glEmulator.reserve(n * (polyline->size() * 2 + 2));
    gleSetNumSides(int(n));
    gleSetJoinStyle(TUBE_JN_ROUND | TUBE_JN_CAP | TUBE_NORM_PATH_EDGE | TUBE_CONTOUR_CLOSED); // TUBE_JN_ROUND is pretty but TUBE_JN_ANGLE is probably quicker

    size_t nPoints = polyline->size() + 2;
    auto point_array = std::make_unique<gleDouble[][3]>(nPoints);
    auto color_array = std::make_unique<gleColor[]>(nPoints);
    pgd::Vector3 v0 = (*polyline)[1] - (*polyline)[0];
    pgd::Vector3 v1 = (*polyline)[0] - v0;
    point_array[0][0] = v1.x; point_array[0][1] = v1.y; point_array[0][2] = v1.z;
    for (size_t i = 1 ; i < nPoints - 1; i++) { point_array[i][0] = (*polyline)[i - 1].x; point_array[i][1] = (*polyline)[i - 1].y; point_array[i][2] = (*polyline)[i - 1].z; }
    v0 = (*polyline)[polyline->size() - 1] - (*polyline)[polyline->size() - 2];
    v1 = (*polyline)[polyline->size() - 1] + v0;
    point_array[nPoints - 1][0] = v1.x; point_array[nPoints - 1][1] = v1.y; point_array[nPoints - 1][2] = v1.z;
    float r = blendColour.redF();
    float g = blendColour.greenF();
    float b = blendColour.blueF();
    for (size_t i = 0 ; i < nPoints; i++) { color_array[i][0] = r; color_array[i][1] = g; color_array[i][2] = b; }
    glePolyCylinder(int(nPoints),           /* num points in polyline */
                                  point_array.get(),      /* polyline vertces */
                    color_array.get(),      /* colors at polyline verts */
                    radius);                /* radius of polycylinder */
    RawAppend(glEmulator.vertexList(), glEmulator.normalList(), glEmulator.colourList(), glEmulator.uvList());

    void glePolyCone (int npoints,	 /* numpoints in poly-line */
                     gleDouble point_array[][3],	/* polyline vertices */
                     float color_array[][3],	/* colors at polyline verts */
                     gleDouble radius_array[]); /* cone radii at polyline verts */
}
