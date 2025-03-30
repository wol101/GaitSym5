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
# Build PhysXCommon
#

SET(PHYSXCOMMON_WINDOWS_HEADERS
    ${PHYSX_SOURCE_DIR}/common/include/windows/CmWindowsLoadLibrary.h
    ${PHYSX_SOURCE_DIR}/common/include/windows/CmWindowsModuleUpdateLoader.h
)
SOURCE_GROUP(common\\include\\windows FILES ${PHYSXCOMMON_WINDOWS_HEADERS})

SET(PHYSXCOMMON_WINDOWS_SOURCE
    ${COMMON_SRC_DIR}/windows/CmWindowsModuleUpdateLoader.cpp
    ${COMMON_SRC_DIR}/windows/CmWindowsDelayLoadHook.cpp
)
SOURCE_GROUP(common\\src\\windows FILES ${PHYSXCOMMON_WINDOWS_SOURCE})

SET(PHYSXCOMMON_RESOURCE
    ${PHYSX_SOURCE_DIR}/compiler/windows/resource/PhysXCommon.rc
    ${PHYSX_SOURCE_DIR}/compiler/windows/resource/resource.h
)
SOURCE_GROUP(resource FILES ${PHYSXCOMMON_RESOURCE})

SET(PXCOMMON_PLATFORM_SRC_FILES
    ${PHYSXCOMMON_WINDOWS_HEADERS}
    ${PHYSXCOMMON_WINDOWS_SOURCE}

    ${PHYSXCOMMON_RESOURCE}
)

SET(PXCOMMON_PLATFORM_INCLUDES
    ${PHYSX_SOURCE_DIR}/common/src/windows
)

