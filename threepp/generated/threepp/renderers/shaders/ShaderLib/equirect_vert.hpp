#ifndef THREEPP_equirect_vert_HPP
#define THREEPP_equirect_vert_HPP

namespace threepp::shaders::shaderlib {

const char* equirect_vert=R"(
varying vec3 vWorldDirection;

#include <common>

void main() {

	vWorldDirection = transformDirection( position, modelMatrix );

	#include <begin_vertex>
	#include <project_vertex>

}

)";

}


#endif
