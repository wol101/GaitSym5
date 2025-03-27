#ifndef THREEPP_project_vertex_HPP
#define THREEPP_project_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* project_vertex=R"(
vec4 mvPosition = vec4( transformed, 1.0 );

#ifdef USE_INSTANCING

	mvPosition = instanceMatrix * mvPosition;

#endif

mvPosition = modelViewMatrix * mvPosition;

gl_Position = projectionMatrix * mvPosition;

)";

}


#endif
