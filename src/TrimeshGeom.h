/*
 *  TrimeshGeom.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 11/08/2009.
 *  Copyright 2009 Bill Sellers. All rights reserved.
 *
 */

#ifndef TRIMESHGEOM_H
#define TRIMESHGEOM_H

#include "Geom.h"

namespace GaitSym
{

class TrimeshGeom : public Geom
{
public:
    TrimeshGeom(const std::vector<double> &vertexList);

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

private:
    std::vector<double> m_vertexList;
};

}

#endif // TRIMESHGEOM_H

