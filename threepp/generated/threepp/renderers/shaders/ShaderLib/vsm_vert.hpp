#ifndef THREEPP_vsm_vert_HPP
#define THREEPP_vsm_vert_HPP

namespace threepp::shaders::shaderlib {

const char* vsm_vert=R"(
void main() {

	gl_Position = vec4( position, 1.0 );

}

)";

}


#endif
