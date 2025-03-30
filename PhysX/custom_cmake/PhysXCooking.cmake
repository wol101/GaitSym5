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
# Build PhysXCooking common
#
SET(PHYSX_SOURCE_DIR ${PHYSX_ROOT_DIR}/source)
SET(LL_SOURCE_DIR ${PHYSX_SOURCE_DIR}/physxcooking/src)

SET(PHYSXCOOKING_LINK_FLAGS_DEBUG " ")
SET(PHYSXCOOKING_LINK_FLAGS_CHECKED " ")
SET(PHYSXCOOKING_LINK_FLAGS_PROFILE " ")
SET(PHYSXCOOKING_LINK_FLAGS_RELEASE " ")

# Include here after the directories are defined so that the platform specific file can use the variables.
include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/${TARGET_BUILD_PLATFORM}/PhysXCooking.cmake)


SET(PHYSX_COOKING_HEADERS
    ${PHYSX_ROOT_DIR}/include/cooking/PxBVH33MidphaseDesc.h
    ${PHYSX_ROOT_DIR}/include/cooking/PxBVH34MidphaseDesc.h
    ${PHYSX_ROOT_DIR}/include/cooking/Pxc.h
    ${PHYSX_ROOT_DIR}/include/cooking/PxConvexMeshDesc.h
    ${PHYSX_ROOT_DIR}/include/cooking/PxCooking.h
    ${PHYSX_ROOT_DIR}/include/cooking/PxCookingInternal.h
    ${PHYSX_ROOT_DIR}/include/cooking/PxMidphaseDesc.h
    ${PHYSX_ROOT_DIR}/include/cooking/PxTriangleMeshDesc.h
    ${PHYSX_ROOT_DIR}/include/cooking/PxTetrahedronMeshDesc.h
    ${PHYSX_ROOT_DIR}/include/cooking/PxBVHDesc.h
    ${PHYSX_ROOT_DIR}/include/cooking/PxTetrahedronMeshDesc.h
    ${PHYSX_ROOT_DIR}/include/cooking/PxSDFDesc.h
)
SOURCE_GROUP(include FILES ${PHYSX_COOKING_HEADERS})

SET(PHYSX_COOKING_SOURCE
    ${LL_SOURCE_DIR}/Cooking.cpp
    ${LL_SOURCE_DIR}/Cooking.h
)
SOURCE_GROUP(src FILES ${PHYSX_COOKING_SOURCE})

set(PROJECT_SOURCES ${PROJECT_SOURCES}
    ${PHYSX_COOKING_HEADERS}

    ${PHYSX_COOKING_SOURCE}

    ${PHYSXCOOKING_PLATFORM_SRC_FILES}
)

# Target specific compile options

set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
    ${PHYSXCOOKING_PLATFORM_INCLUDES}

    ${PHYSX_ROOT_DIR}/include

    ${PHYSX_SOURCE_DIR}/common/include
    ${PHYSX_SOURCE_DIR}/common/src

    ${PHYSX_SOURCE_DIR}/geomutils/include
    ${PHYSX_SOURCE_DIR}/geomutils/src
    ${PHYSX_SOURCE_DIR}/geomutils/src/mesh
)

