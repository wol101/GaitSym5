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

