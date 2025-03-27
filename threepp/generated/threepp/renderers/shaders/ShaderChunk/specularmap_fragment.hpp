#ifndef THREEPP_specularmap_fragment_HPP
#define THREEPP_specularmap_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* specularmap_fragment=R"(
float specularStrength;

#ifdef USE_SPECULARMAP

	vec4 texelSpecular = texture2D( specularMap, vUv );
	specularStrength = texelSpecular.r;

#else

	specularStrength = 1.0;

#endif

)";

}


#endif
