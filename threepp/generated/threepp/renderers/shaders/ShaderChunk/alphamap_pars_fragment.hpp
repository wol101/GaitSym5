#ifndef THREEPP_alphamap_pars_fragment_HPP
#define THREEPP_alphamap_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* alphamap_pars_fragment=R"(
#ifdef USE_ALPHAMAP

	uniform sampler2D alphaMap;

#endif

)";

}


#endif
