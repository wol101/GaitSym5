/*
 *  CylinderWrapStrap.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 29/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#include "CylinderWrapStrap.h"
#include "Body.h"
#include "PGDMath.h"
#include "DataFile.h"
#include "Simulation.h"
#include "GSUtil.h"
#include "Marker.h"

#include <cmath>
#include <string.h>

using namespace std::string_literals;
namespace GaitSym {

CylinderWrapStrap::CylinderWrapStrap()
{
}

CylinderWrapStrap::~CylinderWrapStrap()
{
}


void CylinderWrapStrap::SetOrigin(Marker *originMarker)
{
    m_originMarker = originMarker;
    if (GetPointForceList()->size() == 0)
    {
        std::unique_ptr<PointForce> origin = std::make_unique<PointForce>();
        origin->body = m_originMarker->GetBody();
        GetPointForceList()->push_back(std::move(origin));
    }
    else
    {
        GetPointForceList()->at(0)->body = m_originMarker->GetBody();
    }
}

void CylinderWrapStrap::SetInsertion(Marker *insertionMarker)
{
    m_insertionMarker = insertionMarker;
    if (GetPointForceList()->size() <= 1)
    {
        std::unique_ptr<PointForce> insertion = std::make_unique<PointForce>();
        insertion->body = m_insertionMarker->GetBody();
        GetPointForceList()->push_back(std::move(insertion));
    }
    else
    {
        GetPointForceList()->at(1)->body =  m_insertionMarker->GetBody();
    }
}

void CylinderWrapStrap::SetCylinder(Marker *cylinderMarker)
{
    m_cylinderMarker = cylinderMarker;
    if (GetPointForceList()->size() <= 2)
    {
        std::unique_ptr<PointForce> cylinder = std::make_unique<PointForce>();
        cylinder->body = m_cylinderMarker->GetBody();
        GetPointForceList()->push_back(std::move(cylinder));
    }
    else
    {
        GetPointForceList()->at(2)->body = m_cylinderMarker->GetBody();
    }
}

void CylinderWrapStrap::SetCylinderRadius(double radius)
{
    m_cylinderRadius = radius;
}

void CylinderWrapStrap::SetNumWrapSegments(int numWrapSegments)
{
    m_numWrapSegments = numWrapSegments;
    m_pathCoordinates.reserve(size_t(m_numWrapSegments) + 2);
}

int CylinderWrapStrap::GetNumWrapSegments()
{
    return m_numWrapSegments;
}

Marker *CylinderWrapStrap::GetOriginMarker() const
{
    return m_originMarker;
}

Marker *CylinderWrapStrap::GetInsertionMarker() const
{
    return m_insertionMarker;
}

Marker *CylinderWrapStrap::GetCylinderMarker() const
{
    return m_cylinderMarker;
}

void CylinderWrapStrap::Calculate()
{
    pgd::Quaternion qOriginBody = GetOriginMarker()->GetBody()->GetQuaternion();
    pgd::Vector3 vOriginBody = GetOriginMarker()->GetBody()->GetPosition();
    pgd::Quaternion qInsertionBody = GetInsertionMarker()->GetBody()->GetQuaternion();
    pgd::Vector3 vInsertionBody = GetInsertionMarker()->GetBody()->GetPosition();
    pgd::Quaternion qCylinderBody = GetCylinderMarker()->GetBody()->GetQuaternion();
    pgd::Vector3 vCylinderBody = GetCylinderMarker()->GetBody()->GetPosition();

    pgd::Vector3 m_originPosition = GetOriginMarker()->GetPosition();
    pgd::Vector3 m_insertionPosition = GetInsertionMarker()->GetPosition();
    pgd::Vector3 m_cylinderPosition = GetCylinderMarker()->GetPosition();
    Body *m_originBody = GetOriginMarker()->GetBody();
    Body *m_insertionBody = GetInsertionMarker()->GetBody();
    Body *m_cylinderBody = GetCylinderMarker()->GetBody();

    // the cylinder quaternion in this implementation is the quaternion that rotates the the x axis of the marker to the z axis
    pgd::Vector3 v2 = GetCylinderMarker()->GetAxis(Marker::Axis::X);
    pgd::Vector3 v1(0, 0, 1); // and this is the Z axis we need to rotate
    pgd::Quaternion m_cylinderQuaternion = pgd::FindRotation(v1, v2);

    // calculate some inverses
    pgd::Quaternion qCylinderBodyInv = ~qCylinderBody;
    pgd::Quaternion qCylinderQuaternionInv = ~m_cylinderQuaternion;

    // get the world coordinates of the origin and insertion
    pgd::Vector3 worldOriginPosition = QVRotate(qOriginBody, m_originPosition) + vOriginBody;
    pgd::Vector3 worldInsertionPosition = QVRotate(qInsertionBody, m_insertionPosition) + vInsertionBody;

    // now calculate as cylinder coordinates
    pgd::Vector3 v;
    if (m_originBody == m_cylinderBody) v = m_originPosition;
    else v = QVRotate(qCylinderBodyInv, worldOriginPosition - vCylinderBody);
    pgd::Vector3 cylinderOriginPosition = QVRotate(qCylinderQuaternionInv, v - m_cylinderPosition);

    if (m_insertionBody == m_cylinderBody) v = m_insertionPosition;
    else v = QVRotate(qCylinderBodyInv, worldInsertionPosition - vCylinderBody);
    pgd::Vector3 cylinderInsertionPosition = QVRotate(qCylinderQuaternionInv, v - m_cylinderPosition);

    // std::cerr << "cylinderOriginPosition " << cylinderOriginPosition.x << " " << cylinderOriginPosition.y << " " << cylinderOriginPosition.z << " ";
    // std::cerr << "cylinderInsertionPosition " << cylinderInsertionPosition.x << " " << cylinderInsertionPosition.y << " " << cylinderInsertionPosition.z << "\n";

    pgd::Vector3 theOriginForce;
    pgd::Vector3 theInsertionForce;
    pgd::Vector3 theCylinderForce;
    pgd::Vector3 theCylinderForcePosition;

    double length = 0;
    m_wrapStatus = CylinderWrap(cylinderOriginPosition, cylinderInsertionPosition, m_cylinderRadius, m_numWrapSegments, M_PI,
                                theOriginForce, theInsertionForce, theCylinderForce, theCylinderForcePosition,
                                &length, &m_pathCoordinates);
    if (m_wrapStatus == -1) {
        std::cerr << "Warning: wrapping impossible in \"" << name() << "\" - attachment inside cylinder\n"; }
    if (Length() >= 0 && simulation() && simulation()->GetTimeIncrement() > 0) setVelocity((length - Length()) / simulation()->GetTimeIncrement());
    else setVelocity(0);
    setLength(length);

    // now rotate back to world reference frame

    theOriginForce = QVRotate(qCylinderBody, QVRotate(m_cylinderQuaternion, theOriginForce));
    theInsertionForce = QVRotate(qCylinderBody, QVRotate(m_cylinderQuaternion, theInsertionForce));
    theCylinderForce = QVRotate(qCylinderBody, QVRotate(m_cylinderQuaternion, theCylinderForce));
    theCylinderForcePosition = QVRotate(m_cylinderQuaternion, theCylinderForcePosition) + m_cylinderPosition;
    theCylinderForcePosition = QVRotate(qCylinderBody, theCylinderForcePosition) + vCylinderBody;


    PointForce *theOrigin = (*GetPointForceList())[0].get();
    PointForce *theInsertion = (*GetPointForceList())[1].get();
    PointForce *theCylinder = (*GetPointForceList())[2].get();
    theOrigin->vector[0] = theOriginForce.x; theOrigin->vector[1] = theOriginForce.y; theOrigin->vector[2] = theOriginForce.z;
    theOrigin->point[0] = worldOriginPosition.x; theOrigin->point[1] = worldOriginPosition.y; theOrigin->point[2] = worldOriginPosition.z;
    theInsertion->vector[0] = theInsertionForce.x; theInsertion->vector[1] = theInsertionForce.y; theInsertion->vector[2] = theInsertionForce.z;
    theInsertion->point[0] = worldInsertionPosition.x; theInsertion->point[1] = worldInsertionPosition.y; theInsertion->point[2] = worldInsertionPosition.z;
    theCylinder->vector[0] = theCylinderForce.x; theCylinder->vector[1] = theCylinderForce.y; theCylinder->vector[2] = theCylinderForce.z;
    theCylinder->point[0] = theCylinderForcePosition.x; theCylinder->point[1] = theCylinderForcePosition.y; theCylinder->point[2] = theCylinderForcePosition.z;

    // and handle the path coordinates
    for (size_t i = 0; i < m_pathCoordinates.size(); i++)
    {
        m_pathCoordinates[i] = QVRotate(m_cylinderQuaternion, m_pathCoordinates[i]) + m_cylinderPosition;
        m_pathCoordinates[i] = QVRotate(qCylinderBody, m_pathCoordinates[i]) + vCylinderBody;
    }

    // check that we don't have any non-finite values for directions which can occur if points co-locate
    for (size_t i = 0; i < GetPointForceList()->size(); i++)
    {
        if ((std::isfinite((*GetPointForceList())[i]->vector[0]) && std::isfinite((*GetPointForceList())[i]->vector[1]) && std::isfinite((*GetPointForceList())[i]->vector[2])) == false)
        {
            (*GetPointForceList())[i]->vector[0] = 0.0;
            (*GetPointForceList())[i]->vector[1] = 0.0;
            (*GetPointForceList())[i]->vector[2] = 0.0;
            std::cerr << "Warning: point force direction in \"" << name() << "\" is invalid so applying standard fixup\n";
        }
    }
}

// function to wrap a line around a cylinder

// the cylinder is assumed to have its axis along the z axis and the wrapping is according
// to the right hand rule

// the coordinate system is right handed too

// returns -1 if a solution is impossible
// returns 0 if wrapping is unnecessary
// returns 1 if wrapping occurs
int CylinderWrapStrap::CylinderWrap(pgd::Vector3 &origin, pgd::Vector3 &insertion, double radius, int nWrapSegments, double maxAngle,
                 pgd::Vector3 &originForce, pgd::Vector3 &insertionForce, pgd::Vector3 &cylinderForce, pgd::Vector3 &cylinderForcePosition,
                 double *pathLength, std::vector<pgd::Vector3> *pathCoordinates)
{
    pathCoordinates->clear();
    // first of all calculate the planar case looking down the axis of the cylinder (i.e. xy plane)
    // this is standard tangent to a circle stuff

    // Let's define some variables:
    // origin.x: the x coordinate of the origin
    // origin.y: the y coordinate of the origin
    // d1: the distance of origin from the centre
    // l1: the distance of the origin from the circle along its tangent
    // theta1: the angle of the line from the centre to the origin
    // phi1:the angle at the centre formed by the origin triangle
    // radius: the radius of the circle

    double d1 = sqrt(origin.x*origin.x + origin.y*origin.y);
    // error condition
    if (d1 <= radius)
    {
        return -1;
    }

    double theta1=atan2(origin.y, origin.x);

    double phi1=acos(radius/d1);

    double l1=d1 * sin(phi1);

    // The spherical coordinates of the tangent point are (radius, theta1+phi1)

    double tangentPointTheta1 = theta1 + phi1;

    double tangentPointX1 = radius * cos(tangentPointTheta1);

    double tangentPointY1 = radius * sin(tangentPointTheta1);

    // More variables:
    // insertion.x: the x coordinate of the insertion
    // insertion.y: the y coordinate of the inserttion
    // d2:the distance of the insertion from the centre
    // l2:the distance of the insertion from the centre
    // theta2:the angle of the line from the centre to the insertion
    // phi2:the angle at the centre formed by the insertion triangle

    double d2 = sqrt(insertion.x*insertion.x + insertion.y*insertion.y);
    // error condition
    if (d2 <= radius)
    {
        return -1;
    }


    double theta2 = atan2(insertion.y, insertion.x);

    double phi2 = acos(radius/d2);

    double l2 = d2 * sin(phi2);

    // The spherical coordinates of the tangent point are (radius, theta2-phi2)

    double tangentPointTheta2 = theta2 - phi2;

    double tangentPointX2 = radius * cos(tangentPointTheta2);

    double tangentPointY2 = radius * sin(tangentPointTheta2);

    // rho: the angle around the circumference of the path in this plane
    // c:the distance around the circumference of the path in this plane
    double rho = tangentPointTheta2 - tangentPointTheta1;
    while (rho < 0)
        rho = rho + 2 * M_PI;
    while (rho > 2 * M_PI)
        rho = rho - 2 * M_PI;

    double c = radius * rho;

    // Finally we need to decide whether the path wraps at all.
    // We do this by seeing if the wrap angle is greater than a user specified limit.
    // Useful limits will be between M_PI and 2 M_PI
    // also check if the angle is greater than some small threshold
    // since a tiny rho will lead to zero length segments later

    if (rho > maxAngle || rho < 1e-10)
    {
        // now calculate some forces
        originForce = insertion - origin;
        *pathLength = originForce.Magnitude();
        originForce.Normalize();

        insertionForce = -originForce;
        cylinderForce.x = cylinderForce.y = cylinderForce.z = 0;
        cylinderForcePosition.x = cylinderForcePosition.y = cylinderForcePosition.z = 0;

        // and a simple straight path
        if (pathCoordinates && nWrapSegments > 1)
        {
            pathCoordinates->push_back(origin);
            pathCoordinates->push_back(insertion);
        }

        return 0;
    }

    // OK, that's the x and y bits sorted. Now work out the z coordinates

    // The key point about the unwrapped problem is that the gradient is constant.
    // This means that the path around the cylinder is a helix that is continuous
    // with the line segments at each end.

    // origin.z: z coordinate of origin
    // insertion.z: z coordinate of origin
    // delz: height change
    // delz:horizontal distance
    // tangentPointZ1: origin tangent point
    // tangetPointZ2: insertion tangent point

    double delz = insertion.z - origin.z;

    double delx = l1 + c + l2;

    double tangentPointZ1 = origin.z + (l1/delx) * delz;

    double tangentPointZ2 = tangentPointZ1 + (c/delx) * delz;

    // now calculate some forces

    originForce.x = tangentPointX1 - origin.x;
    originForce.y = tangentPointY1 - origin.y;
    originForce.z = tangentPointZ1 - origin.z;
    originForce.Normalize();

    insertionForce.x = tangentPointX2 - insertion.x;
    insertionForce.y = tangentPointY2 - insertion.y;
    insertionForce.z = tangentPointZ2 - insertion.z;
    insertionForce.Normalize();

    cylinderForce = -originForce - insertionForce;

    cylinderForcePosition.x = 0;
    cylinderForcePosition.y = 0;
    cylinderForcePosition.z = (tangentPointZ1 + tangentPointZ2) / 2;

    *pathLength = sqrt(delx*delx + delz*delz);

    // that's the main calculations done.
    // Now check whether we need more points for drawing the path.
    int i;
    pgd::Vector3 p;
    if (pathCoordinates && nWrapSegments > 1)
    {
        pathCoordinates->push_back(origin);
        double delAngle = rho / (nWrapSegments);
        double angle = tangentPointTheta1;
        double delZ = (tangentPointZ2 - tangentPointZ1) / (nWrapSegments);
        p.z = tangentPointZ1;
        p.x = radius * cos(angle);
        p.y = radius * sin(angle);
        pathCoordinates->push_back(p);
        for (i = 0; i < nWrapSegments; i++)
        {
            angle = angle + delAngle;
            p.x = radius * cos(angle);
            p.y = radius * sin(angle);
            p.z += delZ;
            pathCoordinates->push_back(p);
        }
        pathCoordinates->push_back(insertion);
    }

    return 1;
}

double CylinderWrapStrap::cylinderRadius() const
{
    return m_cylinderRadius;
}

const std::vector<pgd::Vector3> *CylinderWrapStrap::GetPathCoordinates()
{
    return &m_pathCoordinates;
}


//int CylinderWrapStrap::SanityCheck(Strap *otherStrap, Simulation::AxisType axis, const std::string &sanityCheckLeft, const std::string &sanityCheckRight)
//{
//    const double epsilon = 1e-10;

//    CylinderWrapStrap *other = dynamic_cast<CylinderWrapStrap *>(otherStrap);
//    if (other == nullptr) return __LINE__;

//    if (fabs(this->m_cylinderRadius - other->m_cylinderRadius) > epsilon) return __LINE__;

//    // first check attachment errors
//    switch (axis)
//    {
//    case Simulation::XAxis:
//        if (fabs(this->m_originPosition.x + other->m_originPosition.x) > epsilon) return __LINE__;
//        if (fabs(this->m_originPosition.y - other->m_originPosition.y) > epsilon) return __LINE__;
//        if (fabs(this->m_originPosition.z - other->m_originPosition.z) > epsilon) return __LINE__;
//        if (fabs(this->m_insertionPosition.x + other->m_insertionPosition.x) > epsilon) return __LINE__;
//        if (fabs(this->m_insertionPosition.y - other->m_insertionPosition.y) > epsilon) return __LINE__;
//        if (fabs(this->m_insertionPosition.z - other->m_insertionPosition.z) > epsilon) return __LINE__;
//        if (fabs(this->m_cylinderPosition.x + other->m_cylinderPosition.x) > epsilon) return __LINE__;
//        if (fabs(this->m_cylinderPosition.y - other->m_cylinderPosition.y) > epsilon) return __LINE__;
//        if (fabs(this->m_cylinderPosition.z - other->m_cylinderPosition.z) > epsilon) return __LINE__;
//        break;

//    case Simulation::YAxis:
//        if (fabs(this->m_originPosition.x - other->m_originPosition.x) > epsilon) return __LINE__;
//        if (fabs(this->m_originPosition.y + other->m_originPosition.y) > epsilon) return __LINE__;
//        if (fabs(this->m_originPosition.z - other->m_originPosition.z) > epsilon) return __LINE__;
//        if (fabs(this->m_insertionPosition.x - other->m_insertionPosition.x) > epsilon) return __LINE__;
//        if (fabs(this->m_insertionPosition.y + other->m_insertionPosition.y) > epsilon) return __LINE__;
//        if (fabs(this->m_insertionPosition.z - other->m_insertionPosition.z) > epsilon) return __LINE__;
//        if (fabs(this->m_cylinderPosition.x - other->m_cylinderPosition.x) > epsilon) return __LINE__;
//        if (fabs(this->m_cylinderPosition.y + other->m_cylinderPosition.y) > epsilon) return __LINE__;
//        if (fabs(this->m_cylinderPosition.z - other->m_cylinderPosition.z) > epsilon) return __LINE__;
//        break;

//    case Simulation::ZAxis:
//        if (fabs(this->m_originPosition.x - other->m_originPosition.x) > epsilon) return __LINE__;
//        if (fabs(this->m_originPosition.y - other->m_originPosition.y) > epsilon) return __LINE__;
//        if (fabs(this->m_originPosition.z + other->m_originPosition.z) > epsilon) return __LINE__;
//        if (fabs(this->m_insertionPosition.x - other->m_insertionPosition.x) > epsilon) return __LINE__;
//        if (fabs(this->m_insertionPosition.y - other->m_insertionPosition.y) > epsilon) return __LINE__;
//        if (fabs(this->m_insertionPosition.z + other->m_insertionPosition.z) > epsilon) return __LINE__;
//        if (fabs(this->m_cylinderPosition.x - other->m_cylinderPosition.x) > epsilon) return __LINE__;
//        if (fabs(this->m_cylinderPosition.y - other->m_cylinderPosition.y) > epsilon) return __LINE__;
//        if (fabs(this->m_cylinderPosition.z + other->m_cylinderPosition.z) > epsilon) return __LINE__;
//        break;
//    }

//    // now check for left to right crossover errors
//    if (this->name().find(sanityCheckLeft) != std::string::npos)
//    {
//        if (m_originBody->name().find(sanityCheckRight) != std::string::npos) return __LINE__;
//        if (m_insertionBody->name().find(sanityCheckRight) != std::string::npos) return __LINE__;
//        if (m_cylinderBody->name().find(sanityCheckRight) != std::string::npos) return __LINE__;
//    }
//    if (this->name().find(sanityCheckRight) != std::string::npos)
//    {
//        if (m_originBody->name().find(sanityCheckLeft) != std::string::npos) return __LINE__;
//        if (m_insertionBody->name().find(sanityCheckLeft) != std::string::npos) return __LINE__;
//        if (m_cylinderBody->name().find(sanityCheckLeft) != std::string::npos) return __LINE__;
//    }

//    return 0;
//}

std::string *CylinderWrapStrap::createFromAttributes()
{
    if (Strap::createFromAttributes()) return lastErrorPtr();

    std::string buf;

    if (findAttribute("OriginMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto originMarker = simulation()->GetMarkerList()->find(buf);
    if (originMarker == simulation()->GetMarkerList()->end())
    {
        setLastError("STRAP ID=\""s + name() +"\" OriginMarker not found"s);
        return lastErrorPtr();
    }
    this->SetOrigin(originMarker->second.get());
    if (findAttribute("InsertionMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto insertionMarker = simulation()->GetMarkerList()->find(buf);
    if (insertionMarker == simulation()->GetMarkerList()->end())
    {
        setLastError("STRAP ID=\""s + name() +"\" InsertionMarker not found"s);
        return lastErrorPtr();
    }
    this->SetInsertion(insertionMarker->second.get());
    if (findAttribute("CylinderMarkerID"s, &buf) == nullptr) return lastErrorPtr();
    auto cylinderMarker = simulation()->GetMarkerList()->find(buf);
    if (cylinderMarker == simulation()->GetMarkerList()->end())
    {
        setLastError("STRAP ID=\""s + name() +"\" CylinderMarker not found"s);
        return lastErrorPtr();
    }
    this->SetCylinder(cylinderMarker->second.get());
    if (findAttribute("CylinderRadius"s, &buf) == nullptr) return lastErrorPtr();
    this->SetCylinderRadius(GSUtil::Double(buf));

    setUpstreamObjects({m_originMarker, m_insertionMarker, m_cylinderMarker});
    return nullptr;
}

void CylinderWrapStrap::appendToAttributes()
{
    Strap::appendToAttributes();
    std::string buf;
    setAttribute("Type"s, "CylinderWrap"s);
    setAttribute("OriginMarkerID"s, m_originMarker->name());
    setAttribute("InsertionMarkerID"s, m_insertionMarker->name());
    setAttribute("CylinderMarkerID"s, m_cylinderMarker->name());
    setAttribute("CylinderRadius"s, *GSUtil::ToString(m_cylinderRadius, &buf));
}

} // namespace GaitSym


