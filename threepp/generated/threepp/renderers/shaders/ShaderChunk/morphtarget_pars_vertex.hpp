#ifndef THREEPP_morphtarget_pars_vertex_HPP
#define THREEPP_morphtarget_pars_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* morphtarget_pars_vertex=R"(
#ifdef USE_MORPHTARGETS

	uniform float morphTargetBaseInfluence;

	#ifndef USE_MORPHNORMALS

		uniform float morphTargetInfluences[ 8 ];

	#else

		uniform float morphTargetInfluences[ 4 ];

	#endif

#endif

)";

}


#endif
