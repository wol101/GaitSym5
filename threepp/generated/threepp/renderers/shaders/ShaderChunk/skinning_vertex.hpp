#ifndef THREEPP_skinning_vertex_HPP
#define THREEPP_skinning_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* skinning_vertex=R"(
#ifdef USE_SKINNING

	vec4 skinVertex = bindMatrix * vec4( transformed, 1.0 );

	vec4 skinned = vec4( 0.0 );
	skinned += boneMatX * skinVertex * skinWeight.x;
	skinned += boneMatY * skinVertex * skinWeight.y;
	skinned += boneMatZ * skinVertex * skinWeight.z;
	skinned += boneMatW * skinVertex * skinWeight.w;

	transformed = ( bindMatrixInverse * skinned ).xyz;

#endif

)";

}


#endif
