#ifndef THREEPP_lightmap_pars_fragment_HPP
#define THREEPP_lightmap_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* lightmap_pars_fragment=R"(
#ifdef USE_LIGHTMAP

	uniform sampler2D lightMap;
	uniform float lightMapIntensity;

#endif

)";

}


#endif
