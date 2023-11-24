#include <iostream>
#include <mujoco/mujoco.h>

int main(int argc, const char** argv)
{
    // check command-line arguments
    if (argc!=2) {
        std::printf(" USAGE:  basic modelfile\n");
        return 0;
    }

    // MuJoCo data structures
    mjModel* m = NULL;                  // MuJoCo model
    mjData* d = NULL;                   // MuJoCo data
    mjvCamera cam;                      // abstract camera
    mjvOption opt;                      // visualization options
    mjvScene scn;                       // abstract scene
    // mjrContext con;                     // custom GPU context

    // load and compile model
    char error[1000] = "Could not load binary model";
    if (std::strlen(argv[1])>4 && !std::strcmp(argv[1]+std::strlen(argv[1])-4, ".mjb")) {
        m = mj_loadModel(argv[1], 0);
    } else {
        m = mj_loadXML(argv[1], 0, error, 1000);
    }
    if (!m) {
        mju_error("Load model error: %s", error);
    }

    // make data
    d = mj_makeData(m);

    // initialize visualization data structures
    mjv_defaultCamera(&cam);
    mjv_defaultOption(&opt);
    mjv_defaultScene(&scn);
    // mjr_defaultContext(&con);

    // create scene and context
    mjv_makeScene(m, &scn, 2000);
    // mjr_makeContext(m, &con, mjFONTSCALE_150);

    for (int i = 0; i < 1000; i++) {
        // advance interactive simulation for 1/60 sec
        //  Assuming MuJoCo can simulate faster than real-time, which it usually can,
        //  this loop will finish on time for the next frame to be rendered at 60 fps.
        //  Otherwise add a cpu timer and exit this loop when it is time to render.
        mjtNum simstart = d->time;
        while (d->time - simstart < 1.0/60.0) {
            mj_step(m, d);
        }
    }

}
