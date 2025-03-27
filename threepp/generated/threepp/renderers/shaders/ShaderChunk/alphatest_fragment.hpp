#ifndef THREEPP_alphatest_fragment_HPP
#define THREEPP_alphatest_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* alphatest_fragment=R"(
#ifdef ALPHATEST

	if ( diffuseColor.a < ALPHATEST ) discard;

#endif

)";

}


#endif
