/*
 *  LineWrapStrap.h
 *  GaitSym5
 *
 *  Created by Bill Sellers on 19/06/2026.
 *  Copyright 2026 Bill Sellers. All rights reserved.
 *
 */

#include "LineWrapStrap.h"
#include "Body.h"
#include "PGDMath.h"
#include "Simulation.h"
#include "GSUtil.h"
#include "Marker.h"

#include <algorithm>
#include <cmath>
#include <limits>

using namespace std::string_literals;
namespace GaitSym {

LineWrapStrap::LineWrapStrap()
{
}

LineWrapStrap::~LineWrapStrap()
{
}


void LineWrapStrap::setOriginMarker(Marker *originMarker)
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

void LineWrapStrap::setInsertionMarker(Marker *insertionMarker)
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

void LineWrapStrap::setLineMarkers(Marker *lineStartMarker, Marker *lineEndMarker)
{
    // note that the body is set by lineStartMarker
    m_lineStartMarker = lineStartMarker;
    m_lineEndMarker = lineEndMarker;
    if (pointForceList()->size() <= 2)
    {
        std::unique_ptr<PointForce> line = std::make_unique<PointForce>();
        line->body = m_lineStartMarker->body();
        pointForceList()->push_back(std::move(line));
    }
    else
    {
        pointForceList()->at(2)->body = m_lineStartMarker->body();
    }
}

Marker *LineWrapStrap::originMarker() const
{
    return m_originMarker;
}

Marker *LineWrapStrap::insertionMarker() const
{
    return m_insertionMarker;
}

Marker *LineWrapStrap::lineStartMarker() const
{
    return m_lineStartMarker;
}

Marker *LineWrapStrap::lineEndMarker() const
{
    return m_lineEndMarker;
}

void LineWrapStrap::calculate()
{
    pgd::Vector3 origin = m_originMarker->worldPosition();
    pgd::Vector3 insertion = m_insertionMarker->worldPosition();
    pgd::Vector3 lineStart = m_lineStartMarker->worldPosition();
    pgd::Vector3 lineEnd = m_lineEndMarker->worldPosition();
    pgd::Vector3 W;
    double total_length;
    double t;
    int status = wrapPath(origin, insertion, lineStart, lineEnd, &W, &total_length, &t);
    if (status)
    {
        std::cerr << "Error calculating wrapPath in LineWrapStrap::calculate()\n";
        return;
    }

    if (pointForceList()->size() != 3)
    {
        std::cerr << "Error pointForceList()->size() != 3 in LineWrapStrap::calculate()\n";
        return;
    }

    pgd::Vector3 originForce = pgd::normalize(W - origin);
    pgd::Vector3 insertionForce = pgd::normalize(W - insertion);
    pgd::Vector3 lineForce = -originForce - insertionForce;

    m_pathCoordinates.clear();
    m_pathCoordinates.push_back(origin);
    m_pathCoordinates.push_back(W);
    m_pathCoordinates.push_back(insertion);

    PointForce *originPF = (*pointForceList())[0].get();
    PointForce *insertionPF = (*pointForceList())[1].get();
    PointForce *linePF = (*pointForceList())[2].get();
    originPF->vector = originForce;
    originPF->point = origin;
    insertionPF->vector = insertionForce;
    insertionPF->point = insertion;
    linePF->vector = lineForce;
    linePF->point = W;
}

// function to wrap a path around a line segment
// this differs from the cylinder case in that the path always wraps
// and the line has a finite length

// The pathe goes from A to B and 'wraps' around the line P to Q
// The path crosses the segment PQ at some point W; the optimal path A->W->B minimises |AW| + |WB|.
// Start by projecting A and B onto the segment axis (unit vector d̂ = (Q-P)/|Q-P|):
// a_s = projection of (A-P) onto d̂       (scalar)
// b_s = projection of (B-P) onto d̂
// α   = perpendicular distance from A to the line through PQ
// β   = perpendicular distance from B to the line through PQ

// then |AW(s)| = sqrt(α² + (s-a_s)²)
// and  |BW(s)| = sqrt(β²+(s-b_s)²)
// and the problem reduces to minimising
//  f(s) = sqrt(α²+(s-a_s)²) + sqrt(β²+(s-b_s)²) over s in [0, |PQ|].

// If we allow the line to be infinite, this reduces to
// s* = (a_s·β + b_s·α) / (α + β)
// is a weighted average of the two projections, with each point's weight equal to the OTHER point's perpendicular distance
// to allow for a finite line we clamp s* to [0, |PQ|] so W lies on the segment

// Special cases:
// If both A and B lie on the line (α=β=0), every point on the segment is optimal
// we return the midpoint of [a_s, b_s] clamped to the segment.

// Calculates (W, total_length, t) where:
// W            = crossing point on segment PQ
// total_length = |AW| + |WB|
// t            = parameter in [0,1] with W = P + t*(Q-P)

// returns 0 on success
int LineWrapStrap::wrapPath(const pgd::Vector3 &A, const pgd::Vector3 &B, const pgd::Vector3 &P, const pgd::Vector3 &Q,
                            pgd::Vector3 *W, double *total_length, double *t)
{
    pgd::Vector3 seg = Q - P;
    double seg_len = seg.magnitude();
    if (seg_len < std::numeric_limits<double>::epsilon()) return __LINE__; // check for zero line segment
    pgd::Vector3 d_hat = seg / seg_len; // unit vector along segment

    // Project A and B onto the segment axis
    double a_s = pgd::dot(A - P, d_hat);
    double b_s = pgd::dot(B - P, d_hat);

    // Perpendicular (off-axis) components
    pgd::Vector3 A_perp = (A - P) - a_s * d_hat;
    pgd::Vector3 B_perp = (B - P) - b_s * d_hat;
    double alpha = A_perp.magnitude(); // perp distance, A to line
    double beta = B_perp.magnitude(); // perp distance, B to line

    // Closed-form unconstrained optimum along the axis
    double denom = alpha + beta;
    double s_star;
    if (denom < std::numeric_limits<double>::epsilon()) { s_star = 0.5 * (a_s + b_s); } // Both points lie on the line — any point works; use midpoint of projections
    else { s_star = (a_s * beta + b_s * alpha) / denom; }

    // Clamp to segment [0, seg_len]
    double s_clamped = std::clamp(s_star, 0.0, seg_len);
    *t = s_clamped / seg_len;
    *W = P + *t * seg;
    *total_length = (A - *W).magnitude() + (B - *W).magnitude();
    return 0;
}



const std::vector<pgd::Vector3> *LineWrapStrap::pathCoordinates()
{
    return &m_pathCoordinates;
}

std::string *LineWrapStrap::createFromAttributes()
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
    this->setOriginMarker(originMarker->second.get());
    if (findAttribute("InsertionMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto insertionMarker = simulation()->markerList()->find(buf);
    if (insertionMarker == simulation()->markerList()->end())
    {
        setLastError("STRAP ID=\""s + name() +"\" InsertionMarker not found"s);
        return lastErrorPtr();
    }
    this->setInsertionMarker(insertionMarker->second.get());
    if (findAttribute("LineStartMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto lineStartMarker = simulation()->markerList()->find(buf);
    if (lineStartMarker == simulation()->markerList()->end())
    {
        setLastError("STRAP ID=\""s + name() +"\" LineStartMarkerID not found"s);
        return lastErrorPtr();
    }
    if (findAttribute("LineEndMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto lineEndMarker = simulation()->markerList()->find(buf);
    if (lineEndMarker == simulation()->markerList()->end())
    {
        setLastError("STRAP ID=\""s + name() +"\" LineEndMarkerID not found"s);
        return lastErrorPtr();
    }

    this->setLineMarkers(lineStartMarker->second.get(), lineEndMarker->second.get());

    setUpstreamObjects({m_originMarker, m_insertionMarker, m_lineStartMarker, m_lineEndMarker});
    return nullptr;
}

void LineWrapStrap::appendToAttributes()
{
    Strap::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "LineWrap"s);
    setAttribute("OriginMarkerID"s, m_originMarker->name());
    setAttribute("InsertionMarkerID"s, m_insertionMarker->name());
    setAttribute("LineStartMarkerID"s, m_lineStartMarker->name());
    setAttribute("LineEndMarkerID"s, m_lineEndMarker->name());
}

} // namespace GaitSym


