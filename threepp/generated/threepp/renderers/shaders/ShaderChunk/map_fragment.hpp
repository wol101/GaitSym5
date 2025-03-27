#ifndef THREEPP_map_fragment_HPP
#define THREEPP_map_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* map_fragment=R"(
#ifdef USE_MAP

	vec4 texelColor = texture2D( map, vUv );

	texelColor = mapTexelToLinear( texelColor );
	diffuseColor *= texelColor;

#endif

)";

}


#endif
