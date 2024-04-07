#ifndef THREEPP_alphamap_fragment_HPP
#define THREEPP_alphamap_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* alphamap_fragment=R"(
#ifdef USE_ALPHAMAP

	diffuseColor.a *= texture2D( alphaMap, vUv ).g;

#endif

)";

}


#endif
