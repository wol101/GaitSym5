#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine()
{
}

PhysicsEngine::~PhysicsEngine()
{
}

int PhysicsEngine::Initialise(Simulation *simulation)
{
    m_simulation = simulation;
    return 0;
}

Simulation *PhysicsEngine::simulation() const
{
    return m_simulation;
}

void PhysicsEngine::setSimulation(Simulation *newSimulation)
{
    m_simulation = newSimulation;
}

