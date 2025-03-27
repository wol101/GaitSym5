#ifndef THREEPP_default_fragment_HPP
#define THREEPP_default_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* default_fragment=R"(
void main() {
	gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
}

)";

}


#endif
