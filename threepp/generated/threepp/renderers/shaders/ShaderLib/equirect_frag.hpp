#ifndef THREEPP_equirect_frag_HPP
#define THREEPP_equirect_frag_HPP

namespace threepp::shaders::shaderlib {

const char* equirect_frag=R"(
uniform sampler2D tEquirect;

varying vec3 vWorldDirection;

#include <common>

void main() {

	vec3 direction = normalize( vWorldDirection );

	vec2 sampleUV = equirectUv( direction );

	vec4 texColor = texture2D( tEquirect, sampleUV );

	gl_FragColor = mapTexelToLinear( texColor );

	#include <tonemapping_fragment>
	#include <encodings_fragment>

}

)";

}


#endif
