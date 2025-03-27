# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/ccd-src"
  "/home/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/ccd-build"
  "/mnt/d/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/ccd-subbuild/ccd-populate-prefix"
  "/mnt/d/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/ccd-subbuild/ccd-populate-prefix/tmp"
  "/mnt/d/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/ccd-subbuild/ccd-populate-prefix/src/ccd-populate-stamp"
  "/mnt/d/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/ccd-subbuild/ccd-populate-prefix/src"
  "/mnt/d/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/ccd-subbuild/ccd-populate-prefix/src/ccd-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/mnt/d/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/ccd-subbuild/ccd-populate-prefix/src/ccd-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/mnt/d/wis/Unix/git/GaitSym5/MuJoCo/mujoco-3.3.0/build/_deps/ccd-subbuild/ccd-populate-prefix/src/ccd-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
