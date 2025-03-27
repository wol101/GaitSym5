#ifndef THREEPP_metalnessmap_fragment_HPP
#define THREEPP_metalnessmap_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* metalnessmap_fragment=R"(
float metalnessFactor = metalness;

#ifdef USE_METALNESSMAP

	vec4 texelMetalness = texture2D( metalnessMap, vUv );

	// reads channel B, compatible with a combined OcclusionRoughnessMetallic (RGB) texture
	metalnessFactor *= texelMetalness.b;

#endif

)";

}


#endif
