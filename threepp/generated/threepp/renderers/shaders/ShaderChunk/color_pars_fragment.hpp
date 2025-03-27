#ifndef THREEPP_color_pars_fragment_HPP
#define THREEPP_color_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* color_pars_fragment=R"(
#if defined( USE_COLOR_ALPHA )

	varying vec4 vColor;

#elif defined( USE_COLOR )

	varying vec3 vColor;

#endif

)";

}


#endif
