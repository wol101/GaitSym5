#ifndef THREEPP_background_frag_HPP
#define THREEPP_background_frag_HPP

namespace threepp::shaders::shaderlib {

const char* background_frag=R"(
uniform sampler2D t2D;

varying vec2 vUv;

void main() {

	vec4 texColor = texture2D( t2D, vUv );

	gl_FragColor = mapTexelToLinear( texColor );

	#include <tonemapping_fragment>
	#include <encodings_fragment>

}

)";

}


#endif
