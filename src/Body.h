/*
 *  Body.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 19/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

// this class is a wrapper for the ODE body

#ifndef Body_h
#define Body_h

#include "NamedObject.h"
#include "PGDMath.h"
#include "SmartEnum.h"

#include <limits>

namespace GaitSym
{

class Body: public NamedObject
{
public:

    Body();

    virtual void lateInitialisation();

    SMART_ENUM(LimitTestResult, limitTestResultStrings, limitTestResultCount,
               WithinLimits, XPosError, YPosError, ZPosError, XVelError, YVelError, ZVelError, XAVelError, YAVelError, ZAVelError, NumericalError);

    SMART_ENUM(DragControl, dragControlStrings, dragControlCount,
               NoDrag, DragCoefficients, DragCylinderX, DragCylinderY, DragCylinderZ);

    void setConstructionPosition(double x, double y, double z);
    void setConstructionPosition(const pgd::Vector3 &constructionPosition);
    pgd::Vector3 constructionPosition() const;
    void setConstructionDensity(double constructionDensity);
    double constructionDensity() const;

    void setPosition(double x, double y, double z);
    void setQuaternion(double n, double x, double y, double z);
    void setPosition(const pgd::Vector3 &position);
    void setQuaternion(const pgd::Quaternion &quaternion);
    std::string *setPosition(const std::string &buf);
    std::string *setQuaternion(const std::string &buf);
    void setLinearVelocity(double x, double y, double z);
    void setAngularVelocity(double x, double y, double z);
    void setLinearVelocity(const pgd::Vector3 &linearVelocity);
    void setAngularVelocity(const pgd::Vector3 &angularVelocity);
    std::string *setLinearVelocity(const std::string &buf);
    std::string *setAngularVelocity(const std::string &buf);
    void setPositionDelta(double x, double y, double z);
    void setQuaternionDelta(double n, double x, double y, double z);

    void setMass(double mass);
    void setMass(double mass, double ixx, double iyy, double izz, double ixy, double izx, double iyz);

    void setPositionLowBound(double x, double y, double z) { m_positionLowBound[0] = x; m_positionLowBound[1] = y; m_positionLowBound[2] = z; }
    void setPositionHighBound(double x, double y, double z) { m_positionHighBound[0] = x; m_positionHighBound[1] = y; m_positionHighBound[2] = z; }
    void setLinearVelocityLowBound(double x, double y, double z) { m_linearVelocityLowBound[0] = x; m_linearVelocityLowBound[1] = y; m_linearVelocityLowBound[2] = z; }
    void setLinearVelocityHighBound(double x, double y, double z) { m_linearVelocityHighBound[0] = x; m_linearVelocityHighBound[1] = y; m_linearVelocityHighBound[2] = z; }
    void setAngularVelocityLowBound(double x, double y, double z) { m_angularVelocityLowBound[0] = x; m_angularVelocityLowBound[1] = y; m_angularVelocityLowBound[2] = z; }
    void setAngularVelocityHighBound(double x, double y, double z) { m_angularVelocityHighBound[0] = x; m_angularVelocityHighBound[1] = y; m_angularVelocityHighBound[2] = z; }
    pgd::Vector3 positionLowBound() const { return m_positionLowBound; }
    pgd::Vector3 positionHighBound() const { return m_positionHighBound; }
    pgd::Vector3 linearVelocityLowBound() const { return m_linearVelocityLowBound; }
    pgd::Vector3 linearVelocityHighBound() const { return m_linearVelocityHighBound; }
    pgd::Vector3 angularVelocityLowBound() const { return m_angularVelocityLowBound; }
    pgd::Vector3 angularVelocityHighBound() const { return m_angularVelocityHighBound; }

    void setLinearDamping(double linearDamping);
    void setAngularDamping(double angularDamping);
    void setLinearDampingThreshold(double linearDampingThreshold);
    void setAngularDampingThreshold(double angularDampingThreshold);
    void setMaxAngularSpeed(double maxAngularSpeed);

    void setCylinderDragParameters(DragControl dragAxis, double dragFluidDensity, double dragCylinderMin, double dragCylinderMax, double dragCylinderRadius, double dragCylinderCoefficient);
    void setDirectDragCoefficients(double linearDragCoefficientX, double linearDragCoefficientY, double linearDragCoefficientZ, double rotationalDragCoefficientX, double rotationalDragCoefficientY, double rotationalDragCoefficientZ);

    pgd::Vector3 position() const;
    pgd::Quaternion quaternion() const;
    pgd::Vector3 linearVelocity() const;
    pgd::Vector3 angularVelocity() const;
    void getPosition(pgd::Vector3 *pos) const;
    void getQuaternion(pgd::Quaternion *quat) const;
    void getRelativePosition(const Body *rel, pgd::Vector3 *pos) const;
    void getRelativeQuaternion(const Body *rel, pgd::Quaternion *quat) const;
    void getRelativeLinearVelocity(const Body *rel, pgd::Vector3 *vel) const;
    void getRelativeAngularVelocity(const Body *rel, pgd::Vector3 *rVel) const;
    double mass() const;
    void getMass(double *mass, double *ixx, double *iyy, double *izz, double *ixy, double *izx, double *iyz) const;
    double linearKineticEnergy();
    void getLinearKineticEnergy(pgd::Vector3 *ke);
    double rotationalKineticEnergy();
    double gravitationalPotentialEnergy();

    void setInitialPosition(double x, double y, double z);
    void setInitialQuaternion(double n, double x, double y, double z);
    void setInitialPosition(const pgd::Vector3 &position);
    void setInitialQuaternion(const pgd::Quaternion &quaternion);
    pgd::Vector3 initialPosition();
    pgd::Quaternion initialQuaternion();

    LimitTestResult testLimits();
//    int sanityCheck(Body *otherBody, Simulation::AxisType axis, const std::string &sanityCheckLeft, const std::string &sanityCheckRight);

    void enterConstructionMode();
    void enterRunMode();

    void computeDrag();

    // Utility
    static void parallelAxis(double mass, const pgd::Matrix3x3 &inertialTensor, const pgd::Vector3 &translation, const double *quaternion, pgd::Matrix3x3 *newInertialTensor);
    static void parallelAxis(double x, double y, double z, // transformation from centre of mass to new location (m)
                             double mass, // mass (kg)
                             double ixx, double iyy, double izz, double ixy, double izx, double iyz, // moments of inertia kgm2
                             double ang, // rotation angle (radians)
                             double ax, double ay, double az, // axis of rotation - must be unit length
                             double *ixxp, double *iyyp, double *izzp, double *ixyp, double *izxp, double *iyzp); // transformed moments of inertia about new coordinate system

    static double getProjectedAngle(const pgd::Vector3 &planeNormal, const pgd::Vector3 &vector1, const pgd::Vector3 &vector2);

    void setGraphicFile1(const std::string &graphicFile) { m_graphicFile1 = graphicFile; }
    std::string graphicFile1() const { return m_graphicFile1; }
    void setGraphicFile2(const std::string &graphicFile) { m_graphicFile2 = graphicFile; }
    std::string graphicFile2() const { return m_graphicFile2; }
    void setGraphicFile3(const std::string &graphicFile) { m_graphicFile3 = graphicFile; }
    std::string graphicFile3() const { return m_graphicFile3; }

    virtual std::string dumpToString() override;
    virtual std::string *createFromAttributes() override;
    virtual void saveToAttributes() override;
    virtual void appendToAttributes() override;

    pgd::Vector3 dragForce() const;

    pgd::Vector3 dragTorque() const;

    DragControl dragControl() const;
    void setDragControl(const DragControl &newDragControl);

private:

    pgd::Vector3 m_constructionPosition = {0, 0, 0};
    pgd::Quaternion m_constructionQuaternion = {1, 0, 0, 0};
    double m_constructionDensity = 1000.0;

    pgd::Vector3 m_positionLowBound = {-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()};
    pgd::Vector3 m_positionHighBound = {std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    pgd::Vector3 m_linearVelocityLowBound = {-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()};
    pgd::Vector3 m_linearVelocityHighBound = {std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    pgd::Vector3 m_angularVelocityLowBound = {-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()};
    pgd::Vector3 m_angularVelocityHighBound = {std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};

    pgd::Vector3 m_initialPosition = {0, 0, 0};
    pgd::Quaternion m_initialQuaternion = {1, 0, 0, 0};

    pgd::Vector3 m_currentPosition = {0, 0, 0};
    pgd::Quaternion m_currentQuaternion = {1, 0, 0, 0};
    pgd::Vector3 m_currentLinearVelocity = {0, 0, 0};
    pgd::Vector3 m_currentAngularVelocity = {0, 0, 0};

    std::string m_graphicFile1;
    std::string m_graphicFile2;
    std::string m_graphicFile3;

    // values used for saving loading only
    double m_LinearDamping = -1;
    double m_AngularDamping = -1;
    double m_LinearDampingThreshold = -1;
    double m_AngularDampingThreshold = -1;
    double m_MaxAngularSpeed = -1;

    bool m_constructionMode = false;

    DragControl m_dragControl = DragControl::NoDrag;
    double m_dragCoefficients[6] = {0, 0, 0, 0, 0, 0};
    double m_dragFluidDensity = 0;
    double m_dragCylinderMin = 0;
    double m_dragCylinderLength = 0;
    double m_dragCylinderRadius = 0;
    double m_dragCylinderCoefficient = 0;
    pgd::Vector3 m_dragForce;
    pgd::Vector3 m_dragTorque;

    double m_mass = 0;
    pgd::Matrix3x3 m_inertia;
};

}

#endif
