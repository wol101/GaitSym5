#ifndef THREEPP_fog_pars_vertex_HPP
#define THREEPP_fog_pars_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* fog_pars_vertex=R"(
#ifdef USE_FOG

	varying float fogDepth;

#endif

)";

}


#endif
