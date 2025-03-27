#ifndef THREEPP_clearcoat_pars_fragment_HPP
#define THREEPP_clearcoat_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* clearcoat_pars_fragment=R"(

#ifdef USE_CLEARCOATMAP

	uniform sampler2D clearcoatMap;

#endif

#ifdef USE_CLEARCOAT_ROUGHNESSMAP

	uniform sampler2D clearcoatRoughnessMap;

#endif

#ifdef USE_CLEARCOAT_NORMALMAP

	uniform sampler2D clearcoatNormalMap;
	uniform vec2 clearcoatNormalScale;

#endif

)";

}


#endif
