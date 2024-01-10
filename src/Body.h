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

class Body: public NamedObject
{
public:

    Body();

    virtual void LateInitialisation();

    SMART_ENUM(LimitTestResult, limitTestResultStrings, limitTestResultCount,
               WithinLimits, XPosError, YPosError, ZPosError, XVelError, YVelError, ZVelError, XAVelError, YAVelError, ZAVelError, NumericalError);

    SMART_ENUM(DragControl, dragControlStrings, dragControlCount,
               NoDrag, DragCoefficients, DragCylinderX, DragCylinderY, DragCylinderZ);

    void SetConstructionPosition(double x, double y, double z) { m_constructionPosition[0] = x; m_constructionPosition[1] = y; m_constructionPosition[2] = z; }
    pgd::Vector3 GetConstructionPosition() const { return m_constructionPosition; }
    void SetConstructionDensity(double constructionDensity) { m_constructionDensity = constructionDensity; }
    double GetConstructionDensity() const { return m_constructionDensity; }

    void SetPosition(double x, double y, double z);
    void SetQuaternion(double n, double x, double y, double z);
    std::string *SetPosition(const std::string &buf);
    std::string *SetQuaternion(const std::string &buf);
    void SetLinearVelocity(double x, double y, double z);
    void SetAngularVelocity(double x, double y, double z);
    std::string *SetLinearVelocity(const std::string &buf);
    std::string *SetAngularVelocity(const std::string &buf);
    void SetPositionDelta(double x, double y, double z);
    void SetQuaternionDelta(double n, double x, double y, double z);

    void SetMass(double mass);
    void SetMass(double mass, double ixx, double iyy, double izz, double ixy, double izx, double iyz);

    void SetPositionLowBound(double x, double y, double z) { m_positionLowBound[0] = x; m_positionLowBound[1] = y; m_positionLowBound[2] = z; }
    void SetPositionHighBound(double x, double y, double z) { m_positionHighBound[0] = x; m_positionHighBound[1] = y; m_positionHighBound[2] = z; }
    void SetLinearVelocityLowBound(double x, double y, double z) { m_linearVelocityLowBound[0] = x; m_linearVelocityLowBound[1] = y; m_linearVelocityLowBound[2] = z; }
    void SetLinearVelocityHighBound(double x, double y, double z) { m_linearVelocityHighBound[0] = x; m_linearVelocityHighBound[1] = y; m_linearVelocityHighBound[2] = z; }
    void SetAngularVelocityLowBound(double x, double y, double z) { m_angularVelocityLowBound[0] = x; m_angularVelocityLowBound[1] = y; m_angularVelocityLowBound[2] = z; }
    void SetAngularVelocityHighBound(double x, double y, double z) { m_angularVelocityHighBound[0] = x; m_angularVelocityHighBound[1] = y; m_angularVelocityHighBound[2] = z; }
    pgd::Vector3 GetPositionLowBound() const { return m_positionLowBound; }
    pgd::Vector3 GetPositionHighBound() const { return m_positionHighBound; }
    pgd::Vector3 GetLinearVelocityLowBound() const { return m_linearVelocityLowBound; }
    pgd::Vector3 GetLinearVelocityHighBound() const { return m_linearVelocityHighBound; }
    pgd::Vector3 GetAngularVelocityLowBound() const { return m_angularVelocityLowBound; }
    pgd::Vector3 GetAngularVelocityHighBound() const { return m_angularVelocityHighBound; }

    void SetLinearDamping(double linearDamping);
    void SetAngularDamping(double angularDamping);
    void SetLinearDampingThreshold(double linearDampingThreshold);
    void SetAngularDampingThreshold(double angularDampingThreshold);
    void SetMaxAngularSpeed(double maxAngularSpeed);

    void SetCylinderDragParameters(DragControl dragAxis, double dragFluidDensity, double dragCylinderMin, double dragCylinderMax, double dragCylinderRadius, double dragCylinderCoefficient);
    void SetDirectDragCoefficients(double linearDragCoefficientX, double linearDragCoefficientY, double linearDragCoefficientZ,
                                   double rotationalDragCoefficientX, double rotationalDragCoefficientY, double rotationalDragCoefficientZ);

    pgd::Vector3 GetPosition() const;
    pgd::Quaternion GetQuaternion() const;
    pgd::Vector3 GetLinearVelocity() const;
    pgd::Vector3 GetAngularVelocity() const;
    void GetPosition(pgd::Vector3 *pos) const;
    void GetQuaternion(pgd::Quaternion *quat) const;
    void GetRelativePosition(const Body *rel, pgd::Vector3 *pos) const;
    void GetRelativeQuaternion(const Body *rel, pgd::Quaternion *quat) const;
    void GetRelativeLinearVelocity(const Body *rel, pgd::Vector3 *vel) const;
    void GetRelativeAngularVelocity(const Body *rel, pgd::Vector3 *rVel) const;
    double GetMass() const;
    void GetMass(double *mass, double *ixx, double *iyy, double *izz, double *ixy, double *izx, double *iyz) const;
    double GetLinearKineticEnergy();
    void GetLinearKineticEnergy(pgd::Vector3 *ke);
    double GetRotationalKineticEnergy();
    double GetGravitationalPotentialEnergy();

    void SetInitialPosition(double x, double y, double z);
    void SetInitialQuaternion(double n, double x, double y, double z);
    pgd::Vector3 GetInitialPosition();
    pgd::Quaternion GetInitialQuaternion();

    LimitTestResult TestLimits();
//    int SanityCheck(Body *otherBody, Simulation::AxisType axis, const std::string &sanityCheckLeft, const std::string &sanityCheckRight);

    void EnterConstructionMode();
    void EnterRunMode();

    void ComputeDrag();

    // Utility
    static void ParallelAxis(double mass, const pgd::Matrix3x3 &inertialTensor, const pgd::Vector3 &translation, const double *quaternion, pgd::Matrix3x3 *newInertialTensor);
    static void ParallelAxis(double x, double y, double z, // transformation from centre of mass to new location (m)
                             double mass, // mass (kg)
                             double ixx, double iyy, double izz, double ixy, double izx, double iyz, // moments of inertia kgm2
                             double ang, // rotation angle (radians)
                             double ax, double ay, double az, // axis of rotation - must be unit length
                             double *ixxp, double *iyyp, double *izzp, double *ixyp, double *izxp, double *iyzp); // transformed moments of inertia about new coordinate system

    static double GetProjectedAngle(const pgd::Vector3 &planeNormal, const pgd::Vector3 &vector1, const pgd::Vector3 &vector2);

    void SetGraphicFile1(const std::string &graphicFile) { m_graphicFile1 = graphicFile; }
    std::string GetGraphicFile1() const { return m_graphicFile1; }
    void SetGraphicFile2(const std::string &graphicFile) { m_graphicFile2 = graphicFile; }
    std::string GetGraphicFile2() const { return m_graphicFile2; }
    void SetGraphicFile3(const std::string &graphicFile) { m_graphicFile3 = graphicFile; }
    std::string GetGraphicFile3() const { return m_graphicFile3; }

    virtual std::string dumpToString() override;
    virtual std::string *createFromAttributes() override;
    virtual void saveToAttributes() override;
    virtual void appendToAttributes() override;

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

#endif
