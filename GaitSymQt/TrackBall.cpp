/*
 *  Trackball.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 08/08/2009.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#include <cmath>

#include "TrackBall.h"

Trackball::Trackball()
{
}


// called with the mouse start position and the trackbal dimensions
// note: clicks outside the trackball radius have a different rotation behaviour
// note: values given in window coordinates with raster origin at top left
void Trackball::startTrackball(int mouseX, int mouseY, int trackballOriginX, int trackballOriginY,
                               int trackballRadius, const pgd::Vector3 &up, const pgd::Vector3 &out)
{
    m_trackballRadius = trackballRadius;
    m_startMouseX = mouseX;
    m_startMouseY = mouseY;
    m_trackballOriginX = trackballOriginX;
    m_trackballOriginY = trackballOriginY;
    m_out = out;
    m_up = up;
    m_out.normalize();
    m_up.normalize();
    m_left = m_up ^ m_out;
    m_left.normalize();

    double dx = m_startMouseX - m_trackballOriginX;
    double dy = m_trackballOriginY - m_startMouseY;
    double r = sqrt(dx * dx + dy * dy);
    if (r > trackballRadius) m_outsideRadius = true;
    else m_outsideRadius = false;
}

// calculated rotation based on current mouse position
void Trackball::rollTrackballToClick(int mouseX, int mouseY, pgd::Quaternion *rotation)
{
    if (mouseX == m_startMouseX && mouseY == m_startMouseY)
    {
        rotation->n = 1;
        rotation->x = rotation->y = rotation->z = 0;
        return;
    }
    pgd::Vector3 v1;
    pgd::Vector3 v2;
    if (m_outsideRadius == false)   // normal behaviour
    {
        v1 = (m_startMouseX - m_trackballOriginX) * m_left + (m_trackballOriginY - m_startMouseY) * m_up +
             m_trackballRadius * m_out;
        v2 = (mouseX - m_trackballOriginX) * m_left + (m_trackballOriginY - mouseY) * m_up + m_trackballRadius *
             m_out;

    }
    else     // rotate around axis coming out of screen
    {
        v1 = (m_startMouseX - m_trackballOriginX) * m_left + (m_trackballOriginY - m_startMouseY) * m_up;
        v2 = (mouseX - m_trackballOriginX) * m_left + (m_trackballOriginY - mouseY) * m_up;
    }

    // cross product will get us the rotation axis
    pgd::Vector3 axis = v1 ^ v2;

    // Use atan for a better angle.  If you use only cos or sin, you only get
    // half the possible angles, and you can end up with rotations that flip around near
    // the poles.

    // cos angle obtained from dot product formula
    // cos(a) = (s . e) / (||s|| ||e||)
    double cosAng = v1 * v2; // (s . e)
    double ls = v1.magnitude();
    ls = 1. / ls; // 1 / ||s||
    double le = v2.magnitude();
    le = 1. / le; // 1 / ||e||
    cosAng = cosAng * ls * le;

    // sin angle obtained from cross product formula
    // sin(a) = ||(s X e)|| / (||s|| ||e||)
    double sinAng = axis.magnitude(); // ||(s X e)||;
    sinAng = sinAng * ls * le;
    double angle = atan2(sinAng, cosAng); // rotations are in radians.

    *rotation = pgd::makeQFromAxisAngle(axis.x, axis.y, axis.z, angle);
}

