#ifndef THREEPP_beginnormal_vertex_HPP
#define THREEPP_beginnormal_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* beginnormal_vertex=R"(
vec3 objectNormal = vec3( normal );

#ifdef USE_TANGENT

	vec3 objectTangent = vec3( tangent.xyz );

#endif

)";

}


#endif
