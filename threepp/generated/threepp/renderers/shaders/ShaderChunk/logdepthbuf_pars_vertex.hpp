#ifndef THREEPP_logdepthbuf_pars_vertex_HPP
#define THREEPP_logdepthbuf_pars_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* logdepthbuf_pars_vertex=R"(
#ifdef USE_LOGDEPTHBUF

	#ifdef USE_LOGDEPTHBUF_EXT

		varying float vFragDepth;
		varying float vIsPerspective;

	#else

		uniform float logDepthBufFC;

	#endif

#endif

)";

}


#endif
