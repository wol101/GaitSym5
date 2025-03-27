#ifndef THREEPP_envmap_pars_vertex_HPP
#define THREEPP_envmap_pars_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* envmap_pars_vertex=R"(
#ifdef USE_ENVMAP

	#if defined( USE_BUMPMAP ) || defined( USE_NORMALMAP ) ||defined( PHONG )

		#define ENV_WORLDPOS

	#endif

	#ifdef ENV_WORLDPOS
		
		varying vec3 vWorldPosition;

	#else

		varying vec3 vReflect;
		uniform float refractionRatio;

	#endif

#endif

)";

}


#endif
