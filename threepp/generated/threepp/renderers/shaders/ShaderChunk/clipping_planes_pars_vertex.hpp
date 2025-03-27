#ifndef THREEPP_clipping_planes_pars_vertex_HPP
#define THREEPP_clipping_planes_pars_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* clipping_planes_pars_vertex=R"(
#if NUM_CLIPPING_PLANES > 0

	varying vec3 vClipPosition;

#endif

)";

}


#endif
