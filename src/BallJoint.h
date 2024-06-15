/*
 *  BallJoint.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 29/12/2008.
 *  Copyright 2008 Bill Sellers. All rights reserved.
 *
 */

#ifndef BallJoint_h
#define BallJoint_h

#include "Joint.h"
#include "PGDMath.h"

#include <array>
#include <optional>

namespace GaitSym
{

class Marker;

class BallJoint: public Joint
{
public:

    BallJoint();

    pgd::Quaternion CalculateQuaternion();


    virtual std::string dumpToString();

    virtual std::string *createFromAttributes();
    virtual void appendToAttributes();

    pgd::Vector3 anchor() const;
    void setAnchor(const pgd::Vector3 &newAnchor);


    std::optional<std::array<pgd::Vector2, 3> > stops() const;
    void setStops(std::optional<std::array<pgd::Vector2, 3> > newStops);

private:

    pgd::Vector3 m_anchor;
    std::optional<std::array<pgd::Vector2, 3>> m_stops;
};

}

#endif
