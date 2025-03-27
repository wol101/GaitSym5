#ifndef THREEPP_dithering_fragment_HPP
#define THREEPP_dithering_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* dithering_fragment=R"(
#ifdef DITHERING

	gl_FragColor.rgb = dithering( gl_FragColor.rgb );

#endif

)";

}


#endif
