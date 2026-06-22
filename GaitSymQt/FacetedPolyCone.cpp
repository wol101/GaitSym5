/*
 *  FacetedPolyCone.h
 *  GaitSym5
 *
 *  Created by Bill Sellers on 10/06/2024
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#include "FacetedPolyCone.h"
#include "PolylineSweep.h"

// this draws a fat polyline with varying radius. It mostly works very well but large radii and sharp turns will casue holes
// this might be fixable by shifting the circle so that the polyline is at an edge for steep turns rather than in the centre
// the problem is that the cut line can interfere with previous line segments and this is difficult to fix

FacetedPolyCone::FacetedPolyCone(const std::vector<pgd::Vector3> &vertexList, const std::vector<std::array<float, 3>> &vertexColours, const std::vector<double> &radiusList, size_t nSides, const QColor &blendColour, double blendFraction)
{
    setBlendColour(blendColour, blendFraction);

    std::vector<PolylineSweep::Vec3> nGon = PolylineSweep::makeNgon(nSides, 1.0);
    std::vector<PolylineSweep::Vec3> polyLine;
    polyLine.reserve(vertexList.size());
    for (auto &&vertex : vertexList) { polyLine.push_back({vertex.x, vertex.y, vertex.z}); }
    PolylineSweep::SweepOptions opts; opts.closeEnds = true; opts.bendSteps = nSides;
    PolylineSweep::PolylineSweep polylineSweep;
    PolylineSweep::Mesh mesh = polylineSweep.sweep(polyLine, nGon, radiusList, opts);

    double tri[9];
    allocateMemory(mesh.triangles.size() * 9);
    for (auto &&triangle : mesh.triangles)
    {
        tri[0] = mesh.vertices[triangle.a].x;
        tri[1] = mesh.vertices[triangle.a].y;
        tri[2] = mesh.vertices[triangle.a].z;
        tri[3] = mesh.vertices[triangle.b].x;
        tri[4] = mesh.vertices[triangle.b].y;
        tri[5] = mesh.vertices[triangle.b].z;
        tri[6] = mesh.vertices[triangle.c].x;
        tri[7] = mesh.vertices[triangle.c].y;
        tri[8] = mesh.vertices[triangle.c].z;
        addTriangle(tri);
    }
}
