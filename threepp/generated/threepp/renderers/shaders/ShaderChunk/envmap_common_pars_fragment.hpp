#ifndef THREEPP_envmap_common_pars_fragment_HPP
#define THREEPP_envmap_common_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* envmap_common_pars_fragment=R"(
#ifdef USE_ENVMAP

	uniform float envMapIntensity;
	uniform float flipEnvMap;
	uniform int maxMipLevel;

	#ifdef ENVMAP_TYPE_CUBE
		uniform samplerCube envMap;
	#else
		uniform sampler2D envMap;
	#endif
	
#endif

)";

}


#endif
