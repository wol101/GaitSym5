/*
 *  ConvexGeom.h
 *  GaitSym2019
 *
 *  Created by Bill Sellers on 27/02/2021.
 *  Copyright 2021 Bill Sellers. All rights reserved.
 *
 */

#ifndef CONVEXGEOM_H
#define CONVEXGEOM_H

#include "Geom.h"

namespace GaitSym
{

class ConvexGeom: public Geom
{
public:
    ConvexGeom();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    std::vector<double> *vertices();

    std::vector<int> *triangles();

private:
    std::vector<double> m_vertices;
    std::vector<int> m_triangles;
    bool m_reverseWinding = false;
    int m_indexStart = 0; // allows both 1 indexed and 0 indexed triangle to make pasting in OBJ data easier
};

}

#endif // CONVEXGEOM_H
