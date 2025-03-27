#ifndef THREEPP_emissivemap_fragment_HPP
#define THREEPP_emissivemap_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* emissivemap_fragment=R"(
#ifdef USE_EMISSIVEMAP

	vec4 emissiveColor = texture2D( emissiveMap, vUv );

	emissiveColor.rgb = emissiveMapTexelToLinear( emissiveColor ).rgb;

	totalEmissiveRadiance *= emissiveColor.rgb;

#endif

)";

}


#endif
