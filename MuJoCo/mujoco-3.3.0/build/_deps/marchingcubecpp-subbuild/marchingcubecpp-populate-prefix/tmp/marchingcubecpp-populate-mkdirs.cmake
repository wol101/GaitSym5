# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/marchingcubecpp-src"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/marchingcubecpp-build"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/marchingcubecpp-subbuild/marchingcubecpp-populate-prefix"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/marchingcubecpp-subbuild/marchingcubecpp-populate-prefix/tmp"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/marchingcubecpp-subbuild/marchingcubecpp-populate-prefix/src/marchingcubecpp-populate-stamp"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/marchingcubecpp-subbuild/marchingcubecpp-populate-prefix/src"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/marchingcubecpp-subbuild/marchingcubecpp-populate-prefix/src/marchingcubecpp-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/marchingcubecpp-subbuild/marchingcubecpp-populate-prefix/src/marchingcubecpp-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/marchingcubecpp-subbuild/marchingcubecpp-populate-prefix/src/marchingcubecpp-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
