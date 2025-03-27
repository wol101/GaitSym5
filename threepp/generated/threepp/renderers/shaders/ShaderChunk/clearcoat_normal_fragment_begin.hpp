#ifndef THREEPP_clearcoat_normal_fragment_begin_HPP
#define THREEPP_clearcoat_normal_fragment_begin_HPP

namespace threepp::shaders::shaderchunk {

const char* clearcoat_normal_fragment_begin=R"(
#ifdef CLEARCOAT

	vec3 clearcoatNormal = geometryNormal;

#endif

)";

}


#endif
