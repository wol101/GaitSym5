#ifndef THREEPP_premultiplied_alpha_fragment_HPP
#define THREEPP_premultiplied_alpha_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* premultiplied_alpha_fragment=R"(
#ifdef PREMULTIPLIED_ALPHA

	// Get get normal blending with premultipled, use with CustomBlending, OneFactor, OneMinusSrcAlphaFactor, AddEquation.
	gl_FragColor.rgb *= gl_FragColor.a;

#endif

)";

}


#endif
