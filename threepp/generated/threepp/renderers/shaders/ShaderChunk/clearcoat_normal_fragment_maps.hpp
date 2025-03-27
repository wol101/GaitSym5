#ifndef THREEPP_clearcoat_normal_fragment_maps_HPP
#define THREEPP_clearcoat_normal_fragment_maps_HPP

namespace threepp::shaders::shaderchunk {

const char* clearcoat_normal_fragment_maps=R"(
#ifdef USE_CLEARCOAT_NORMALMAP

	vec3 clearcoatMapN = texture2D( clearcoatNormalMap, vUv ).xyz * 2.0 - 1.0;
	clearcoatMapN.xy *= clearcoatNormalScale;

	#ifdef USE_TANGENT

		clearcoatNormal = normalize( vTBN * clearcoatMapN );

	#else

		clearcoatNormal = perturbNormal2Arb( - vViewPosition, clearcoatNormal, clearcoatMapN, faceDirection );

	#endif

#endif

)";

}


#endif
