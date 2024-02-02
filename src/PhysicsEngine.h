#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "NamedObject.h"

class Simulation;

class PhysicsEngine : public NamedObject
{
public:
    PhysicsEngine();

    virtual int Initialise(Simulation *simulation) = 0;
    virtual int Step() = 0;

private:
    Simulation *m_simulation = nullptr;
};

#endif // PHYSICSENGINE_H
