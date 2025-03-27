#ifndef THREEPP_fog_pars_fragment_HPP
#define THREEPP_fog_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* fog_pars_fragment=R"(
#ifdef USE_FOG

	uniform vec3 fogColor;
	varying float fogDepth;

	#ifdef FOG_EXP2

		uniform float fogDensity;

	#else

		uniform float fogNear;
		uniform float fogFar;

	#endif

#endif

)";

}


#endif
