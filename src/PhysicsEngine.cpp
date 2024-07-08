/*
 *  PhysicsEngine.cpp
 *  GaitSym5
 *
 *  Created by Bill Sellers on 20/03/2024.
 *  Copyright 2024 Bill Sellers. All rights reserved.
 *
 */

#include "PhysicsEngine.h"

namespace GaitSym {

PhysicsEngine::PhysicsEngine()
{
}

PhysicsEngine::~PhysicsEngine()
{
}

std::string *PhysicsEngine::Initialise(Simulation *simulation)
{
    m_simulation = simulation;
    return nullptr;
}

Simulation *PhysicsEngine::simulation() const
{
    return m_simulation;
}

void PhysicsEngine::setSimulation(Simulation *newSimulation)
{
    m_simulation = newSimulation;
}

} // namespace GaitSym


