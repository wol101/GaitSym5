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

    bool AllowConnectedCollisions() const;
    void setAllowConnectedCollisions(bool AllowConnectedCollisions);

    bool AllowInternalCollisions() const;
    void setAllowInternalCollisions(bool AllowInternalCollisions);

    pgd::Vector3 Gravity() const;
    void setGravity(const pgd::Vector3 &gravity);
    void setGravity(double gravityX, double gravityY, double gravityZ);

    double BMR() const;
    void setBMR(double BMR);

    double CFM() const;
    void setCFM(double CFM);

    double ContactMaxCorrectingVel() const;
    void setContactMaxCorrectingVel(double ContactMaxCorrectingVel);

    double ContactSurfaceLayer() const;
    void setContactSurfaceLayer(double ContactSurfaceLayer);

    double ERP() const;
    void setERP(double ERP);

    double MechanicalEnergyLimit() const;
    void setMechanicalEnergyLimit(double MechanicalEnergyLimit);

    double MetabolicEnergyLimit() const;
    void setMetabolicEnergyLimit(double MetabolicEnergyLimit);

    double StepSize() const;
    void setStepSize(double StepSize);

    double TimeLimit() const;
    void setTimeLimit(double TimeLimit);

    double SpringConstant() const;
    void setSpringConstant(double SpringConstant);

    double DampingConstant() const;
    void setDampingConstant(double DampingConstant);

    std::vector<std::string> *MeshSearchPath();
    const std::vector<std::string> *ConstMeshSearchPath() const;
    void MeshSearchPathAddToFront(const std::string &meshSearchPath);
    void MeshSearchPathAddToBack(const std::string &meshSearchPath);
    bool MeshSearchPathRemove(const std::string &meshSearchPath);

    double LinearDamping() const;
    void setLinearDamping(double LinearDamping);

    double AngularDamping() const;
    void setAngularDamping(double AngularDamping);

    static std::string percentEncode(const std::string &input, const std::string &encodeList);
    static std::string percentDecode(const std::string &input);

    int PermittedNumericalErrors() const;
    void setPermittedNumericalErrors(int PermittedNumericalErrors);

    double NumericalErrorsScore() const;
    void setNumericalErrorsScore(double NumericalErrorsScore);

private:
    FitnessType m_FitnessType = TargetSum;
    StepType m_StepType = World;
    PhysicsEngine m_PhysicsEngine = ODE;
    bool m_AllowConnectedCollisions = false;
    bool m_AllowInternalCollisions = false;
    int m_PermittedNumericalErrors = 0;
    pgd::Vector3 m_Gravity = {0, 0, -9.81};
    double m_BMR = 0;
    double m_CFM = 1e-10;
    double m_ContactMaxCorrectingVel = 100;
    double m_ContactSurfaceLayer = 0.001;
    double m_DampingConstant = 0;
    double m_ERP = 0.2;
    double m_MechanicalEnergyLimit = 0;
    double m_MetabolicEnergyLimit = 0;
    double m_SpringConstant = 0;
    double m_StepSize = 1e-4;
    double m_TimeLimit = 10;
    double m_LinearDamping = 0;
    double m_AngularDamping = 0;
    double m_NumericalErrorsScore = 0;
    std::vector<std::string> m_MeshSearchPath = {"."s};
};

}

#endif // GLOBAL_H
