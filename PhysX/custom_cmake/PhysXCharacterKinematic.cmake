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
# Build PhysXCharacterKinematic common
#

SET(PHYSX_SOURCE_DIR ${PHYSX_ROOT_DIR}/source)
SET(LL_SOURCE_DIR ${PHYSX_SOURCE_DIR}/physxcharacterkinematic/src)

# Include here after the directories are defined so that the platform specific file can use the variables.
include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/${TARGET_BUILD_PLATFORM}/PhysXCharacterKinematic.cmake)


SET(PHYSXCCT_HEADERS
    ${PHYSX_ROOT_DIR}/include/characterkinematic/PxBoxController.h
    ${PHYSX_ROOT_DIR}/include/characterkinematic/PxCapsuleController.h
    ${PHYSX_ROOT_DIR}/include/characterkinematic/PxController.h
    ${PHYSX_ROOT_DIR}/include/characterkinematic/PxControllerBehavior.h
    ${PHYSX_ROOT_DIR}/include/characterkinematic/PxControllerManager.h
    ${PHYSX_ROOT_DIR}/include/characterkinematic/PxControllerObstacles.h
    ${PHYSX_ROOT_DIR}/include/characterkinematic/PxExtended.h
)
SOURCE_GROUP(include FILES ${PHYSXCCT_HEADERS})

SET(PHYSXCCT_SOURCE
    ${LL_SOURCE_DIR}/CctBoxController.cpp
    ${LL_SOURCE_DIR}/CctCapsuleController.cpp
    ${LL_SOURCE_DIR}/CctCharacterController.cpp
    ${LL_SOURCE_DIR}/CctCharacterControllerCallbacks.cpp
    ${LL_SOURCE_DIR}/CctCharacterControllerManager.cpp
    ${LL_SOURCE_DIR}/CctController.cpp
    ${LL_SOURCE_DIR}/CctObstacleContext.cpp
    ${LL_SOURCE_DIR}/CctSweptBox.cpp
    ${LL_SOURCE_DIR}/CctSweptCapsule.cpp
    ${LL_SOURCE_DIR}/CctSweptVolume.cpp
    ${LL_SOURCE_DIR}/CctBoxController.h
    ${LL_SOURCE_DIR}/CctCapsuleController.h
    ${LL_SOURCE_DIR}/CctCharacterController.h
    ${LL_SOURCE_DIR}/CctCharacterControllerManager.h
    ${LL_SOURCE_DIR}/CctController.h
    ${LL_SOURCE_DIR}/CctInternalStructs.h
    ${LL_SOURCE_DIR}/CctObstacleContext.h
    ${LL_SOURCE_DIR}/CctSweptBox.h
    ${LL_SOURCE_DIR}/CctSweptCapsule.h
    ${LL_SOURCE_DIR}/CctSweptVolume.h
    ${LL_SOURCE_DIR}/CctUtils.h
)
SOURCE_GROUP(src FILES ${PHYSXCCT_SOURCE})

set(PROJECT_SOURCES ${PROJECT_SOURCES}
    ${PHYSXCCT_HEADERS}
    ${PHYSXCCT_SOURCE}
)

set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}

    ${PHYSXCHARACTERKINEMATICS_PLATFORM_INCLUDES}

    ${PHYSX_ROOT_DIR}/include

    ${PHYSX_SOURCE_DIR}/common/src

    ${PHYSX_SOURCE_DIR}/geomutils/include
)

