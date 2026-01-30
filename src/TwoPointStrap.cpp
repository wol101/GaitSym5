/*
 *  TwoPointStrap.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 29/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#include "TwoPointStrap.h"
#include "PGDMath.h"
#include "Simulation.h"
#include "Marker.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <string.h>

using namespace std::string_literals;
namespace GaitSym {

TwoPointStrap::TwoPointStrap()
{
}

void TwoPointStrap::setOrigin(Marker *originMarker)
{
    m_originMarker = originMarker;
    if (pointForceList()->size() == 0)
    {
        std::unique_ptr<PointForce> origin = std::make_unique<PointForce>();
        origin->body = m_originMarker->body();
        pointForceList()->push_back(std::move(origin));
    }
    else
    {
        pointForceList()->at(0)->body = m_originMarker->body();
    }
}

void TwoPointStrap::setInsertion(Marker *insertionMarker)
{
    m_insertionMarker = insertionMarker;
    if (pointForceList()->size() <= 1)
    {
        std::unique_ptr<PointForce> insertion = std::make_unique<PointForce>();
        insertion->body = m_insertionMarker->body();
        pointForceList()->push_back(std::move(insertion));
    }
    else
    {
        pointForceList()->at(1)->body =  m_insertionMarker->body();
    }
}

void TwoPointStrap::calculate()
{
    PointForce *theOrigin = (*pointForceList())[0].get();
    PointForce *theInsertion = (*pointForceList())[1].get();

    // calculate the world positions
    pgd::Vector3 origin = m_originMarker->worldPosition();
    theOrigin->point[0] = origin.x;
    theOrigin->point[1] = origin.y;
    theOrigin->point[2] = origin.z;
    pgd::Vector3 insertion = m_insertionMarker->worldPosition();
    theInsertion->point[0] = insertion.x;
    theInsertion->point[1] = insertion.y;
    theInsertion->point[2] = insertion.z;

    // calculate the vector from the origin to the insertion
    pgd::Vector3 line;
    line[0] = theInsertion->point[0] - theOrigin->point[0];
    line[1] = theInsertion->point[1] - theOrigin->point[1];
    line[2] = theInsertion->point[2] - theOrigin->point[2];

    // calculate the length and velocity
    double length = std::sqrt(line[0]*line[0] + line[1]*line[1] + line[2]*line[2]);
    if (this->length() >= 0 && simulation() && simulation()->global()->stepSize() > 0) setVelocity((length - this->length()) / simulation()->global()->stepSize());
    else setVelocity(0);
    setLength(length);

    // normalise the direction vector
    line[0] /= this->length();
    line[1] /= this->length();
    line[2] /= this->length();

    theOrigin->vector[0] = line[0];
    theOrigin->vector[1] = line[1];
    theOrigin->vector[2] = line[2];

    // simply reverse the direction for the insertion
    theInsertion->vector[0] = -line[0];
    theInsertion->vector[1] = -line[1];
    theInsertion->vector[2] = -line[2];

    // check that we don't have any non-normal values for directions which can occur if points co-locate
    for (size_t i = 0; i < pointForceList()->size(); i++)
    {
        if ((std::isfinite((*pointForceList())[i]->vector[0]) && std::isfinite((*pointForceList())[i]->vector[1]) && std::isfinite((*pointForceList())[i]->vector[2])) == false)
        {
            (*pointForceList())[i]->vector[0] = 0.0;
            (*pointForceList())[i]->vector[1] = 0.0;
            (*pointForceList())[i]->vector[2] = 0.0;
            std::cerr << "Warning: point force direction in \"" << name() << "\" is invalid so applying standard fixup\n";
        }
    }
}

//int TwoPointStrap::sanityCheck(Strap *otherStrap, Simulation::AxisType axis, const std::string &sanityCheckLeft, const std::string &sanityCheckRight)
//{
//    const double epsilon = DBL_EPSILON;

//    TwoPointStrap *other = dynamic_cast<TwoPointStrap *>(otherStrap);
//    if (other == nullptr) return __LINE__;

//    // first check attachment errors
//    switch (axis)
//    {
//    case Simulation::XAxis:
//        if (fabs(this->m_origin[0] + other->m_origin[0]) > epsilon) return __LINE__;
//        if (fabs(this->m_origin[1] - other->m_origin[1]) > epsilon) return __LINE__;
//        if (fabs(this->m_origin[2] - other->m_origin[2]) > epsilon) return __LINE__;
//        if (fabs(this->m_insertion[0] + other->m_insertion[0]) > epsilon) return __LINE__;
//        if (fabs(this->m_insertion[1] - other->m_insertion[1]) > epsilon) return __LINE__;
//        if (fabs(this->m_insertion[2] - other->m_insertion[2]) > epsilon) return __LINE__;
//        break;

//    case Simulation::YAxis:
//        if (fabs(this->m_origin[0] - other->m_origin[0]) > epsilon) return __LINE__;
//        if (fabs(this->m_origin[1] + other->m_origin[1]) > epsilon) return __LINE__;
//        if (fabs(this->m_origin[2] - other->m_origin[2]) > epsilon) return __LINE__;
//        if (fabs(this->m_insertion[0] - other->m_insertion[0]) > epsilon) return __LINE__;
//        if (fabs(this->m_insertion[1] + other->m_insertion[1]) > epsilon) return __LINE__;
//        if (fabs(this->m_insertion[2] - other->m_insertion[2]) > epsilon) return __LINE__;
//        break;

//    case Simulation::ZAxis:
//        if (fabs(this->m_origin[0] - other->m_origin[0]) > epsilon) return __LINE__;
//        if (fabs(this->m_origin[1] - other->m_origin[1]) > epsilon) return __LINE__;
//        if (fabs(this->m_origin[2] + other->m_origin[2]) > epsilon) return __LINE__;
//        if (fabs(this->m_insertion[0] - other->m_insertion[0]) > epsilon) return __LINE__;
//        if (fabs(this->m_insertion[1] - other->m_insertion[1]) > epsilon) return __LINE__;
//        if (fabs(this->m_insertion[2] + other->m_insertion[2]) > epsilon) return __LINE__;
//        break;
//    }

//    // now check for left to right crossover errors
//    if (this->name().find(sanityCheckLeft) != std::string::npos)
//    {
//        if (m_originBody->name().find(sanityCheckRight) != std::string::npos) return __LINE__;
//        if (m_insertionBody->name().find(sanityCheckRight) != std::string::npos) return __LINE__;
//    }
//    if (this->name().find(sanityCheckRight) != std::string::npos)
//    {
//        if (m_originBody->name().find(sanityCheckLeft) != std::string::npos) return __LINE__;
//        if (m_insertionBody->name().find(sanityCheckLeft) != std::string::npos) return __LINE__;
//    }

//    return 0;
//}

std::string *TwoPointStrap::createFromAttributes()
{
    if (Strap::createFromAttributes()) return lastErrorPtr();

    std::string buf;

    if (findAttribute("OriginMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto originMarker = simulation()->markerList()->find(buf);
    if (originMarker == simulation()->markerList()->end())
    {
        setLastError("STRAP ID=\""s + name() +"\" OriginMarker not found"s);
        return lastErrorPtr();
    }
    this->setOrigin(originMarker->second.get());
    if (findAttribute("InsertionMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto insertionMarker = simulation()->markerList()->find(buf);
    if (insertionMarker == simulation()->markerList()->end())
    {
        setLastError("STRAP ID=\""s + name() +"\" InsertionMarker not found"s);
        return lastErrorPtr();
    }
    this->setInsertion(insertionMarker->second.get());

    setUpstreamObjects({m_originMarker, m_insertionMarker});
    return nullptr;
}

void TwoPointStrap::appendToAttributes()
{
    Strap::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "TwoPoint"s);
    setAttribute("OriginMarkerID"s, m_originMarker->name());
    setAttribute("InsertionMarkerID"s, m_insertionMarker->name());
}

Marker *TwoPointStrap::originMarker() const
{
    return m_originMarker;
}

Marker *TwoPointStrap::insertionMarker() const
{
    return m_insertionMarker;
}



} // namespace GaitSym


