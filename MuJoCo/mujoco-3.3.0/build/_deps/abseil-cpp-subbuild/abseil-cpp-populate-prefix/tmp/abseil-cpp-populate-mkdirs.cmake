# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/abseil-cpp-src"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/abseil-cpp-build"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/abseil-cpp-subbuild/abseil-cpp-populate-prefix"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/abseil-cpp-subbuild/abseil-cpp-populate-prefix/tmp"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/abseil-cpp-subbuild/abseil-cpp-populate-prefix/src/abseil-cpp-populate-stamp"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/abseil-cpp-subbuild/abseil-cpp-populate-prefix/src"
  "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/abseil-cpp-subbuild/abseil-cpp-populate-prefix/src/abseil-cpp-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/abseil-cpp-subbuild/abseil-cpp-populate-prefix/src/abseil-cpp-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/abseil-cpp-subbuild/abseil-cpp-populate-prefix/src/abseil-cpp-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
