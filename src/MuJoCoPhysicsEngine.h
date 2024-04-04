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

    // utility functions
    static void XMLInitiateTag(std::string *xmlString, const std::string &tag, const std::map<std::string, std::string> &attributes = std::map<std::string, std::string>(), bool terminate = false);
    static void XMLTerminateTag(std::string *xmlString, const std::string &tag);

private:
    struct TreeBody
    {
        Body *body = nullptr;
        TreeBody *parent = nullptr;
        Joint *jointToParent = nullptr;
        int bodyID = 0;
        std::vector<std::unique_ptr<TreeBody>> childList;
    };

    std::string *CreateTree();
    void InsertMJBodyIDs(TreeBody *treeBody);

    std::string *CreateBody(const TreeBody &treeBody);
    std::string *CreateJoint(const Joint *joint);
    std::string *CreateGeom(const Geom *geom);

    std::string *MoveBodies();

    // Returns a newly-allocated mjModel, loaded from the contents of xml.
    // On failure returns nullptr and populates the error array if present.
    mjModel* LoadModelFromString(const std::string &xml, char* error = nullptr, int error_size = 0, mjVFS* vfs = nullptr);

    // Simulation variables
    // MuJoCo model and data
    mjModel *m_mjModel = nullptr;
    mjData *m_mjData = nullptr;

    std::string m_mjXML;

    TreeBody m_rootTreeBody;
    std::multiset<Body *> m_jointLoopDetector;
    std::vector<Joint *> m_jointsLeftToInclude;
    std::vector<TreeBody *> m_bodiesLeftToInclude;
    std::vector<TreeBody *> m_flatTreeBodyList;

    int m_freeJointCount = 0;
};

#endif // MuJoCoPhysicsEngine_H
