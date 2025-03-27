#ifndef THREEPP_color_vertex_HPP
#define THREEPP_color_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* color_vertex=R"(
#if defined( USE_COLOR_ALPHA )

	vColor = vec4( 1.0 );

#elif defined( USE_COLOR ) || defined( USE_INSTANCING_COLOR )

	vColor = vec3( 1.0 );

#endif

#ifdef USE_COLOR

	vColor *= color;

#endif

#ifdef USE_INSTANCING_COLOR

	vColor.xyz *= instanceColor.xyz;

#endif

)";

}


#endif
