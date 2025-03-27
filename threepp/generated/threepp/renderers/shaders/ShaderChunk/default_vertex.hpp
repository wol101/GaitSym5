#ifndef THREEPP_default_vertex_HPP
#define THREEPP_default_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* default_vertex=R"(
void main() {
	gl_Position = projectionMatrix * modelViewMatrix * vec4( position, 1.0 );
}

)";

}


#endif
