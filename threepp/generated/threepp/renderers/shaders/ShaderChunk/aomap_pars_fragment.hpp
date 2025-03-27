#ifndef THREEPP_aomap_pars_fragment_HPP
#define THREEPP_aomap_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* aomap_pars_fragment=R"(
#ifdef USE_AOMAP

	uniform sampler2D aoMap;
	uniform float aoMapIntensity;

#endif

)";

}


#endif
