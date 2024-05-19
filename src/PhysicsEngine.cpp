#include "PhysicsEngine.h"

using namespace GaitSym;

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

