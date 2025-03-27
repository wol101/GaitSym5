#ifndef THREEPP_uv_pars_vertex_HPP
#define THREEPP_uv_pars_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* uv_pars_vertex=R"(
#ifdef USE_UV

	#ifdef UVS_VERTEX_ONLY

		vec2 vUv;

	#else

		varying vec2 vUv;

	#endif

	uniform mat3 uvTransform;

#endif

)";

}


#endif
