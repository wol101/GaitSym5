#ifndef THREEPP_shadow_frag_HPP
#define THREEPP_shadow_frag_HPP

namespace threepp::shaders::shaderlib {

const char* shadow_frag=R"(
uniform vec3 color;
uniform float opacity;

#include <common>
#include <packing>
#include <fog_pars_fragment>
#include <bsdfs>
#include <lights_pars_begin>
#include <shadowmap_pars_fragment>
#include <shadowmask_pars_fragment>

void main() {

	gl_FragColor = vec4( color, opacity * ( 1.0 - getShadowMask() ) );

	#include <tonemapping_fragment>
	#include <encodings_fragment>
	#include <fog_fragment>

}

)";

}


#endif
