#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "NamedObject.h"

namespace GaitSym
{

class Simulation;

class PhysicsEngine : public NamedObject
{
public:
    PhysicsEngine();
    virtual ~PhysicsEngine();
    PhysicsEngine(const PhysicsEngine&) = delete;
    PhysicsEngine(PhysicsEngine&&) = delete;
    PhysicsEngine& operator=(const PhysicsEngine&) = delete;
    PhysicsEngine& operator=(PhysicsEngine&&) = delete;

    virtual std::string *Initialise(Simulation *simulation);
    virtual std::string *Step() = 0;

    Simulation *simulation() const;
    void setSimulation(Simulation *newSimulation);

private:
    Simulation *m_simulation = nullptr;
};

}

#endif // PHYSICSENGINE_H
