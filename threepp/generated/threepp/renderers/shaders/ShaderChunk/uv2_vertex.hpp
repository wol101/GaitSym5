#ifndef THREEPP_uv2_vertex_HPP
#define THREEPP_uv2_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* uv2_vertex=R"(
#if defined( USE_LIGHTMAP ) || defined( USE_AOMAP )

	vUv2 = ( uv2Transform * vec3( uv2, 1 ) ).xy;

#endif

)";

}


#endif
