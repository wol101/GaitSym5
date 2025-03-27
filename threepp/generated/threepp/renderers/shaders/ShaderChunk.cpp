
#include "threepp/renderers/shaders/ShaderChunk.hpp"

// clang-format off

#include "ShaderChunk/alphamap_fragment.hpp"
#include "ShaderChunk/alphamap_pars_fragment.hpp"
#include "ShaderChunk/alphatest_fragment.hpp"
#include "ShaderChunk/aomap_fragment.hpp"
#include "ShaderChunk/aomap_pars_fragment.hpp"
#include "ShaderChunk/begin_vertex.hpp"
#include "ShaderChunk/beginnormal_vertex.hpp"
#include "ShaderChunk/bsdfs.hpp"
#include "ShaderChunk/bumpmap_pars_fragment.hpp"
#include "ShaderChunk/clearcoat_normal_fragment_begin.hpp"
#include "ShaderChunk/clearcoat_normal_fragment_maps.hpp"
#include "ShaderChunk/clearcoat_pars_fragment.hpp"
#include "ShaderChunk/clipping_planes_fragment.hpp"
#include "ShaderChunk/clipping_planes_pars_fragment.hpp"
#include "ShaderChunk/clipping_planes_pars_vertex.hpp"
#include "ShaderChunk/clipping_planes_vertex.hpp"
#include "ShaderChunk/color_fragment.hpp"
#include "ShaderChunk/color_pars_fragment.hpp"
#include "ShaderChunk/color_pars_vertex.hpp"
#include "ShaderChunk/color_vertex.hpp"
#include "ShaderChunk/common.hpp"
#include "ShaderChunk/cube_uv_reflection_fragment.hpp"
#include "ShaderChunk/default_fragment.hpp"
#include "ShaderChunk/default_vertex.hpp"
#include "ShaderChunk/defaultnormal_vertex.hpp"
#include "ShaderChunk/displacementmap_pars_vertex.hpp"
#include "ShaderChunk/displacementmap_vertex.hpp"
#include "ShaderChunk/dithering_fragment.hpp"
#include "ShaderChunk/dithering_pars_fragment.hpp"
#include "ShaderChunk/emissivemap_fragment.hpp"
#include "ShaderChunk/emissivemap_pars_fragment.hpp"
#include "ShaderChunk/encodings_fragment.hpp"
#include "ShaderChunk/encodings_pars_fragment.hpp"
#include "ShaderChunk/envmap_common_pars_fragment.hpp"
#include "ShaderChunk/envmap_fragment.hpp"
#include "ShaderChunk/envmap_pars_fragment.hpp"
#include "ShaderChunk/envmap_pars_vertex.hpp"
#include "ShaderChunk/envmap_physical_pars_fragment.hpp"
#include "ShaderChunk/envmap_vertex.hpp"
#include "ShaderChunk/fog_fragment.hpp"
#include "ShaderChunk/fog_pars_fragment.hpp"
#include "ShaderChunk/fog_pars_vertex.hpp"
#include "ShaderChunk/fog_vertex.hpp"
#include "ShaderChunk/gradientmap_pars_fragment.hpp"
#include "ShaderChunk/lightmap_fragment.hpp"
#include "ShaderChunk/lightmap_pars_fragment.hpp"
#include "ShaderChunk/lights_fragment_begin.hpp"
#include "ShaderChunk/lights_fragment_end.hpp"
#include "ShaderChunk/lights_fragment_maps.hpp"
#include "ShaderChunk/lights_lambert_vertex.hpp"
#include "ShaderChunk/lights_pars_begin.hpp"
#include "ShaderChunk/lights_phong_fragment.hpp"
#include "ShaderChunk/lights_phong_pars_fragment.hpp"
#include "ShaderChunk/lights_physical_fragment.hpp"
#include "ShaderChunk/lights_physical_pars_fragment.hpp"
#include "ShaderChunk/lights_toon_fragment.hpp"
#include "ShaderChunk/lights_toon_pars_fragment.hpp"
#include "ShaderChunk/logdepthbuf_fragment.hpp"
#include "ShaderChunk/logdepthbuf_pars_fragment.hpp"
#include "ShaderChunk/logdepthbuf_pars_vertex.hpp"
#include "ShaderChunk/logdepthbuf_vertex.hpp"
#include "ShaderChunk/map_fragment.hpp"
#include "ShaderChunk/map_pars_fragment.hpp"
#include "ShaderChunk/map_particle_fragment.hpp"
#include "ShaderChunk/map_particle_pars_fragment.hpp"
#include "ShaderChunk/metalnessmap_fragment.hpp"
#include "ShaderChunk/metalnessmap_pars_fragment.hpp"
#include "ShaderChunk/morphnormal_vertex.hpp"
#include "ShaderChunk/morphtarget_pars_vertex.hpp"
#include "ShaderChunk/morphtarget_vertex.hpp"
#include "ShaderChunk/normal_fragment_begin.hpp"
#include "ShaderChunk/normal_fragment_maps.hpp"
#include "ShaderChunk/normalmap_pars_fragment.hpp"
#include "ShaderChunk/packing.hpp"
#include "ShaderChunk/premultiplied_alpha_fragment.hpp"
#include "ShaderChunk/project_vertex.hpp"
#include "ShaderChunk/roughnessmap_fragment.hpp"
#include "ShaderChunk/roughnessmap_pars_fragment.hpp"
#include "ShaderChunk/shadowmap_pars_fragment.hpp"
#include "ShaderChunk/shadowmap_pars_vertex.hpp"
#include "ShaderChunk/shadowmap_vertex.hpp"
#include "ShaderChunk/shadowmask_pars_fragment.hpp"
#include "ShaderChunk/skinbase_vertex.hpp"
#include "ShaderChunk/skinning_pars_vertex.hpp"
#include "ShaderChunk/skinning_vertex.hpp"
#include "ShaderChunk/skinnormal_vertex.hpp"
#include "ShaderChunk/specularmap_fragment.hpp"
#include "ShaderChunk/specularmap_pars_fragment.hpp"
#include "ShaderChunk/tonemapping_fragment.hpp"
#include "ShaderChunk/tonemapping_pars_fragment.hpp"
#include "ShaderChunk/transmission_fragment.hpp"
#include "ShaderChunk/transmission_pars_fragment.hpp"
#include "ShaderChunk/uv2_pars_fragment.hpp"
#include "ShaderChunk/uv2_pars_vertex.hpp"
#include "ShaderChunk/uv2_vertex.hpp"
#include "ShaderChunk/uv_pars_fragment.hpp"
#include "ShaderChunk/uv_pars_vertex.hpp"
#include "ShaderChunk/uv_vertex.hpp"
#include "ShaderChunk/worldpos_vertex.hpp"
#include "ShaderLib/background_frag.hpp"
#include "ShaderLib/background_vert.hpp"
#include "ShaderLib/cube_frag.hpp"
#include "ShaderLib/cube_vert.hpp"
#include "ShaderLib/depth_frag.hpp"
#include "ShaderLib/depth_vert.hpp"
#include "ShaderLib/distanceRGBA_frag.hpp"
#include "ShaderLib/distanceRGBA_vert.hpp"
#include "ShaderLib/equirect_frag.hpp"
#include "ShaderLib/equirect_vert.hpp"
#include "ShaderLib/linedashed_frag.hpp"
#include "ShaderLib/linedashed_vert.hpp"
#include "ShaderLib/meshbasic_frag.hpp"
#include "ShaderLib/meshbasic_vert.hpp"
#include "ShaderLib/meshlambert_frag.hpp"
#include "ShaderLib/meshlambert_vert.hpp"
#include "ShaderLib/meshmatcap_frag.hpp"
#include "ShaderLib/meshmatcap_vert.hpp"
#include "ShaderLib/meshphong_frag.hpp"
#include "ShaderLib/meshphong_vert.hpp"
#include "ShaderLib/meshphysical_frag.hpp"
#include "ShaderLib/meshphysical_vert.hpp"
#include "ShaderLib/meshtoon_frag.hpp"
#include "ShaderLib/meshtoon_vert.hpp"
#include "ShaderLib/normal_frag.hpp"
#include "ShaderLib/normal_vert.hpp"
#include "ShaderLib/points_frag.hpp"
#include "ShaderLib/points_vert.hpp"
#include "ShaderLib/shadow_frag.hpp"
#include "ShaderLib/shadow_vert.hpp"
#include "ShaderLib/sprite_frag.hpp"
#include "ShaderLib/sprite_vert.hpp"
#include "ShaderLib/vsm_frag.hpp"
#include "ShaderLib/vsm_vert.hpp"

using namespace threepp::shaders;

ShaderChunk::ShaderChunk() {

	data_["alphamap_fragment"] = shaderchunk::alphamap_fragment;
	data_["alphamap_pars_fragment"] = shaderchunk::alphamap_pars_fragment;
	data_["alphatest_fragment"] = shaderchunk::alphatest_fragment;
	data_["aomap_fragment"] = shaderchunk::aomap_fragment;
	data_["aomap_pars_fragment"] = shaderchunk::aomap_pars_fragment;
	data_["begin_vertex"] = shaderchunk::begin_vertex;
	data_["beginnormal_vertex"] = shaderchunk::beginnormal_vertex;
	data_["bsdfs"] = shaderchunk::bsdfs;
	data_["bumpmap_pars_fragment"] = shaderchunk::bumpmap_pars_fragment;
	data_["clearcoat_normal_fragment_begin"] = shaderchunk::clearcoat_normal_fragment_begin;
	data_["clearcoat_normal_fragment_maps"] = shaderchunk::clearcoat_normal_fragment_maps;
	data_["clearcoat_pars_fragment"] = shaderchunk::clearcoat_pars_fragment;
	data_["clipping_planes_fragment"] = shaderchunk::clipping_planes_fragment;
	data_["clipping_planes_pars_fragment"] = shaderchunk::clipping_planes_pars_fragment;
	data_["clipping_planes_pars_vertex"] = shaderchunk::clipping_planes_pars_vertex;
	data_["clipping_planes_vertex"] = shaderchunk::clipping_planes_vertex;
	data_["color_fragment"] = shaderchunk::color_fragment;
	data_["color_pars_fragment"] = shaderchunk::color_pars_fragment;
	data_["color_pars_vertex"] = shaderchunk::color_pars_vertex;
	data_["color_vertex"] = shaderchunk::color_vertex;
	data_["common"] = shaderchunk::common;
	data_["cube_uv_reflection_fragment"] = shaderchunk::cube_uv_reflection_fragment;
	data_["default_fragment"] = shaderchunk::default_fragment;
	data_["default_vertex"] = shaderchunk::default_vertex;
	data_["defaultnormal_vertex"] = shaderchunk::defaultnormal_vertex;
	data_["displacementmap_pars_vertex"] = shaderchunk::displacementmap_pars_vertex;
	data_["displacementmap_vertex"] = shaderchunk::displacementmap_vertex;
	data_["dithering_fragment"] = shaderchunk::dithering_fragment;
	data_["dithering_pars_fragment"] = shaderchunk::dithering_pars_fragment;
	data_["emissivemap_fragment"] = shaderchunk::emissivemap_fragment;
	data_["emissivemap_pars_fragment"] = shaderchunk::emissivemap_pars_fragment;
	data_["encodings_fragment"] = shaderchunk::encodings_fragment;
	data_["encodings_pars_fragment"] = shaderchunk::encodings_pars_fragment;
	data_["envmap_common_pars_fragment"] = shaderchunk::envmap_common_pars_fragment;
	data_["envmap_fragment"] = shaderchunk::envmap_fragment;
	data_["envmap_pars_fragment"] = shaderchunk::envmap_pars_fragment;
	data_["envmap_pars_vertex"] = shaderchunk::envmap_pars_vertex;
	data_["envmap_physical_pars_fragment"] = shaderchunk::envmap_physical_pars_fragment;
	data_["envmap_vertex"] = shaderchunk::envmap_vertex;
	data_["fog_fragment"] = shaderchunk::fog_fragment;
	data_["fog_pars_fragment"] = shaderchunk::fog_pars_fragment;
	data_["fog_pars_vertex"] = shaderchunk::fog_pars_vertex;
	data_["fog_vertex"] = shaderchunk::fog_vertex;
	data_["gradientmap_pars_fragment"] = shaderchunk::gradientmap_pars_fragment;
	data_["lightmap_fragment"] = shaderchunk::lightmap_fragment;
	data_["lightmap_pars_fragment"] = shaderchunk::lightmap_pars_fragment;
	data_["lights_fragment_begin"] = shaderchunk::lights_fragment_begin;
	data_["lights_fragment_end"] = shaderchunk::lights_fragment_end;
	data_["lights_fragment_maps"] = shaderchunk::lights_fragment_maps;
	data_["lights_lambert_vertex"] = shaderchunk::lights_lambert_vertex;
	data_["lights_pars_begin"] = shaderchunk::lights_pars_begin;
	data_["lights_phong_fragment"] = shaderchunk::lights_phong_fragment;
	data_["lights_phong_pars_fragment"] = shaderchunk::lights_phong_pars_fragment;
	data_["lights_physical_fragment"] = shaderchunk::lights_physical_fragment;
	data_["lights_physical_pars_fragment"] = shaderchunk::lights_physical_pars_fragment;
	data_["lights_toon_fragment"] = shaderchunk::lights_toon_fragment;
	data_["lights_toon_pars_fragment"] = shaderchunk::lights_toon_pars_fragment;
	data_["logdepthbuf_fragment"] = shaderchunk::logdepthbuf_fragment;
	data_["logdepthbuf_pars_fragment"] = shaderchunk::logdepthbuf_pars_fragment;
	data_["logdepthbuf_pars_vertex"] = shaderchunk::logdepthbuf_pars_vertex;
	data_["logdepthbuf_vertex"] = shaderchunk::logdepthbuf_vertex;
	data_["map_fragment"] = shaderchunk::map_fragment;
	data_["map_pars_fragment"] = shaderchunk::map_pars_fragment;
	data_["map_particle_fragment"] = shaderchunk::map_particle_fragment;
	data_["map_particle_pars_fragment"] = shaderchunk::map_particle_pars_fragment;
	data_["metalnessmap_fragment"] = shaderchunk::metalnessmap_fragment;
	data_["metalnessmap_pars_fragment"] = shaderchunk::metalnessmap_pars_fragment;
	data_["morphnormal_vertex"] = shaderchunk::morphnormal_vertex;
	data_["morphtarget_pars_vertex"] = shaderchunk::morphtarget_pars_vertex;
	data_["morphtarget_vertex"] = shaderchunk::morphtarget_vertex;
	data_["normal_fragment_begin"] = shaderchunk::normal_fragment_begin;
	data_["normal_fragment_maps"] = shaderchunk::normal_fragment_maps;
	data_["normalmap_pars_fragment"] = shaderchunk::normalmap_pars_fragment;
	data_["packing"] = shaderchunk::packing;
	data_["premultiplied_alpha_fragment"] = shaderchunk::premultiplied_alpha_fragment;
	data_["project_vertex"] = shaderchunk::project_vertex;
	data_["roughnessmap_fragment"] = shaderchunk::roughnessmap_fragment;
	data_["roughnessmap_pars_fragment"] = shaderchunk::roughnessmap_pars_fragment;
	data_["shadowmap_pars_fragment"] = shaderchunk::shadowmap_pars_fragment;
	data_["shadowmap_pars_vertex"] = shaderchunk::shadowmap_pars_vertex;
	data_["shadowmap_vertex"] = shaderchunk::shadowmap_vertex;
	data_["shadowmask_pars_fragment"] = shaderchunk::shadowmask_pars_fragment;
	data_["skinbase_vertex"] = shaderchunk::skinbase_vertex;
	data_["skinning_pars_vertex"] = shaderchunk::skinning_pars_vertex;
	data_["skinning_vertex"] = shaderchunk::skinning_vertex;
	data_["skinnormal_vertex"] = shaderchunk::skinnormal_vertex;
	data_["specularmap_fragment"] = shaderchunk::specularmap_fragment;
	data_["specularmap_pars_fragment"] = shaderchunk::specularmap_pars_fragment;
	data_["tonemapping_fragment"] = shaderchunk::tonemapping_fragment;
	data_["tonemapping_pars_fragment"] = shaderchunk::tonemapping_pars_fragment;
	data_["transmission_fragment"] = shaderchunk::transmission_fragment;
	data_["transmission_pars_fragment"] = shaderchunk::transmission_pars_fragment;
	data_["uv2_pars_fragment"] = shaderchunk::uv2_pars_fragment;
	data_["uv2_pars_vertex"] = shaderchunk::uv2_pars_vertex;
	data_["uv2_vertex"] = shaderchunk::uv2_vertex;
	data_["uv_pars_fragment"] = shaderchunk::uv_pars_fragment;
	data_["uv_pars_vertex"] = shaderchunk::uv_pars_vertex;
	data_["uv_vertex"] = shaderchunk::uv_vertex;
	data_["worldpos_vertex"] = shaderchunk::worldpos_vertex;

	data_["background_frag"] = shaderlib::background_frag;
	data_["background_vert"] = shaderlib::background_vert;
	data_["cube_frag"] = shaderlib::cube_frag;
	data_["cube_vert"] = shaderlib::cube_vert;
	data_["depth_frag"] = shaderlib::depth_frag;
	data_["depth_vert"] = shaderlib::depth_vert;
	data_["distanceRGBA_frag"] = shaderlib::distanceRGBA_frag;
	data_["distanceRGBA_vert"] = shaderlib::distanceRGBA_vert;
	data_["equirect_frag"] = shaderlib::equirect_frag;
	data_["equirect_vert"] = shaderlib::equirect_vert;
	data_["linedashed_frag"] = shaderlib::linedashed_frag;
	data_["linedashed_vert"] = shaderlib::linedashed_vert;
	data_["meshbasic_frag"] = shaderlib::meshbasic_frag;
	data_["meshbasic_vert"] = shaderlib::meshbasic_vert;
	data_["meshlambert_frag"] = shaderlib::meshlambert_frag;
	data_["meshlambert_vert"] = shaderlib::meshlambert_vert;
	data_["meshmatcap_frag"] = shaderlib::meshmatcap_frag;
	data_["meshmatcap_vert"] = shaderlib::meshmatcap_vert;
	data_["meshphong_frag"] = shaderlib::meshphong_frag;
	data_["meshphong_vert"] = shaderlib::meshphong_vert;
	data_["meshphysical_frag"] = shaderlib::meshphysical_frag;
	data_["meshphysical_vert"] = shaderlib::meshphysical_vert;
	data_["meshtoon_frag"] = shaderlib::meshtoon_frag;
	data_["meshtoon_vert"] = shaderlib::meshtoon_vert;
	data_["normal_frag"] = shaderlib::normal_frag;
	data_["normal_vert"] = shaderlib::normal_vert;
	data_["points_frag"] = shaderlib::points_frag;
	data_["points_vert"] = shaderlib::points_vert;
	data_["shadow_frag"] = shaderlib::shadow_frag;
	data_["shadow_vert"] = shaderlib::shadow_vert;
	data_["sprite_frag"] = shaderlib::sprite_frag;
	data_["sprite_vert"] = shaderlib::sprite_vert;
	data_["vsm_frag"] = shaderlib::vsm_frag;
	data_["vsm_vert"] = shaderlib::vsm_vert;


}
// clang-format on
