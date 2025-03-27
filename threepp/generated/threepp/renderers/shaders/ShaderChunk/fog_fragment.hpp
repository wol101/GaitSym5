#ifndef THREEPP_fog_fragment_HPP
#define THREEPP_fog_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* fog_fragment=R"(
#ifdef USE_FOG

	#ifdef FOG_EXP2

		float fogFactor = 1.0 - exp( - fogDensity * fogDensity * fogDepth * fogDepth );

	#else

		float fogFactor = smoothstep( fogNear, fogFar, fogDepth );

	#endif

	gl_FragColor.rgb = mix( gl_FragColor.rgb, fogColor, fogFactor );

#endif

)";

}


#endif
