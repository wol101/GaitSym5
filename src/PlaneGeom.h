/*
 *  PlaneGeom.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 13/09/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef PlaneGeom_h
#define PlaneGeom_h

#include "Geom.h"

// The plane equation is:
// a * x + b * y + c * z = d
// The plane's normal vector is (a, b, c), and it must have length 1.

class PlaneGeom: public Geom
{
public:

    PlaneGeom(double a, double b, double c, double d);

    void SetPlane(double a, double b, double c, double d);
    void GetPlane(double *a, double *b, double *c, double *d);

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

private:
    double m_a;
    double m_b;
    double m_c;
    double m_d;
};


#endif
