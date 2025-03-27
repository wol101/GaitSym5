#ifndef THREEPP_roughnessmap_pars_fragment_HPP
#define THREEPP_roughnessmap_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* roughnessmap_pars_fragment=R"(
#ifdef USE_ROUGHNESSMAP

	uniform sampler2D roughnessMap;

#endif

)";

}


#endif
