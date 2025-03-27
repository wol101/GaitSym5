#ifndef THREEPP_uv2_pars_fragment_HPP
#define THREEPP_uv2_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* uv2_pars_fragment=R"(
#if defined( USE_LIGHTMAP ) || defined( USE_AOMAP )

	varying vec2 vUv2;

#endif

)";

}


#endif
