#ifndef THREEPP_shadow_vert_HPP
#define THREEPP_shadow_vert_HPP

namespace threepp::shaders::shaderlib {

const char* shadow_vert=R"(
#include <common>
#include <fog_pars_vertex>
#include <shadowmap_pars_vertex>

void main() {

	#include <begin_vertex>
	#include <project_vertex>
	#include <worldpos_vertex>

	#include <beginnormal_vertex>
	#include <morphnormal_vertex>
	#include <skinbase_vertex>
	#include <skinnormal_vertex>
	#include <defaultnormal_vertex>

	#include <shadowmap_vertex>
	#include <fog_vertex>

}

)";

}


#endif
