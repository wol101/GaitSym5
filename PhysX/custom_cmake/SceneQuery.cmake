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
# Build SceneQuery common
#

SET(PHYSX_SOURCE_DIR ${PHYSX_ROOT_DIR}/source)
SET(LL_SOURCE_DIR ${PHYSX_SOURCE_DIR}/scenequery/src)

# Include here after the directories are defined so that the platform specific file can use the variables.
include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/${TARGET_BUILD_PLATFORM}/SceneQuery.cmake)


SET(SCENEQUERY_BASE_DIR ${PHYSX_ROOT_DIR}/source/scenequery)
SET(SCENEQUERY_HEADERS
    ${SCENEQUERY_BASE_DIR}/include/SqFactory.h
    ${SCENEQUERY_BASE_DIR}/include/SqPruner.h
    ${SCENEQUERY_BASE_DIR}/include/SqPrunerData.h
    ${SCENEQUERY_BASE_DIR}/include/SqManager.h
    ${SCENEQUERY_BASE_DIR}/include/SqQuery.h
    ${SCENEQUERY_BASE_DIR}/include/SqTypedef.h
)
SOURCE_GROUP(include FILES ${SCENEQUERY_HEADERS})

SET(SCENEQUERY_SOURCE
    ${SCENEQUERY_BASE_DIR}/src/SqFactory.cpp
    ${SCENEQUERY_BASE_DIR}/src/SqCompoundPruner.cpp
    ${SCENEQUERY_BASE_DIR}/src/SqCompoundPruner.h
    ${SCENEQUERY_BASE_DIR}/src/SqCompoundPruningPool.cpp
    ${SCENEQUERY_BASE_DIR}/src/SqCompoundPruningPool.h
    ${SCENEQUERY_BASE_DIR}/src/SqManager.cpp
    ${SCENEQUERY_BASE_DIR}/src/SqQuery.cpp
)
SOURCE_GROUP(src FILES ${SCENEQUERY_SOURCE})

set(PROJECT_SOURCES ${PROJECT_SOURCES}
    ${SCENEQUERY_HEADERS}
    ${SCENEQUERY_SOURCE}
)

set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
    ${SCENEQUERY_PLATFORM_INCLUDES}

    ${PHYSXFOUNDATION_INCLUDES}

    ${PHYSX_ROOT_DIR}/include

    ${PHYSX_SOURCE_DIR}/common/include
    ${PHYSX_SOURCE_DIR}/common/src

    ${PHYSX_SOURCE_DIR}/geomutils/include
    ${PHYSX_SOURCE_DIR}/geomutils/src
    ${PHYSX_SOURCE_DIR}/geomutils/src/contact
    ${PHYSX_SOURCE_DIR}/geomutils/src/common
    ${PHYSX_SOURCE_DIR}/geomutils/src/convex
    ${PHYSX_SOURCE_DIR}/geomutils/src/distance
    ${PHYSX_SOURCE_DIR}/geomutils/src/sweep
    ${PHYSX_SOURCE_DIR}/geomutils/src/gjk
    ${PHYSX_SOURCE_DIR}/geomutils/src/intersection
    ${PHYSX_SOURCE_DIR}/geomutils/src/mesh
    ${PHYSX_SOURCE_DIR}/geomutils/src/hf
    ${PHYSX_SOURCE_DIR}/geomutils/src/pcm
    ${PHYSX_SOURCE_DIR}/geomutils/src/ccd

    ${PHYSX_SOURCE_DIR}/scenequery/include

    ${PHYSX_SOURCE_DIR}/pvd/include
)

