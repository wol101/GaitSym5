#ifndef THREEPP_clipping_planes_pars_fragment_HPP
#define THREEPP_clipping_planes_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* clipping_planes_pars_fragment=R"(
#if NUM_CLIPPING_PLANES > 0

	varying vec3 vClipPosition;

	uniform vec4 clippingPlanes[ NUM_CLIPPING_PLANES ];

#endif

)";

}


#endif
