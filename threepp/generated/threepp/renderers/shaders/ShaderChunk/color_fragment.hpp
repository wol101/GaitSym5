#ifndef THREEPP_color_fragment_HPP
#define THREEPP_color_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* color_fragment=R"(
#if defined( USE_COLOR_ALPHA )

	diffuseColor *= vColor;

#elif defined( USE_COLOR )

	diffuseColor.rgb *= vColor;

#endif

)";

}


#endif
