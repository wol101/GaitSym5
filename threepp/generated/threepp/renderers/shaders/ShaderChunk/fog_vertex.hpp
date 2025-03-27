#ifndef THREEPP_fog_vertex_HPP
#define THREEPP_fog_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* fog_vertex=R"(
#ifdef USE_FOG

	fogDepth = - mvPosition.z;

#endif

)";

}


#endif
