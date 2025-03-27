#ifndef THREEPP_lights_fragment_end_HPP
#define THREEPP_lights_fragment_end_HPP

namespace threepp::shaders::shaderchunk {

const char* lights_fragment_end=R"(
#if defined( RE_IndirectDiffuse )

	RE_IndirectDiffuse( irradiance, geometry, material, reflectedLight );

#endif

#if defined( RE_IndirectSpecular )

	RE_IndirectSpecular( radiance, iblIrradiance, clearcoatRadiance, geometry, material, reflectedLight );

#endif

)";

}


#endif
