#ifndef THREEPP_envmap_pars_fragment_HPP
#define THREEPP_envmap_pars_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* envmap_pars_fragment=R"(
#ifdef USE_ENVMAP

	uniform float reflectivity;

	#if defined( USE_BUMPMAP ) || defined( USE_NORMALMAP ) || defined( PHONG )

		#define ENV_WORLDPOS

	#endif

	#ifdef ENV_WORLDPOS

		varying vec3 vWorldPosition;
		uniform float refractionRatio;
	#else
		varying vec3 vReflect;
	#endif

#endif

)";

}


#endif
