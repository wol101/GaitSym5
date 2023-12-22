# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/qhull-src"
  "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/qhull-build"
  "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/qhull-subbuild/qhull-populate-prefix"
  "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/qhull-subbuild/qhull-populate-prefix/tmp"
  "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/qhull-subbuild/qhull-populate-prefix/src/qhull-populate-stamp"
  "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/qhull-subbuild/qhull-populate-prefix/src"
  "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/qhull-subbuild/qhull-populate-prefix/src/qhull-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/qhull-subbuild/qhull-populate-prefix/src/qhull-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/qhull-subbuild/qhull-populate-prefix/src/qhull-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()