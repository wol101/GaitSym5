#ifndef THREEPP_lightmap_fragment_HPP
#define THREEPP_lightmap_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* lightmap_fragment=R"(
#ifdef USE_LIGHTMAP

	vec4 lightMapTexel= texture2D( lightMap, vUv2 );
	reflectedLight.indirectDiffuse += PI * lightMapTexelToLinear( lightMapTexel ).rgb * lightMapIntensity; // factor of PI should not be present; included here to prevent breakage

#endif

)";

}


#endif
