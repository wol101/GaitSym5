#ifndef THREEPP_uv_vertex_HPP
#define THREEPP_uv_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* uv_vertex=R"(
#ifdef USE_UV

	vUv = ( uvTransform * vec3( uv, 1 ) ).xy;

#endif

)";

}


#endif
