#ifndef THREEPP_cube_frag_HPP
#define THREEPP_cube_frag_HPP

namespace threepp::shaders::shaderlib {

const char* cube_frag=R"(
#include <envmap_common_pars_fragment>
uniform float opacity;

varying vec3 vWorldDirection;

#include <cube_uv_reflection_fragment>

void main() {

	vec3 vReflect = vWorldDirection;
	#include <envmap_fragment>

	gl_FragColor = envColor;
	gl_FragColor.a *= opacity;

	#include <tonemapping_fragment>
	#include <encodings_fragment>

}

)";

}


#endif
