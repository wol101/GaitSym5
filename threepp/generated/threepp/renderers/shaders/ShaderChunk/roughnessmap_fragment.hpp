#ifndef THREEPP_roughnessmap_fragment_HPP
#define THREEPP_roughnessmap_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* roughnessmap_fragment=R"(
float roughnessFactor = roughness;

#ifdef USE_ROUGHNESSMAP

	vec4 texelRoughness = texture2D( roughnessMap, vUv );

	// reads channel G, compatible with a combined OcclusionRoughnessMetallic (RGB) texture
	roughnessFactor *= texelRoughness.g;

#endif

)";

}


#endif
