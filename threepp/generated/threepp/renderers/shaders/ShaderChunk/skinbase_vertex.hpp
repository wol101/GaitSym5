#ifndef THREEPP_skinbase_vertex_HPP
#define THREEPP_skinbase_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* skinbase_vertex=R"(
#ifdef USE_SKINNING

	mat4 boneMatX = getBoneMatrix( skinIndex.x );
	mat4 boneMatY = getBoneMatrix( skinIndex.y );
	mat4 boneMatZ = getBoneMatrix( skinIndex.z );
	mat4 boneMatW = getBoneMatrix( skinIndex.w );

#endif

)";

}


#endif
