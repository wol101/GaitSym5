# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/sdflib-src"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/sdflib-build"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/sdflib-subbuild/sdflib-populate-prefix"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/sdflib-subbuild/sdflib-populate-prefix/tmp"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/sdflib-subbuild/sdflib-populate-prefix/src/sdflib-populate-stamp"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/sdflib-subbuild/sdflib-populate-prefix/src"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/sdflib-subbuild/sdflib-populate-prefix/src/sdflib-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/sdflib-subbuild/sdflib-populate-prefix/src/sdflib-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/sdflib-subbuild/sdflib-populate-prefix/src/sdflib-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
