#ifndef THREEPP_logdepthbuf_pars_fragment_HPP
#define THREEPP_logdepthbuf_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* logdepthbuf_pars_fragment=R"(
#if defined( USE_LOGDEPTHBUF ) && defined( USE_LOGDEPTHBUF_EXT )

	uniform float logDepthBufFC;
	varying float vFragDepth;
	varying float vIsPerspective;

#endif

)";

}


#endif
