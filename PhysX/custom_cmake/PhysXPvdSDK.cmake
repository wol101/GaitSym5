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
# Build PhysXPvdSDK common
#

SET(PHYSX_SOURCE_DIR ${PHYSX_ROOT_DIR}/source)
SET(LL_SOURCE_DIR ${PHYSX_SOURCE_DIR}/pvd)

# Include here after the directories are defined so that the platform specific file can use the variables.
include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/${TARGET_BUILD_PLATFORM}/PhysXPvdSDK.cmake)


SET(PHYSXPVDSDK_HEADERS
    ${PHYSX_ROOT_DIR}/include/pvd/PxPvd.h
    ${PHYSX_ROOT_DIR}/include/pvd/PxPvdTransport.h
)
SOURCE_GROUP(include FILES ${PHYSXPVDSDK_HEADERS})

SET(PHYSXPVDSDK_SOURCE
    ${LL_SOURCE_DIR}/src/PxProfileContextProvider.h
    ${LL_SOURCE_DIR}/src/PxProfileContextProviderImpl.h
    ${LL_SOURCE_DIR}/src/PxProfileDataBuffer.h
    ${LL_SOURCE_DIR}/src/PxProfileDataParsing.h
    ${LL_SOURCE_DIR}/src/PxProfileEventBuffer.h
    ${LL_SOURCE_DIR}/src/PxProfileEventBufferAtomic.h
    ${LL_SOURCE_DIR}/src/PxProfileEventBufferClient.h
    ${LL_SOURCE_DIR}/src/PxProfileEventBufferClientManager.h
    ${LL_SOURCE_DIR}/src/PxProfileEventId.h
    ${LL_SOURCE_DIR}/src/PxProfileEventImpl.cpp
    ${LL_SOURCE_DIR}/src/PxProfileEventMutex.h
    ${LL_SOURCE_DIR}/src/PxProfileEventNames.h
    ${LL_SOURCE_DIR}/src/PxProfileEvents.h
    ${LL_SOURCE_DIR}/src/PxProfileEventSender.h
    ${LL_SOURCE_DIR}/src/PxProfileEventSerialization.h
    ${LL_SOURCE_DIR}/src/PxProfileMemory.h
    ${LL_SOURCE_DIR}/src/PxProfileMemoryBuffer.h
    ${LL_SOURCE_DIR}/src/PxProfileMemoryEventBuffer.h
    ${LL_SOURCE_DIR}/src/PxProfileMemoryEvents.h
    ${LL_SOURCE_DIR}/src/PxProfileScopedEvent.h
    ${LL_SOURCE_DIR}/src/PxProfileScopedMutexLock.h
    ${LL_SOURCE_DIR}/src/PxPvdProfileZone.h
    ${LL_SOURCE_DIR}/src/PxProfileZoneImpl.h
    ${LL_SOURCE_DIR}/src/PxProfileZoneManager.h
    ${LL_SOURCE_DIR}/src/PxProfileZoneManagerImpl.h
    ${LL_SOURCE_DIR}/src/PxPvd.cpp
    ${LL_SOURCE_DIR}/src/PxPvdBits.h
    ${LL_SOURCE_DIR}/src/PxPvdByteStreams.h
    ${LL_SOURCE_DIR}/src/PxPvdCommStreamEvents.h
    ${LL_SOURCE_DIR}/src/PxPvdCommStreamEventSink.h
    ${LL_SOURCE_DIR}/src/PxPvdCommStreamTypes.h
    ${LL_SOURCE_DIR}/src/PxPvdDataStream.cpp
    ${LL_SOURCE_DIR}/src/PxPvdDefaultFileTransport.cpp
    ${LL_SOURCE_DIR}/src/PxPvdDefaultFileTransport.h
    ${LL_SOURCE_DIR}/src/PxPvdDefaultSocketTransport.cpp
    ${LL_SOURCE_DIR}/src/PxPvdDefaultSocketTransport.h
    ${LL_SOURCE_DIR}/src/PxPvdFoundation.h
    ${LL_SOURCE_DIR}/src/PxPvdImpl.cpp
    ${LL_SOURCE_DIR}/src/PxPvdImpl.h
    ${LL_SOURCE_DIR}/src/PxPvdInternalByteStreams.h
    ${LL_SOURCE_DIR}/src/PxPvdMarshalling.h
    ${LL_SOURCE_DIR}/src/PxPvdMemClient.cpp
    ${LL_SOURCE_DIR}/src/PxPvdMemClient.h
    ${LL_SOURCE_DIR}/src/PxPvdObjectModelInternalTypeDefs.h
    ${LL_SOURCE_DIR}/src/PxPvdObjectModelInternalTypes.h
    ${LL_SOURCE_DIR}/src/PxPvdObjectModelMetaData.cpp
    ${LL_SOURCE_DIR}/src/PxPvdObjectModelMetaData.h
    ${LL_SOURCE_DIR}/src/PxPvdObjectRegistrar.cpp
    ${LL_SOURCE_DIR}/src/PxPvdObjectRegistrar.h
    ${LL_SOURCE_DIR}/src/PxPvdProfileZoneClient.cpp
    ${LL_SOURCE_DIR}/src/PxPvdProfileZoneClient.h
    ${LL_SOURCE_DIR}/src/PxPvdUserRenderer.cpp
    ${LL_SOURCE_DIR}/src/PxPvdUserRenderImpl.h
    ${LL_SOURCE_DIR}/src/PxPvdUserRenderTypes.h
)
SOURCE_GROUP(src\\src FILES ${PHYSXPVDSDK_SOURCE})

SET(PHYSXPVDSDK_INTERNAL_HEADERS
    ${LL_SOURCE_DIR}/include/PsPvd.h
    ${LL_SOURCE_DIR}/include/PxProfileAllocatorWrapper.h
    ${LL_SOURCE_DIR}/include/PxPvdClient.h
    ${LL_SOURCE_DIR}/include/PxPvdDataStream.h
    ${LL_SOURCE_DIR}/include/PxPvdDataStreamHelpers.h
    ${LL_SOURCE_DIR}/include/PxPvdErrorCodes.h
    ${LL_SOURCE_DIR}/include/PxPvdObjectModelBaseTypes.h
    ${LL_SOURCE_DIR}/include/PxPvdUserRenderer.h
)
SOURCE_GROUP(src\\include FILES ${PHYSXPVDSDK_INTERNAL_HEADERS})

SET(PHYSXPVDSDK_FILEBUF_FILES
    ${PHYSX_SOURCE_DIR}/filebuf/include/PsFileBuffer.h
)
SOURCE_GROUP(filebuf\\include FILES ${PHYSXPVDSDK_FILEBUF_FILES})

set(PROJECT_SOURCES ${PROJECT_SOURCES}
    ${PHYSXPVDSDK_HEADERS}
    ${PHYSXPVDSDK_FILEBUF_FILES}

    ${PHYSXPVDSDK_INTERNAL_HEADERS}
    ${PHYSXPVDSDK_SOURCE}

    ${PHYSXPVDSDK_PLATFORM_FILES}
)

set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
    ${PHYSXPVDSDK_PLATFORM_INCLUDES}

    ${PHYSX_ROOT_DIR}/include
    ${LL_SOURCE_DIR}/include
    ${PHYSX_SOURCE_DIR}/filebuf/include

)

