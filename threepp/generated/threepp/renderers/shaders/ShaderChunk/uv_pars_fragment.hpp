#ifndef THREEPP_uv_pars_fragment_HPP
#define THREEPP_uv_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* uv_pars_fragment=R"(
#if ( defined( USE_UV ) && ! defined( UVS_VERTEX_ONLY ) )

	varying vec2 vUv;

#endif

)";

}


#endif
