#ifndef THREEPP_map_pars_fragment_HPP
#define THREEPP_map_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* map_pars_fragment=R"(
#ifdef USE_MAP

	uniform sampler2D map;

#endif

)";

}


#endif
