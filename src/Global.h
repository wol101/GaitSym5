/*
 *  Global.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 11/11/2018.
 *  Copyright 2018 Bill Sellers. All rights reserved.
 *
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include "NamedObject.h"
#include "PGDMath.h"
#include "SmartEnum.h"

#include <string>
#include <vector>

using namespace std::string_literals;

namespace GaitSym
{

class Global: public NamedObject
{
public:
    Global();

    SMART_ENUM(StepType, stepTypeStrings, stepTypeCount, World, Quick);
    SMART_ENUM(FitnessType, fitnessTypeStrings, fitnessTypeCount, TargetSum, TargetMiniMax, TargetMaxiMin);
    SMART_ENUM(PhysicsEngine, physicsEngineTypeStrings, physicsEngineTypeCount, ODE, PhysX, MuJoCo);

    virtual std::string *createFromAttributes() override;
    virtual void saveToAttributes() override;
    virtual void appendToAttributes() override;

    FitnessType fitnessType() const;
    void setFitnessType(FitnessType fitnessType);

    StepType stepType() const;
    void setStepType(StepType stepType);

    PhysicsEngine physicsEngine() const;
    void setPhysicsEngine(PhysicsEngine newPhysicsEngine);

    bool allowConnectedCollisions() const;
    void setAllowConnectedCollisions(bool allowConnectedCollisions);

    bool allowInternalCollisions() const;
    void setAllowInternalCollisions(bool allowInternalCollisions);

    pgd::Vector3 gravity() const;
    void setGravity(const pgd::Vector3 &gravity);
    void setGravity(double gravityX, double gravityY, double gravityZ);

    double BMR() const;
    void setBMR(double BMR);

    double CFM() const;
    void setCFM(double CFM);

    double contactMaxCorrectingVel() const;
    void setContactMaxCorrectingVel(double ContactMaxCorrectingVel);

    double contactSurfaceLayer() const;
    void setContactSurfaceLayer(double ContactSurfaceLayer);

    double ERP() const;
    void setERP(double ERP);

    double mechanicalEnergyLimit() const;
    void setMechanicalEnergyLimit(double MechanicalEnergyLimit);

    double metabolicEnergyLimit() const;
    void setMetabolicEnergyLimit(double MetabolicEnergyLimit);

    double stepSize() const;
    void setStepSize(double StepSize);

    double timeLimit() const;
    void setTimeLimit(double TimeLimit);

    double springConstant() const;
    void setSpringConstant(double SpringConstant);

    double dampingConstant() const;
    void setDampingConstant(double DampingConstant);

    std::vector<std::string> *meshSearchPath();
    const std::vector<std::string> *constMeshSearchPath() const;
    void meshSearchPathAddToFront(const std::string &meshSearchPath);
    void meshSearchPathAddToBack(const std::string &meshSearchPath);
    bool meshSearchPathRemove(const std::string &meshSearchPath);

    double linearDamping() const;
    void setLinearDamping(double LinearDamping);

    double angularDamping() const;
    void setAngularDamping(double AngularDamping);

    static std::string percentEncode(const std::string &input, const std::string &encodeList);
    static std::string percentDecode(const std::string &input);

    int permittedNumericalErrors() const;
    void setPermittedNumericalErrors(int PermittedNumericalErrors);

    double numericalErrorsScore() const;
    void setNumericalErrorsScore(double NumericalErrorsScore);

    double defaultLength() const;
    void setDefaultLength(double newDefaultLength);

    double defaultSpeed() const;
    void setDefaultSpeed(double newDefaultSpeed);

private:
    FitnessType m_fitnessType = TargetSum;
    StepType m_stepType = World;
    PhysicsEngine m_physicsEngine = ODE;
    bool m_allowConnectedCollisions = false;
    bool m_allowInternalCollisions = false;
    int m_permittedNumericalErrors = 0;
    pgd::Vector3 m_gravity = {0, 0, -9.81};
    double m_bmr = 0;
    double m_cfm = 1e-10;
    double m_contactMaxCorrectingVel = 100;
    double m_contactSurfaceLayer = 0.001;
    double m_dampingConstant = 0;
    double m_erp = 0.2;
    double m_mechanicalEnergyLimit = 0;
    double m_metabolicEnergyLimit = 0;
    double m_springConstant = 0;
    double m_stepSize = 1e-4;
    double m_timeLimit = 10;
    double m_linearDamping = 0;
    double m_angularDamping = 0;
    double m_numericalErrorsScore = 0;
    std::vector<std::string> m_meshSearchPath = {"."s};
    double m_defaultLength = 1.0;
    double m_defaultSpeed = 10.0;
};

}

#endif // GLOBAL_H
