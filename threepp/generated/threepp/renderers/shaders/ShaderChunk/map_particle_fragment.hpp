#ifndef THREEPP_map_particle_fragment_HPP
#define THREEPP_map_particle_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* map_particle_fragment=R"(
#if defined( USE_MAP ) || defined( USE_ALPHAMAP )

	vec2 uv = ( uvTransform * vec3( gl_PointCoord.x, 1.0 - gl_PointCoord.y, 1 ) ).xy;

#endif

#ifdef USE_MAP

	vec4 mapTexel = texture2D( map, uv );
	diffuseColor *= mapTexelToLinear( mapTexel );

#endif

#ifdef USE_ALPHAMAP

	diffuseColor.a *= texture2D( alphaMap, uv ).g;

#endif

)";

}


#endif
