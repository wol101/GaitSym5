#ifndef THREEPP_logdepthbuf_vertex_HPP
#define THREEPP_logdepthbuf_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* logdepthbuf_vertex=R"(
#ifdef USE_LOGDEPTHBUF

	#ifdef USE_LOGDEPTHBUF_EXT

		vFragDepth = 1.0 + gl_Position.w;
		vIsPerspective = float( isPerspectiveMatrix( projectionMatrix ) );

	#else

		if ( isPerspectiveMatrix( projectionMatrix ) ) {

			gl_Position.z = log2( max( EPSILON, gl_Position.w + 1.0 ) ) * logDepthBufFC - 1.0;

			gl_Position.z *= gl_Position.w;

		}

	#endif

#endif

)";

}


#endif
