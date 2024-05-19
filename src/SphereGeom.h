/*
 *  SphereGeom.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 05/12/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef SphereGeom_h
#define SphereGeom_h

#include "Geom.h"

namespace GaitSym
{

class SphereGeom: public Geom
{
public:

    SphereGeom(double radius);

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    double radius() const;
    void setRadius(double radius);

private:
    double m_radius = 0;
};

}

#endif

