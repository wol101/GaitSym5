#ifndef THREEPP_worldpos_vertex_HPP
#define THREEPP_worldpos_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* worldpos_vertex=R"(
#if defined( USE_ENVMAP ) || defined( DISTANCE ) || defined ( USE_SHADOWMAP ) || defined ( USE_TRANSMISSION )

	vec4 worldPosition = vec4( transformed, 1.0 );

	#ifdef USE_INSTANCING

		worldPosition = instanceMatrix * worldPosition;

	#endif

	worldPosition = modelMatrix * worldPosition;

#endif

)";

}


#endif
