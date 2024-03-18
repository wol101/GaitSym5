#ifndef MuJoCoPhysicsEngine_H
#define MuJoCoPhysicsEngine_H

#include "PhysicsEngine.h"

#include "mujoco/mujoco.h"

#include <memory>
#include <set>

class Body;
class Joint;
class Geom;

class MuJoCoPhysicsEngine : public PhysicsEngine
{
public:
    MuJoCoPhysicsEngine();
    virtual ~MuJoCoPhysicsEngine();

    virtual std::string *Initialise(Simulation *theSimulation);
    virtual std::string *Step();

 private:

    std::string *CreateBodies();
    std::string *CreateJoints();
    std::string *CreateGeoms();
    std::string *MoveBodies();

    // Simulation variables
    // MuJoCo model and data
    mjModel *m_mjModel = nullptr;
    mjData *m_mjData = nullptr;

    std::string m_mjXML;

    // Returns a newly-allocated mjModel, loaded from the contents of xml.
    // On failure returns nullptr and populates the error array if present.
    mjModel* LoadModelFromString(const std::string &xml, char* error = nullptr, int error_size = 0, mjVFS* vfs = nullptr);

    struct TreeBody
    {
        Body *body;
        TreeBody *parent;
        Joint *jointToParent;
        std::vector<std::unique_ptr<TreeBody>> childList;
    };
    TreeBody m_rootTreeBody;
    std::string *CreateTree();
    // std::string *AddNodeToTree(TreeBody *treeBody);
    std::multiset<Body *> m_jointLoopDetector;
    std::vector<Joint *> m_jointsLeftToInclude;
    std::vector<TreeBody *> m_bodiesLeftToInclude;

};

#endif // MuJoCoPhysicsEngine_H
