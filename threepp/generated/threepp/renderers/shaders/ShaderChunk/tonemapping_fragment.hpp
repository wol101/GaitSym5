#ifndef THREEPP_tonemapping_fragment_HPP
#define THREEPP_tonemapping_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* tonemapping_fragment=R"(
#if defined( TONE_MAPPING )

	gl_FragColor.rgb = toneMapping( gl_FragColor.rgb );

#endif

)";

}


#endif
