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
# Build LowLevelAABB common
#

SET(PHYSX_SOURCE_DIR ${PHYSX_ROOT_DIR}/source)
SET(LLAABB_DIR ${PHYSX_SOURCE_DIR}/lowlevelaabb)

# Include here after the directories are defined so that the platform specific file can use the variables.
include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/${TARGET_BUILD_PLATFORM}/LowLevelAABB.cmake)


SET(LLAABB_HEADERS
    ${LLAABB_DIR}/include/BpAABBManager.h
    ${LLAABB_DIR}/include/BpVolumeData.h
    ${LLAABB_DIR}/include/BpAABBManagerBase.h
    ${LLAABB_DIR}/include/BpAABBManagerTasks.h
    ${LLAABB_DIR}/include/BpBroadPhase.h
    ${LLAABB_DIR}/include/BpBroadPhaseUpdate.h
    ${LLAABB_DIR}/include/BpFiltering.h
)
SOURCE_GROUP("include" FILES ${LLAABB_HEADERS})

SET(LLAABB_SOURCE
    ${LLAABB_DIR}/src/BpAABBManager.cpp
    ${LLAABB_DIR}/src/BpAABBManagerBase.cpp
    ${LLAABB_DIR}/src/BpBroadPhase.cpp
    ${LLAABB_DIR}/src/BpBroadPhaseUpdate.cpp
    ${LLAABB_DIR}/src/BpBroadPhaseABP.cpp
    ${LLAABB_DIR}/src/BpBroadPhaseABP.h
    ${LLAABB_DIR}/src/BpBroadPhaseMBP.cpp
    ${LLAABB_DIR}/src/BpBroadPhaseMBP.h
    ${LLAABB_DIR}/src/BpBroadPhaseMBPCommon.h
    ${LLAABB_DIR}/src/BpBroadPhaseSap.cpp
    ${LLAABB_DIR}/src/BpBroadPhaseSap.h
    ${LLAABB_DIR}/src/BpBroadPhaseSapAux.cpp
    ${LLAABB_DIR}/src/BpBroadPhaseSapAux.h
    ${LLAABB_DIR}/src/BpBroadPhaseShared.cpp
    ${LLAABB_DIR}/src/BpBroadPhaseShared.h
    ${LLAABB_DIR}/src/BpBroadPhaseIntegerAABB.h
    ${LLAABB_DIR}/src/BpFiltering.cpp
)
SOURCE_GROUP("src" FILES ${LLAABB_SOURCE})

set(PROJECT_SOURCES ${PROJECT_SOURCES}
	${LLAABB_HEADERS}
    ${LLAABB_SOURCE}
)

set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
	${LOWLEVELAABB_PLATFORM_INCLUDES}

    ${PHYSXFOUNDATION_INCLUDES}

    ${PHYSX_ROOT_DIR}/include

    ${PHYSX_SOURCE_DIR}/common/include
    ${PHYSX_SOURCE_DIR}/common/src

    ${PHYSX_SOURCE_DIR}/geomutils/include
    ${PHYSX_SOURCE_DIR}/geomutils/src

    ${PHYSX_SOURCE_DIR}/lowlevel/api/include
    ${PHYSX_SOURCE_DIR}/lowlevel/common/include/utils
    ${PHYSX_SOURCE_DIR}/lowlevel/common/include/pipeline
    ${PHYSX_SOURCE_DIR}/lowlevelaabb/include
    ${PHYSX_SOURCE_DIR}/lowlevelaabb/src
)

