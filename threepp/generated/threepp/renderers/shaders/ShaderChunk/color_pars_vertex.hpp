#ifndef THREEPP_color_pars_vertex_HPP
#define THREEPP_color_pars_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* color_pars_vertex=R"(
#if defined( USE_COLOR_ALPHA )

	varying vec4 vColor;

#elif defined( USE_COLOR ) || defined( USE_INSTANCING_COLOR )

	varying vec3 vColor;

#endif

)";

}


#endif
