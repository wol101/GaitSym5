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

FacetedPolyCone::FacetedPolyCone(const std::vector<pgd::Vector3> &vertexList, const std::vector<std::array<float, 3>> &vertexColours, const std::vector<double> &radiusList, size_t nSides, const QColor &blendColour, double blendFraction)
{
    setBlendColour(blendColour, blendFraction);

    glEmulator.clear();
    glEmulator.reserve(nSides * (vertexList.size() * 2 + 2));
    gleSetNumSides(int(nSides));
    gleSetJoinStyle(TUBE_JN_ANGLE | TUBE_JN_CAP | TUBE_NORM_PATH_EDGE | TUBE_CONTOUR_CLOSED); // TUBE_JN_ROUND is pretty but TUBE_JN_ANGLE is probably quicker
    size_t nPoints = vertexList.size() + 2;
    auto pointArray = std::make_unique<gleDouble[][3]>(nPoints);
    auto colourArray = std::make_unique<gleColor[]>(nPoints);
    auto radiusArray = std::make_unique<gleDouble[]>(nPoints);
    size_t index = 0;
    // need to create an extra vertex at the begining and end because it is used to define the normal to the end cap
    pgd::Vector3 prefix = vertexList[0] - (vertexList[1] - vertexList[0]);
    pgd::Vector3 suffix = vertexList[vertexList.size() - 1] + (vertexList[vertexList.size() - 1] - vertexList[vertexList.size() - 2]);
    pointArray[index][0] = prefix.x; pointArray[index][1] = prefix.y; pointArray[index][2] = prefix.z;
    colourArray[index][0] = vertexColours[0][0]; colourArray[index][1] = vertexColours[0][1]; colourArray[index][2] = vertexColours[0][2];
    radiusArray[index] = radiusList[0];
    ++index;
    for (size_t i = 0 ; i < vertexList.size(); i++)
    {
        pointArray[index][0] = vertexList[i].x; pointArray[index][1] = vertexList[i].y; pointArray[index][2] = vertexList[i].z;
        colourArray[index][0] = vertexColours[i][0]; colourArray[index][1] = vertexColours[i][1]; colourArray[index][2] = vertexColours[i][2];
        radiusArray[index] = radiusList[i];
        ++index;
    }
    pointArray[index][0] = suffix.x; pointArray[index][1] = suffix.y; pointArray[index][2] = suffix.z;
    colourArray[index][0] = vertexColours.back()[0]; colourArray[index][1] = vertexColours.back()[1]; colourArray[index][2] = vertexColours.back()[2];
    radiusArray[index] = radiusList.back();
    glePolyCone(int(nPoints), pointArray.get(), colourArray.get(), radiusArray.get());
    RawAppend(glEmulator.vertexList(), glEmulator.normalList(), glEmulator.colourList(), glEmulator.uvList());
}
