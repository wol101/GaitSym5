/*
 *  TrimeshGeom.h
 *  GaitSym2019
 *
 *  Created by Bill Sellers on 27/02/2021.
 *  Copyright 2021 Bill Sellers. All rights reserved.
 *
 */

#ifndef TRIMESHGEOM_H
#define TRIMESHGEOM_H

#include "Geom.h"

namespace GaitSym
{

class TrimeshGeom: public Geom
{
public:
    TrimeshGeom();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    std::vector<double> *vertices();

    std::vector<int> *triangles();

    bool reverseWinding() const;
    void setReverseWinding(bool newReverseWinding);

    int indexStart() const;
    void setIndexStart(int newIndexStart);

private:
    std::vector<double> m_vertices;
    std::vector<int> m_triangles;
    bool m_reverseWinding = false;
    int m_indexStart = 0; // allows both 1 indexed and 0 indexed triangle to make pasting in OBJ data easier
};

}

#endif // TRIMESHGEOM_H
