#ifndef THREEPP_displacementmap_pars_vertex_HPP
#define THREEPP_displacementmap_pars_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* displacementmap_pars_vertex=R"(
#ifdef USE_DISPLACEMENTMAP

	uniform sampler2D displacementMap;
	uniform float displacementScale;
	uniform float displacementBias;

#endif

)";

}


#endif
