#ifndef THREEPP_cube_vert_HPP
#define THREEPP_cube_vert_HPP

namespace threepp::shaders::shaderlib {

const char* cube_vert=R"(
varying vec3 vWorldDirection;

#include <common>

void main() {

	vWorldDirection = transformDirection( position, modelMatrix );

	#include <begin_vertex>
	#include <project_vertex>

	gl_Position.z = gl_Position.w; // set z to camera.far

}

)";

}


#endif
