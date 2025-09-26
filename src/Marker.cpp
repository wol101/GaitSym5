/*
 *  Marker.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 22/08/2009.
 *  Copyright 2009 Bill Sellers. All rights reserved.
 *
 */

#include "Marker.h"
#include "PGDMath.h"
#include "Simulation.h"
#include "Body.h"
#include "GSUtil.h"

#include "pystring.h"

#include <iostream>
#include <sstream>

using namespace std::string_literals;
namespace GaitSym {

Marker::Marker(Body *body)
{
    m_body = body;
}

void Marker::setPosition(double x, double y, double z)
{
    m_position.x = x; m_position.y = y; m_position.z = z;
}

void Marker::setPosition(const pgd::Vector3 &position)
{
    m_position = position;
}

void Marker::setQuaternion(double qs0, double qx1, double qy2, double qz3)
{
    m_quaternion.n = qs0;
    m_quaternion.x = qx1; m_quaternion.y = qy2; m_quaternion.z = qz3;
}

void Marker::setQuaternion(const pgd::Quaternion &quaternion)
{
    m_quaternion = quaternion;
}

// parses the position allowing a relative position specified by BODY ID
// x y z - body coordinates
// bodyName x y z - position relative to bodyName local coordinate system
// bodyName can be "World"
std::string *Marker::setPosition(const std::string &buf)
{
    std::vector<std::string> tokens;
    pystring::split(buf, tokens);
    if (tokens.size() < 3 || tokens.size() > 4)
    {
        setLastError("Marker ID=\""s + name() +"\" Position=\""s + buf + "\" needs 3 or 4 tokens"s);
        return lastErrorPtr();
    }
    if (tokens.size() == 3)
    {
        setPosition(GSUtil::toDouble(tokens[0]), GSUtil::toDouble(tokens[1]), GSUtil::toDouble(tokens[2]));
        return nullptr;
    }

    if (tokens[0] == "World"s)
    {
        if (m_body)
        {
//            pgd::Vector3 pos;
//            dBodyGetPosRelPoint(m_body->GetBodyID(), GSUtil::toDouble(tokens[1]), GSUtil::toDouble(tokens[2]), GSUtil::toDouble(tokens[3]), pos); // convert from world to body
//            SetPosition(pos[0], pos[1], pos[2]);
            pgd::Vector3 bodyRelativePosition = pgd::qVRotate(pgd::conjugate(m_body->quaternion()), pgd::Vector3(GSUtil::toDouble(tokens[1]), GSUtil::toDouble(tokens[2]), GSUtil::toDouble(tokens[3])) - pgd::Vector3(m_body->position()));
            setPosition(bodyRelativePosition.x, bodyRelativePosition.y, bodyRelativePosition.z);
        }
        else
        {
            setPosition(GSUtil::toDouble(tokens[1]), GSUtil::toDouble(tokens[2]), GSUtil::toDouble(tokens[3]));
        }
        return nullptr;
    }

    Body *theBody = simulation()->GetBody(tokens[0]);
    if (!theBody)
    {
        setLastError("Marker ID=\""s + name() +"\" Position=\""s + buf + "\" body not found"s);
        return lastErrorPtr();
    }
//    pgd::Vector3 result;
//    dBodyGetRelPointPos(theBody->GetBodyID(), GSUtil::toDouble(tokens[1]), GSUtil::toDouble(tokens[2]), GSUtil::toDouble(tokens[3]), result); // convert from body to world
    pgd::Vector3 bodyWorldPosition = pgd::qVRotate(theBody->quaternion(), pgd::Vector3(GSUtil::toDouble(tokens[1]), GSUtil::toDouble(tokens[2]), GSUtil::toDouble(tokens[3]))) + pgd::Vector3(theBody->position());
    if (m_body)
    {
//        pgd::Vector3 pos;
//        dBodyGetPosRelPoint(m_body->GetBodyID(), result[0], result[1], result[2], pos); // convert from world to body
//        SetPosition(pos[0], pos[1], pos[2]);
        pgd::Vector3 bodyRelativePosition = pgd::qVRotate(pgd::conjugate(m_body->quaternion()), bodyWorldPosition - pgd::Vector3(m_body->position()));
        setPosition(bodyRelativePosition.x, bodyRelativePosition.y, bodyRelativePosition.z);
    }
    else
    {
//        SetPosition(result[0], result[1], result[2]);
        setPosition(bodyWorldPosition.x, bodyWorldPosition.y, bodyWorldPosition.z);
    }
    return nullptr;
}

// parses the position allowing a relative position specified by BODY ID
// x y z - body coordinates
// bodyName x y z - position relative to bodyName local coordinate system
// bodyName can be "World"
std::string *Marker::setPosition(const std::string &body, double x, double y, double z)
{
//    pgd::Vector3 result;
    if (body == "World"s)
    {
        if (m_body)
        {
//            dBodyGetPosRelPoint(m_body->GetBodyID(), x, y, z, result); // convert from world to body
//            SetPosition(result[0], result[1], result[2]);
            pgd::Vector3 bodyRelativePosition = pgd::qVRotate(pgd::conjugate(m_body->quaternion()), pgd::Vector3(x, y, z) - pgd::Vector3(m_body->position()));
            setPosition(bodyRelativePosition.x, bodyRelativePosition.y, bodyRelativePosition.z);
        }
        else
        {
            setPosition(x, y, z);
        }
        return nullptr;
    }

    Body *theBody = simulation()->GetBody(body);
    if (!theBody)
    {
        setLastError("Marker ID=\""s + name() +"\" Position=\""s + body + "\" body not found"s);
        return lastErrorPtr();
    }
//    dBodyGetRelPointPos(theBody->GetBodyID(), x, y, z, result); // convert from body to world
    pgd::Vector3 bodyWorldPosition = pgd::qVRotate(theBody->quaternion(), pgd::Vector3(x, y, z)) + pgd::Vector3(theBody->position());
    if (m_body)
    {
//        pgd::Vector3 pos;
//        dBodyGetPosRelPoint(m_body->GetBodyID(), result[0], result[1], result[2], pos); // convert from world to body
//        SetPosition(pos[0], pos[1], pos[2]);
        pgd::Vector3 bodyRelativePosition = pgd::qVRotate(pgd::conjugate(m_body->quaternion()), bodyWorldPosition - pgd::Vector3(m_body->position()));
        setPosition(bodyRelativePosition.x, bodyRelativePosition.y, bodyRelativePosition.z);
    }
    else
    {
//        SetPosition(result[0], result[1], result[2]);
        setPosition(bodyWorldPosition.x, bodyWorldPosition.y, bodyWorldPosition.z);
    }
    return nullptr;
}

void Marker::setWorldPosition(double x, double y, double z)
{
    //    pgd::Vector3 result;
    if (m_body)
    {
        //        dBodyGetPosRelPoint(m_body->GetBodyID(), x, y, z, result); // convert from world to body
        //        SetPosition(result[0], result[1], result[2]);
        pgd::Vector3 bodyRelativePosition = pgd::qVRotate(pgd::conjugate(m_body->quaternion()), pgd::Vector3(x, y, z) - pgd::Vector3(m_body->position()));
        setPosition(bodyRelativePosition.x, bodyRelativePosition.y, bodyRelativePosition.z);
    }
    else
    {
        setPosition(x, y, z);
    }
}

void Marker::setWorldPosition(const pgd::Vector3 &pWorld)
{
    if (m_body)
    {
        pgd::Vector3 bodyRelativePosition = pgd::qVRotate(pgd::conjugate(m_body->quaternion()), pWorld - pgd::Vector3(m_body->position()));
        setPosition(bodyRelativePosition.x, bodyRelativePosition.y, bodyRelativePosition.z);
    }
    else
    {
        setPosition(pWorld);
    }
}

// parses the quaternion allowing a relative position specified by BODY ID
// note quaternion is (qs,qx,qy,qz)
// s x y z - body coordinates
// bodyName s x y z - position relative to bodyName local coordinate system
// bodyName can be "World"
std::string *Marker::setQuaternion(const std::string &buf)
{
    std::vector<std::string> tokens;
    pystring::split(buf, tokens);
    if (tokens.size() < 4 || tokens.size() > 5)
    {
        setLastError("Marker ID=\""s + name() +"\" Quaternion=\""s + buf + "\" needs 4 or 5 tokens"s);
        return lastErrorPtr();
    }
    if (tokens.size() == 4)
    {
        pgd::Quaternion q = GSUtil::toQuaternion(tokens, 0);
        setQuaternion(q.n, q.x, q.y, q.z);
        return nullptr;
    }

    if (tokens[0] == "World"s)
    {
        if (m_body)
        {
            pgd::Quaternion qBody = m_body->quaternion();
            pgd::Quaternion qWorld = GSUtil::toQuaternion(tokens, 1);
            pgd::Quaternion qLocal = ~qBody * qWorld;
            setQuaternion(qLocal.n, qLocal.x, qLocal.y, qLocal.z);
        }
        else
        {
            pgd::Quaternion q = GSUtil::toQuaternion(tokens, 1);
            setQuaternion(q.n, q.x, q.y, q.z);
        }
        return nullptr;
    }

    Body *theBody = simulation()->GetBody(tokens[0]);
    if (!theBody)
    {
        setLastError("Marker ID=\""s + name() +"\" Quaternion=\""s + buf + "\" body not found"s);
        return lastErrorPtr();
    }

    // first get world quaternion
    pgd::Quaternion qBody1 = theBody->quaternion();
    pgd::Quaternion qBody2 = GSUtil::toQuaternion(tokens, 1);
    pgd::Quaternion qWorld = qBody1 * qBody2;

    // then set the local quaternion
    if (m_body)
    {
        pgd::Quaternion qBody = m_body->quaternion();
        pgd::Quaternion qLocal = ~qBody * qWorld;
        setQuaternion(qLocal.n, qLocal.x, qLocal.y, qLocal.z);
    }
    else
    {
        setQuaternion(qWorld.n, qWorld.x, qWorld.y, qWorld.z);
    }
    return nullptr;
}

// parses the quaternion allowing a relative position specified by BODY ID
// note quaternion is (qs,qx,qy,qz)
// s x y z - body coordinates
// bodyName s x y z - position relative to bodyName local coordinate system
// bodyName can be "World"
std::string *Marker::setQuaternion(const std::string &body, double qs0, double qx1, double qy2, double qz3)
{
    if (body == "World"s)
    {
        if (m_body)
        {
            pgd::Quaternion qBody = m_body->quaternion();
            pgd::Quaternion qWorld(qs0, qx1, qy2, qz3);
            pgd::Quaternion qLocal = ~qBody * qWorld;
            setQuaternion(qLocal.n, qLocal.x, qLocal.y, qLocal.z);
        }
        else
        {
            setQuaternion(qs0, qx1, qy2, qz3);
        }
        return nullptr;
    }

    Body *theBody = simulation()->GetBody(body);
    if (!theBody)
    {
        setLastError("Marker ID=\""s + name() +"\" Quaternion=\""s + body + "\" body not found"s);
        return lastErrorPtr();
    }

    // first get world quaternion
    pgd::Quaternion qBody1 = theBody->quaternion();
    pgd::Quaternion qBody2(qs0, qx1, qy2, qz3);
    pgd::Quaternion qWorld = qBody1 * qBody2;

    // then set the local quaternion
    if (m_body)
    {
        pgd::Quaternion qBody = m_body->quaternion();
        pgd::Quaternion qLocal = ~qBody * qWorld;
        setQuaternion(qLocal.n, qLocal.x, qLocal.y, qLocal.z);
    }
    else
    {
        setQuaternion(qWorld.n, qWorld.x, qWorld.y, qWorld.z);
    }
    return nullptr;
}

void Marker::setWorldQuaternion(double qs0, double qx1, double qy2, double qz3)
{
    if (m_body)
    {
        pgd::Quaternion qBody = m_body->quaternion();
        pgd::Quaternion qWorld(qs0, qx1, qy2, qz3);
        pgd::Quaternion qLocal = ~qBody * qWorld;
        setQuaternion(qLocal.n, qLocal.x, qLocal.y, qLocal.z);
    }
    else
    {
        setQuaternion(qs0, qx1, qy2, qz3);
    }
}

void Marker::setWorldQuaternion(const pgd::Quaternion &qWorld)
{
    if (m_body)
    {
        pgd::Quaternion qBody = m_body->quaternion();
        pgd::Quaternion qLocal = ~qBody * qWorld;
        setQuaternion(qLocal.n, qLocal.x, qLocal.y, qLocal.z);
    }
    else
    {
        setQuaternion(qWorld);
    }
}

void Marker::offsetPosition(double x, double y, double z)
{
    m_position.x += x; m_position.y += y; m_position.z += z;
}

pgd::Vector3 Marker::position() const
{
    return m_position;
}

pgd::Quaternion Marker::quaternion() const
{
    return m_quaternion;
}

pgd::Vector3 Marker::axis(Marker::Axis axis) const
{
    switch (axis)
    {
    case Marker::Axis::X:
        return pgd::qVRotate(this->quaternion(), pgd::Vector3(1, 0, 0));
    case Marker::Axis::Y:
        return pgd::qVRotate(this->quaternion(), pgd::Vector3(0, 1, 0));
    case Marker::Axis::Z:
        return pgd::qVRotate(this->quaternion(), pgd::Vector3(0, 0, 1));
    }
    return pgd::Vector3(1, 0, 0); // just to stop warnings
}

void Marker::getBasis(pgd::Vector3 *x, pgd::Vector3 *y, pgd::Vector3 *z) const
{
    pgd::Matrix3x3 m(this->quaternion());
    x->x = m.e11;
    x->y = m.e21;
    x->z = m.e31;
    y->x = m.e12;
    y->y = m.e22;
    y->z = m.e32;
    z->x = m.e13;
    z->y = m.e23;
    z->z = m.e33;
}

pgd::Matrix3x3  Marker::basis() const
{
    pgd::Matrix3x3 m(this->quaternion());
    return m;
}

pgd::Vector3 Marker::worldPosition() const
{
    if (m_body)
    {
        // get the position in world coordinates
        //        pgd::Vector3 p;
        //        dBodyGetRelPointPos(m_body->GetBodyID(), m_position.x, m_position.y, m_position.z, p);
        //        return pgd::Vector3(p[0], p[1], p[2]);
        pgd::Vector3 bodyWorldPosition = pgd::qVRotate(m_body->quaternion(), m_position) + pgd::Vector3(m_body->position());
        return bodyWorldPosition;
    }
    else
    {
        return m_position;
    }
}

pgd::Vector3 Marker::constructionPosition() const
{
    if (m_body)
    {
        pgd::Vector3 bodyConstructionPosition = m_position + pgd::Vector3(m_body->constructionPosition()); // no rotation
        return bodyConstructionPosition;
    }
    else
    {
        return m_position;
    }
}

pgd::Vector3 Marker::worldPosition(const pgd::Vector3 &localCoordinates) const
{
    pgd::Vector3 worldDelta = pgd::qVRotate(worldQuaternion(), localCoordinates);
    return worldPosition() + worldDelta;
}

pgd::Vector3 Marker::position(const pgd::Vector3 &worldCoordinates) const
{
    pgd::Vector3 worldDelta = worldCoordinates - worldPosition();
    return pgd::qVRotate(~worldQuaternion(), worldDelta);
}

pgd::Vector3 Marker::worldVector(const pgd::Vector3 &localVector) const
{
    return pgd::qVRotate(worldQuaternion(), localVector);
}

pgd::Vector3 Marker::vector(const pgd::Vector3 &worldVector) const
{
    return pgd::qVRotate(~worldQuaternion(), worldVector);
}

pgd::Quaternion Marker::worldQuaternion(const pgd::Quaternion &localQuaternion) const
{
    return worldQuaternion() * localQuaternion;
}

pgd::Quaternion Marker::quaternion(const pgd::Quaternion &worldQuaternion) const
{
    return (~Marker::worldQuaternion()) * worldQuaternion;
}

pgd::Vector3 Marker::position(const Body &body, const pgd::Vector3 &worldCoordinates)
{
    pgd::Vector3 worldDelta = worldCoordinates - body.position();
    return pgd::qVRotate(~body.quaternion(), worldDelta);
}

pgd::Vector3 Marker::worldPosition(const Body &body, const pgd::Vector3 &localCoordinates)
{
    pgd::Vector3 worldDelta = pgd::qVRotate(body.quaternion(), localCoordinates);
    return body.position() + worldDelta;
}

pgd::Vector3 Marker::vector(const Body &body, const pgd::Vector3 &worldVector)
{
    return pgd::qVRotate(~body.quaternion(), worldVector);
}

pgd::Vector3 Marker::worldVector(const Body &body, const pgd::Vector3 &localVector)
{
    return pgd::qVRotate(body.quaternion(), localVector);
}

pgd::Quaternion Marker::worldQuaternion(const Body &body, const pgd::Quaternion &localQuaternion)
{
    return body.quaternion() * localQuaternion;
}

pgd::Quaternion Marker::quaternion(const Body &body, const pgd::Quaternion &worldQuaternion)
{
    return (~body.quaternion()) * worldQuaternion;
}

pgd::Vector3 Marker::worldLinearVelocity() const
{
    if (m_body)
    {
        // get the velocity in world coordinates
        pgd::Vector3 worldVelocity(m_body->linearVelocity());
        pgd::Vector3 av(m_body->angularVelocity());
        pgd::Quaternion q(m_body->quaternion());
        pgd::Vector3 p = pgd::qVRotate(q, m_position);
        pgd::Vector3 v1 = pgd::cross(av, p);
        worldVelocity += v1;
#ifdef CHECK_MARKER_MATH
        pgd::Vector3 p1;
        dBodyGetRelPointVel(m_body->GetBodyID(), m_position.x, m_position.y, m_position.z, p1);
        worldVelocity.Set(p1[0], p1[1], p1[2]);
#endif
        return worldVelocity;
    }
    else
    {
        return pgd::Vector3();
    }
}

pgd::Vector3 Marker::worldAngularVelocity() const
{
    pgd::Vector3 worldAngularVelocity;
    if (m_body) { worldAngularVelocity = m_body->angularVelocity(); }
    return worldAngularVelocity;
}

pgd::Vector3 Marker::linearVelocity() const
{
    pgd::Vector3 worldVelocity = worldLinearVelocity();
    return vector(worldVelocity);
}

pgd::Vector3 Marker::angularVelocity() const
{
    pgd::Vector3 worldAngularVelocity = Marker::worldAngularVelocity();
    return vector(worldAngularVelocity);
}

pgd::Quaternion Marker::worldQuaternion() const
{
    if (m_body)
    {
        pgd::Quaternion bodyQuaternion = m_body->quaternion();
        return bodyQuaternion * m_quaternion;
    }
    else
    {
        return m_quaternion;
    }
}

pgd::Vector3 Marker::worldAxis(Marker::Axis axis) const
{
    switch (axis)
    {
    case Marker::Axis::X:
        return pgd::qVRotate(this->worldQuaternion(), pgd::Vector3(1, 0, 0));
    case Marker::Axis::Y:
        return pgd::qVRotate(this->worldQuaternion(), pgd::Vector3(0, 1, 0));
    case Marker::Axis::Z:
        return pgd::qVRotate(this->worldQuaternion(), pgd::Vector3(0, 0, 1));
    }
    return pgd::Vector3(1, 0, 0); // just to stop warnings
}

void Marker::getWorldBasis(pgd::Vector3 *x, pgd::Vector3 *y, pgd::Vector3 *z) const
{
    pgd::Matrix3x3 m(this->worldQuaternion());
    x->x = m.e11;
    x->y = m.e21;
    x->z = m.e31;
    y->x = m.e12;
    y->y = m.e22;
    y->z = m.e32;
    z->x = m.e13;
    z->y = m.e23;
    z->z = m.e33;
}

pgd::Matrix3x3 Marker::worldBasis() const
{
    pgd::Matrix3x3 m(this->worldQuaternion());
    return m;
}

std::string Marker::dumpToString()
{
    std::stringstream ss;
    ss.precision(17);
    ss.setf(std::ios::scientific);
    if (firstDump())
    {
        setFirstDump(false);
        ss << "Time\tXP\tYP\tZP\tQW\tQX\tQY\tQZ\n";
    }
    pgd::Vector3 p = worldPosition();
    pgd::Quaternion q = worldQuaternion();

    ss << simulation()->GetTime() << "\t" << p.x << "\t" << p.y << "\t" << p.z <<
          "\t" << q.n << "\t" << q.x << "\t" << q.y << "\t" << q.z << "\n";
    return ss.str();
}

// this function initialises the data in the object based on the contents
// of an xml_node node. It uses information from the simulation as required
// to satisfy dependencies
// it returns nullptr on success and a pointer to lastError() on failure
std::string *Marker::createFromAttributes()
{
    if (NamedObject::createFromAttributes()) return lastErrorPtr();

    std::string buf;

    if (findAttribute("BodyID"s, &buf) == nullptr) return lastErrorPtr();
    if (buf != "World"s)
    {
        auto it = simulation()->GetBodyList()->find(buf);
        if (it == simulation()->GetBodyList()->end())
        {
            setLastError("Marker ID=\""s + name() +"\" BodyID=\""s + buf + "\" not found"s);
            return lastErrorPtr();
        }
        this->setBody(it->second.get());
    }
    else
    {
        m_body = nullptr;
    }

    // note quaternion is (qs,qx,qy,qz)
    if (findAttribute("Quaternion"s, &buf) == nullptr) return lastErrorPtr();
    this->setQuaternion(buf);

    // note position
    if (findAttribute("Position"s, &buf) == nullptr) return lastErrorPtr();
    this->setPosition(buf);

    if (m_body) setUpstreamObjects({m_body});
    return nullptr;
}

// this function copies the data in the object to an xml_node node that it creates internally.
// doc is used to allocate the memory so deletion should be automatic
void Marker::saveToAttributes()
{
    this->setTag("MARKER"s);
    this->clearAttributeMap();
    this->appendToAttributes();
}

// this function appends data to a pre-existing xml_node - often created by XMLSave
void Marker::appendToAttributes()
{
    NamedObject::appendToAttributes();
    std::string buf;
    std::string bodyName;
    if (this->body()) bodyName = this->body()->name();
    else bodyName = "World"s;
    setAttribute("BodyID"s, bodyName);
    setAttribute("Quaternion"s, bodyName + " "s + *GSUtil::toString(m_quaternion, &buf));
    setAttribute("Position"s, bodyName + " "s + *GSUtil::toString(m_position, &buf));
    setAttribute("WorldQuaternion"s, *GSUtil::toString(worldQuaternion(), &buf));
    setAttribute("WorldPosition"s, *GSUtil::toString(worldPosition(), &buf));
}

Body *Marker::body() const
{
    return m_body;
}

void Marker::setBody(Body *body)
{
    m_body = body;
}
} // namespace GaitSym


