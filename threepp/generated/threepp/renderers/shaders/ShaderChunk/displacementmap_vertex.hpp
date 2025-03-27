#ifndef THREEPP_displacementmap_vertex_HPP
#define THREEPP_displacementmap_vertex_HPP

namespace threepp::shaders::shaderchunk {

const char* displacementmap_vertex=R"(
#ifdef USE_DISPLACEMENTMAP

	transformed += normalize( objectNormal ) * ( texture2D( displacementMap, vUv ).x * displacementScale + displacementBias );

#endif

)";

}


#endif
