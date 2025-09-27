/*
 *  Trackball.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 08/08/2009.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef TRACKBALL_H
#define TRACKBALL_H

#include "PGDMath.h"

class Trackball
{
public:
    Trackball();

    // called with the mouse start position and the trackbal dimensions
    // note: clicks outside the trackball radius have a different rotation behaviour
    // note: values given in window coordinates with raster origin at top left
    void startTrackball(int mouseX, int mouseY, int trackballOriginX, int trackballOriginY, int trackballRadius, const pgd::Vector3 &up, const pgd::Vector3 &out);

    // calculated rotation based on current mouse position
    void rollTrackballToClick(int mouseX, int mouseY, pgd::Quaternion *rotation);

    int trackballRadius() { return m_trackballRadius; }
    bool outsideRadius() { return m_outsideRadius; }

private:

    int m_trackballRadius;
    int m_startMouseX;
    int m_startMouseY;
    int m_trackballOriginX;
    int m_trackballOriginY;
    bool m_outsideRadius;

    pgd::Vector3 m_left;
    pgd::Vector3 m_up;
    pgd::Vector3 m_out;
};

#endif // TRACKBALL_H
