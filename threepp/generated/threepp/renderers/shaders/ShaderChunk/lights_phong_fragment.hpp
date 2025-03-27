#ifndef THREEPP_lights_phong_fragment_HPP
#define THREEPP_lights_phong_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* lights_phong_fragment=R"(
BlinnPhongMaterial material;
material.diffuseColor = diffuseColor.rgb;
material.specularColor = specular;
material.specularShininess = shininess;
material.specularStrength = specularStrength;

)";

}


#endif
