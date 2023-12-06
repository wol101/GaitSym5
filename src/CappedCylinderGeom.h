/*
 *  CappedCylinderGeom.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 28/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef CappedCylinderGeom_h
#define CappedCylinderGeom_h

#include "Geom.h"

class CappedCylinderGeom:public Geom
{
public:

    CappedCylinderGeom(double radius, double length);

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    void setLengthRadius(double length, double radius);
    void getLengthRadius(double *length, double *radius) const;

private:
    double m_radius = 0;
    double m_length = 0;
};


#endif

