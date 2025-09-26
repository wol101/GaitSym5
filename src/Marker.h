/*
 *  Marker.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 22/08/2009.
 *  Copyright 2009 Bill Sellers. All rights reserved.
 *
 */

#ifndef Marker_h
#define Marker_h

#include "NamedObject.h"
#include "PGDMath.h"
#include "SmartEnum.h"

namespace GaitSym
{

class Body;

class Marker: public NamedObject
{
public:

    Marker(Body *body);

    // these functions set the geom position relative to its body
    void setPosition(double x, double y, double z);
    void setQuaternion(double qs0, double qx1, double qy2, double qz3);
    void setPosition(const pgd::Vector3 &position);
    void setQuaternion(const pgd::Quaternion &quaternion);
    std::string *setPosition (const std::string &buf);
    std::string *setPosition(const std::string &body, double x, double y, double z);
    void setWorldPosition(double x, double y, double z);
    void setWorldPosition(const pgd::Vector3 &pWorld);
    std::string *setQuaternion(const std::string &buf);
    std::string *setQuaternion(const std::string &body, double qs0, double qx1, double qy2, double qz3);
    void setWorldQuaternion(double qs0, double qx1, double qy2, double qz3);
    void setWorldQuaternion(const pgd::Quaternion &qWorld);
    void offsetPosition(double x, double y, double z);

    SMART_ENUM(Axis, axisStrings, axisCount, X, Y, Z);

    pgd::Vector3 position() const;
    pgd::Quaternion quaternion() const;
    pgd::Vector3 axis(Marker::Axis axis) const;
    void getBasis(pgd::Vector3 *x, pgd::Vector3 *y, pgd::Vector3 *z) const;
    pgd::Matrix3x3 basis() const;
    pgd::Vector3 worldPosition() const;
    pgd::Quaternion worldQuaternion() const;
    pgd::Vector3 worldAxis(Marker::Axis axis) const;
    void getWorldBasis(pgd::Vector3 *x, pgd::Vector3 *y, pgd::Vector3 *z) const;
    pgd::Matrix3x3 worldBasis() const;
    pgd::Vector3 worldLinearVelocity() const;
    pgd::Vector3 worldAngularVelocity() const;
    pgd::Vector3 linearVelocity() const;
    pgd::Vector3 angularVelocity() const;

    pgd::Vector3 constructionPosition() const;

    // these functions get things into and out of marker based coordinates
    pgd::Vector3 position(const pgd::Vector3 &worldCoordinates) const;
    pgd::Vector3 worldPosition(const pgd::Vector3 &localCoordinates) const;
    pgd::Vector3 vector(const pgd::Vector3 &worldVector) const;
    pgd::Vector3 worldVector(const pgd::Vector3 &localVector) const;
    pgd::Quaternion worldQuaternion(const pgd::Quaternion &localQuaternion) const;
    pgd::Quaternion quaternion(const pgd::Quaternion &worldQuaternion) const;

    // utility statics
    static pgd::Vector3 position(const Body &body, const pgd::Vector3 &worldCoordinates);
    static pgd::Vector3 worldPosition(const Body &body, const pgd::Vector3 &localCoordinates);
    static pgd::Vector3 vector(const Body &body, const pgd::Vector3 &worldVector);
    static pgd::Vector3 worldVector(const Body &body, const pgd::Vector3 &localVector);
    static pgd::Quaternion worldQuaternion(const Body &body, const pgd::Quaternion &localQuaternion);
    static pgd::Quaternion quaternion(const Body &body, const pgd::Quaternion &worldQuaternion);

    virtual std::string dumpToString();
    virtual std::string *createFromAttributes();
    virtual void saveToAttributes();
    virtual void appendToAttributes();

    Body *body() const;
    void setBody(Body *body);

private:

    Body *m_body = nullptr; // if nullptr then this is the World, otherwise a pre-existing body
    pgd::Vector3 m_position; // this is the position with respect to m_body (which can be World)
    pgd::Quaternion m_quaternion = {1, 0, 0, 0}; // this is the orientation with respect to m_body (which can be World)
};

}

#endif
