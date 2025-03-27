#ifndef THREEPP_specularmap_pars_fragment_HPP
#define THREEPP_specularmap_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* specularmap_pars_fragment=R"(
#ifdef USE_SPECULARMAP

	uniform sampler2D specularMap;

#endif

)";

}


#endif
