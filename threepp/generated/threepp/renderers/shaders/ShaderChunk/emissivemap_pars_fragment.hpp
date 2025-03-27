#ifndef THREEPP_emissivemap_pars_fragment_HPP
#define THREEPP_emissivemap_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* emissivemap_pars_fragment=R"(
#ifdef USE_EMISSIVEMAP

	uniform sampler2D emissiveMap;

#endif

)";

}


#endif
