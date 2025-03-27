#ifndef THREEPP_metalnessmap_pars_fragment_HPP
#define THREEPP_metalnessmap_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* metalnessmap_pars_fragment=R"(
#ifdef USE_METALNESSMAP

	uniform sampler2D metalnessMap;

#endif

)";

}


#endif
