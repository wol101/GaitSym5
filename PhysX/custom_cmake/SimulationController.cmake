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
# Build SimulationController common
#

SET(PHYSX_SOURCE_DIR ${PHYSX_ROOT_DIR}/source)
SET(LL_SOURCE_DIR ${PHYSX_SOURCE_DIR}/simulationcontroller/src)

# Include here after the directories are defined so that the platform specific file can use the variables.
include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/${TARGET_BUILD_PLATFORM}/SimulationController.cmake)


SET(SIMULATIONCONTROLLER_BASE_DIR ${PHYSX_ROOT_DIR}/source/simulationcontroller)
SET(SIMULATIONCONTROLLER_HEADERS
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScActorCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScArticulationCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScArticulationJointCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScArticulationMimicJointCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScBodyCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScBroadphase.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScConstraintCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScDeformableSurfaceCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScDeformableVolumeCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScIterators.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScPhysics.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScRigidCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScScene.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScShapeCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScSqBoundsSync.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScStaticCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScParticleSystemCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScArticulationTendonCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScArticulationAttachmentCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/include/ScArticulationTendonJointCore.h
)
SOURCE_GROUP(include FILES ${SIMULATIONCONTROLLER_HEADERS})

SET(SIMULATIONCONTROLLER_SOURCE
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScActorCore.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScActorPair.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScActorSim.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScActorSim.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScGpuActorSim.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScGpuActorSim.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationCore.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationJointCore.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationTendonCore.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationJointSim.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationJointSim.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationSim.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationSim.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationTendonJointCore.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationTendonSim.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationTendonSim.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationMimicJointSim.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationMimicJointSim.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScBroadphase.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScDeformableSurfaceCore.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScDeformableSurfaceSim.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScDeformableSurfaceSim.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScDeformableVolumeCore.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScDeformableVolumeSim.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScDeformableVolumeSim.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScFiltering.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScFiltering.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScParticleSystemCore.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScParticleSystemSim.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScParticleSystemSim.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScParticleSystemShapeCore.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScParticleSystemShapeCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScBodyCore.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScBodySim.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScBodySim.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScConstraintBreakage.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScConstraintCore.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScConstraintInteraction.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScConstraintInteraction.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScConstraintSim.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScConstraintSim.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScContactReportBuffer.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScContactStream.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScElementInteractionMarker.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScElementInteractionMarker.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScElementSim.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScElementSim.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScElementSimInteraction.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScInteraction.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScInteraction.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScInteractionFlags.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScIterators.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScKinematics.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScNPhaseCore.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScNPhaseCore.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScObjectIDTracker.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScPhysics.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScRigidCore.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScRigidSim.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScRigidSim.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScScene.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScCCD.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScShapeCore.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScShapeInteraction.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScShapeInteraction.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScShapeSim.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScShapeSim.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScShapeSimBase.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScShapeSimBase.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScSimStateData.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScSimStats.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScSimStats.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScSimulationController.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScSimulationController.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScSqBoundsManager.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScSqBoundsManager.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScStaticCore.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScStaticSim.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScTriggerInteraction.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScTriggerInteraction.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScTriggerPairs.h
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScVisualize.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScSleep.cpp
    ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScPipeline.cpp
)
SOURCE_GROUP(src FILES ${SIMULATIONCONTROLLER_SOURCE})

set(PROJECT_SOURCES ${PROJECT_SOURCES}
    ${SIMULATIONCONTROLLER_HEADERS}
    ${SIMULATIONCONTROLLER_SOURCE}
)

set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
    ${SIMULATIONCONTROLLER_PLATFORM_INCLUDES}

    ${PHYSXFOUNDATION_INCLUDES}

    ${PHYSX_ROOT_DIR}/include

    ${PHYSX_SOURCE_DIR}/common/include
    ${PHYSX_SOURCE_DIR}/common/src

    ${PHYSX_SOURCE_DIR}/physxgpu/include

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

    ${PHYSX_SOURCE_DIR}/simulationcontroller/include
    ${PHYSX_SOURCE_DIR}/simulationcontroller/src

    ${PHYSX_SOURCE_DIR}/lowlevel/api/include
    ${PHYSX_SOURCE_DIR}/lowlevel/common/include
    ${PHYSX_SOURCE_DIR}/lowlevel/common/include/collision
    ${PHYSX_SOURCE_DIR}/lowlevel/common/include/pipeline
    ${PHYSX_SOURCE_DIR}/lowlevel/common/include/utils
    ${PHYSX_SOURCE_DIR}/lowlevel/software/include

    ${PHYSX_SOURCE_DIR}/lowleveldynamics/include
    ${PHYSX_SOURCE_DIR}/lowleveldynamics/shared

    ${PHYSX_SOURCE_DIR}/lowlevelaabb/include

)

