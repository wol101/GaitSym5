## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##  * Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
##  * Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
##  * Neither the name of NVIDIA CORPORATION nor the names of its
##    contributors may be used to endorse or promote products derived
##    from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
## EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
## PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
## CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
## EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
## PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
## OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
## Copyright (c) 2008-2025 NVIDIA Corporation. All rights reserved.

#
# Build PhysXTask common
#

SET(PHYSX_SOURCE_DIR ${PHYSX_ROOT_DIR}/source)
SET(LL_SOURCE_DIR ${PHYSX_SOURCE_DIR}/task)

# Include here after the directories are defined so that the platform specific file can use the variables.
include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/${TARGET_BUILD_PLATFORM}/PhysXTask.cmake)

SET(PHYSXTASK_HEADERS
    ${PHYSX_ROOT_DIR}/include/task/PxCpuDispatcher.h
    ${PHYSX_ROOT_DIR}/include/task/PxTask.h
    ${PHYSX_ROOT_DIR}/include/task/PxTaskManager.h
)
SOURCE_GROUP(include FILES ${PHYSXTASK_HEADERS})

SET(PHYSXTASK_SOURCE
    ${LL_SOURCE_DIR}/src/TaskManager.cpp
)
SOURCE_GROUP(src FILES ${PHYSXTASK_SOURCE})

set(PROJECT_SOURCES ${PROJECT_SOURCES}
    ${PHYSXTASK_HEADERS}
    ${PHYSXTASK_SOURCE}
)

set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
    ${PHYSXTASK_PLATFORM_INCLUDES}

    ${PHYSXFOUNDATION_INCLUDES}

    ${PHYSX_ROOT_DIR}/include
    ${PHYSX_SOURCE_DIR}/cudamanager/include
)

