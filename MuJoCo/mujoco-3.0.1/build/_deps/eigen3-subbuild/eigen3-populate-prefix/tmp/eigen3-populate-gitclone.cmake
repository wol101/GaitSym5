# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

if(EXISTS "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp/eigen3-populate-gitclone-lastrun.txt" AND EXISTS "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp/eigen3-populate-gitinfo.txt" AND
  "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp/eigen3-populate-gitclone-lastrun.txt" IS_NEWER_THAN "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp/eigen3-populate-gitinfo.txt")
  message(STATUS
    "Avoiding repeated git clone, stamp file is up to date: "
    "'D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp/eigen3-populate-gitclone-lastrun.txt'"
  )
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/eigen3-src"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: 'D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/eigen3-src'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "C:/Program Files/Git/cmd/git.exe"
            clone --no-checkout --config "advice.detachedHead=false" "https://gitlab.com/libeigen/eigen.git" "eigen3-src"
    WORKING_DIRECTORY "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps"
    RESULT_VARIABLE error_code
  )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once: ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://gitlab.com/libeigen/eigen.git'")
endif()

execute_process(
  COMMAND "C:/Program Files/Git/cmd/git.exe"
          checkout "e8515f78ac098329ab9f8cab21c87caede090a3f" --
  WORKING_DIRECTORY "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/eigen3-src"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'e8515f78ac098329ab9f8cab21c87caede090a3f'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "C:/Program Files/Git/cmd/git.exe" 
            submodule update --recursive --init 
    WORKING_DIRECTORY "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/eigen3-src"
    RESULT_VARIABLE error_code
  )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: 'D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/eigen3-src'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp/eigen3-populate-gitinfo.txt" "D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp/eigen3-populate-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: 'D:/wis/Synchronised/Research/GaitSym/GaitSym5/MuJoCo/mujoco-3.0.1/build/_deps/eigen3-subbuild/eigen3-populate-prefix/src/eigen3-populate-stamp/eigen3-populate-gitclone-lastrun.txt'")
endif()
