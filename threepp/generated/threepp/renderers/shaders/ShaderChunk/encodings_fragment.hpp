#ifndef THREEPP_encodings_fragment_HPP
#define THREEPP_encodings_fragment_HPP

namespace threepp::shaders::shaderchunk {

const char* encodings_fragment=R"(
gl_FragColor = linearToOutputTexel( gl_FragColor );

)";

}


#endif
