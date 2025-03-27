#ifndef THREEPP_background_vert_HPP
#define THREEPP_background_vert_HPP

namespace threepp::shaders::shaderlib {

const char* background_vert=R"(
varying vec2 vUv;
uniform mat3 uvTransform;

void main() {

	vUv = ( uvTransform * vec3( uv, 1 ) ).xy;

	gl_Position = vec4( position.xy, 1.0, 1.0 );

}

)";

}


#endif
