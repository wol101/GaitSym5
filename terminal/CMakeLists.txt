# cd into the folder where this CMakeFList.txt file is
# create a build folder with "mkdir build" and cd into it with "cd build"
# run "cmake -D CMAKE_BUILD_TYPE=Release .." for a release version
# run "cmake -D CMAKE_BUILD_TYPE=Debug .." for a debug version
# on Unix you can then run "make" to build the project (optionally using the -j flag for parallel compilation)
# on Windows you can open the generated .sln file in Visual Studio and build from there
# or you can use "msbuild gaitsym.sln -p:Configuration=Release" (optionally using the -m flag for parallel compilation)
# without -p:Configuration=Release it will build the Debug version by default
# use -t:Rebuild -p:Configuration=Release to do a clean release build

cmake_minimum_required(VERSION 3.5)

# decide whether to use libraries or source code
set(use_mucojo_src 1)
set(use_physx_src 1)

# this command stops the CMAKE_BUILD_TYPE from being cached
SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)

# set some output folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/$<CONFIG>/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/$<CONFIG>/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/$<CONFIG>/lib)

project(gaitsym VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# gaitsym5/gaitsym5_asio default to a fully static link (see the HOST_NAME
# block below) so they can run unmodified across HPC nodes with mismatched
# shared library versions. Some deployments need the opposite: e.g. running
# under DMTCP, which injects its checkpoint runtime via LD_PRELOAD and
# therefore requires a real dynamic linker to be present, which a fully
# static executable does not have. Configure with -DSTATIC_BUILD=OFF to get
# a dynamically linked build for those cases; plain cmake keeps the current
# static default. Has no effect on the csf3 branch below, which never links
# static regardless (that cluster's toolchain can't produce static binaries).
option(STATIC_BUILD "Link gaitsym5/gaitsym5_asio statically" ON)

# customise the behaviour
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # generates detailed compile_commands.json file
if(CMAKE_BUILD_TYPE STREQUAL Release)
    add_compile_definitions(QT_NO_DEBUG_OUTPUT) # stops qDebug from generating any output
    message(STATUS "QT_NO_DEBUG_OUTPUT defined")
endif()

# ODE definitions
add_compile_definitions(dIDEDOUBLE dTRIMESH_ENABLED=1 dTRIMESH_OPCODE=1 dTRIMESH_GIMPACT=0 dTRIMESH_16BIT_INDICES=0
    CCD_IDEDOUBLE dLIBCCD_ENABLED=1 dTHREADING_INTF_DISABLED=1 dTRIMESH_OPCODE_USE_OLD_TRIMESH_TRIMESH_COLLIDER=0
    ICE_NO_DLL dTLS_ENABLED=0 USE_UNIX_ERRORS)
if (MSVC)
    add_compile_definitions(HAVE_MALLOC_H)
endif()

# gle definitions
add_compile_definitions(DEBUG_OUTPUT)

# extra MSVC options required
if (MSVC)
    add_compile_definitions(_USE_MATH_DEFINES)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options(-utf-8)
    add_compile_options(-D_WIN32_WINNT=0x0601)
endif()

set(PROJECT_SOURCES
    ../ode-0.15/libccd/src/alloc.c
    ../ode-0.15/libccd/src/ccd.c
    ../ode-0.15/libccd/src/mpr.c
    ../ode-0.15/libccd/src/polytope.c
    ../ode-0.15/libccd/src/support.c
    ../ode-0.15/libccd/src/vec3.c
    ../ode-0.15/OPCODE/Ice/IceAABB.cpp
    ../ode-0.15/OPCODE/Ice/IceContainer.cpp
    ../ode-0.15/OPCODE/Ice/IceHPoint.cpp
    ../ode-0.15/OPCODE/Ice/IceIndexedTriangle.cpp
    ../ode-0.15/OPCODE/Ice/IceMatrix3x3.cpp
    ../ode-0.15/OPCODE/Ice/IceMatrix4x4.cpp
    ../ode-0.15/OPCODE/Ice/IceOBB.cpp
    ../ode-0.15/OPCODE/Ice/IcePlane.cpp
    ../ode-0.15/OPCODE/Ice/IcePoint.cpp
    ../ode-0.15/OPCODE/Ice/IceRandom.cpp
    ../ode-0.15/OPCODE/Ice/IceRay.cpp
    ../ode-0.15/OPCODE/Ice/IceRevisitedRadix.cpp
    ../ode-0.15/OPCODE/Ice/IceSegment.cpp
    ../ode-0.15/OPCODE/Ice/IceTriangle.cpp
    ../ode-0.15/OPCODE/Ice/IceUtils.cpp
    ../ode-0.15/OPCODE/OPC_AABBCollider.cpp
    ../ode-0.15/OPCODE/OPC_AABBTree.cpp
    ../ode-0.15/OPCODE/OPC_BaseModel.cpp
    ../ode-0.15/OPCODE/OPC_Collider.cpp
    ../ode-0.15/OPCODE/OPC_Common.cpp
    ../ode-0.15/OPCODE/OPC_HybridModel.cpp
    ../ode-0.15/OPCODE/OPC_LSSCollider.cpp
    ../ode-0.15/OPCODE/OPC_MeshInterface.cpp
    ../ode-0.15/OPCODE/OPC_Model.cpp
    ../ode-0.15/OPCODE/OPC_OBBCollider.cpp
    ../ode-0.15/OPCODE/OPC_OptimizedTree.cpp
    ../ode-0.15/OPCODE/OPC_Picking.cpp
    ../ode-0.15/OPCODE/OPC_PlanesCollider.cpp
    ../ode-0.15/OPCODE/OPC_RayCollider.cpp
    ../ode-0.15/OPCODE/OPC_SphereCollider.cpp
    ../ode-0.15/OPCODE/OPC_TreeBuilders.cpp
    ../ode-0.15/OPCODE/OPC_TreeCollider.cpp
    ../ode-0.15/OPCODE/OPC_VolumeCollider.cpp
    ../ode-0.15/OPCODE/Opcode.cpp
    ../ode-0.15/OPCODE/StdAfx.cpp
    ../ode-0.15/ode/src/array.cpp
    ../ode-0.15/ode/src/box.cpp
    ../ode-0.15/ode/src/capsule.cpp
    ../ode-0.15/ode/src/collision_convex_trimesh.cpp
    ../ode-0.15/ode/src/collision_cylinder_box.cpp
    ../ode-0.15/ode/src/collision_cylinder_plane.cpp
    ../ode-0.15/ode/src/collision_cylinder_sphere.cpp
    ../ode-0.15/ode/src/collision_cylinder_trimesh.cpp
    ../ode-0.15/ode/src/collision_kernel.cpp
    ../ode-0.15/ode/src/collision_libccd.cpp
    ../ode-0.15/ode/src/collision_quadtreespace.cpp
    ../ode-0.15/ode/src/collision_sapspace.cpp
    ../ode-0.15/ode/src/collision_space.cpp
    ../ode-0.15/ode/src/collision_transform.cpp
    ../ode-0.15/ode/src/collision_trimesh_box.cpp
    ../ode-0.15/ode/src/collision_trimesh_ccylinder.cpp
    ../ode-0.15/ode/src/collision_trimesh_disabled.cpp
    ../ode-0.15/ode/src/collision_trimesh_distance.cpp
    ../ode-0.15/ode/src/collision_trimesh_gimpact.cpp
    ../ode-0.15/ode/src/collision_trimesh_opcode.cpp
    ../ode-0.15/ode/src/collision_trimesh_plane.cpp
    ../ode-0.15/ode/src/collision_trimesh_ray.cpp
    ../ode-0.15/ode/src/collision_trimesh_sphere.cpp
    ../ode-0.15/ode/src/collision_trimesh_trimesh.cpp
    ../ode-0.15/ode/src/collision_trimesh_trimesh_new.cpp
    ../ode-0.15/ode/src/collision_util.cpp
    ../ode-0.15/ode/src/convex.cpp
    ../ode-0.15/ode/src/cylinder.cpp
    ../ode-0.15/ode/src/error.cpp
    ../ode-0.15/ode/src/export-dif.cpp
    ../ode-0.15/ode/src/fastdot.cpp
    ../ode-0.15/ode/src/fastldlt.cpp
    ../ode-0.15/ode/src/fastlsolve.cpp
    ../ode-0.15/ode/src/fastltsolve.cpp
    ../ode-0.15/ode/src/heightfield.cpp
    ../ode-0.15/ode/src/joints/amotor.cpp
    ../ode-0.15/ode/src/joints/ball.cpp
    ../ode-0.15/ode/src/joints/contact.cpp
    ../ode-0.15/ode/src/joints/dball.cpp
    ../ode-0.15/ode/src/joints/dhinge.cpp
    ../ode-0.15/ode/src/joints/fixed.cpp
    ../ode-0.15/ode/src/joints/floatinghinge.cpp
    ../ode-0.15/ode/src/joints/hinge.cpp
    ../ode-0.15/ode/src/joints/hinge2.cpp
    ../ode-0.15/ode/src/joints/joint.cpp
    ../ode-0.15/ode/src/joints/lmotor.cpp
    ../ode-0.15/ode/src/joints/null.cpp
    ../ode-0.15/ode/src/joints/piston.cpp
    ../ode-0.15/ode/src/joints/plane2d.cpp
    ../ode-0.15/ode/src/joints/pr.cpp
    ../ode-0.15/ode/src/joints/pu.cpp
    ../ode-0.15/ode/src/joints/slider.cpp
    ../ode-0.15/ode/src/joints/transmission.cpp
    ../ode-0.15/ode/src/joints/universal.cpp
    ../ode-0.15/ode/src/lcp.cpp
    ../ode-0.15/ode/src/mass.cpp
    ../ode-0.15/ode/src/mat.cpp
    ../ode-0.15/ode/src/matrix.cpp
    ../ode-0.15/ode/src/memory.cpp
    ../ode-0.15/ode/src/misc.cpp
    ../ode-0.15/ode/src/nextafterf.c
    ../ode-0.15/ode/src/objects.cpp
    ../ode-0.15/ode/src/obstack.cpp
    ../ode-0.15/ode/src/ode.cpp
    ../ode-0.15/ode/src/odeinit.cpp
    ../ode-0.15/ode/src/odemath.cpp
    ../ode-0.15/ode/src/odeou.cpp
    ../ode-0.15/ode/src/odetls.cpp
    ../ode-0.15/ode/src/plane.cpp
    ../ode-0.15/ode/src/quickstep.cpp
    ../ode-0.15/ode/src/ray.cpp
    ../ode-0.15/ode/src/rotation.cpp
    ../ode-0.15/ode/src/sphere.cpp
    ../ode-0.15/ode/src/step.cpp
    ../ode-0.15/ode/src/threading_base.cpp
    ../ode-0.15/ode/src/threading_impl.cpp
    ../ode-0.15/ode/src/threading_pool_posix.cpp
    ../ode-0.15/ode/src/threading_pool_win.cpp
    ../ode-0.15/ode/src/timer.cpp
    ../ode-0.15/ode/src/util.cpp
    ../ode-0.15/OPCODE/Ice/IceAABB.h
    ../ode-0.15/OPCODE/Ice/IceAxes.h
    ../ode-0.15/OPCODE/Ice/IceBoundingSphere.h
    ../ode-0.15/OPCODE/Ice/IceContainer.h
    ../ode-0.15/OPCODE/Ice/IceFPU.h
    ../ode-0.15/OPCODE/Ice/IceHPoint.h
    ../ode-0.15/OPCODE/Ice/IceIndexedTriangle.h
    ../ode-0.15/OPCODE/Ice/IceLSS.h
    ../ode-0.15/OPCODE/Ice/IceMatrix3x3.h
    ../ode-0.15/OPCODE/Ice/IceMatrix4x4.h
    ../ode-0.15/OPCODE/Ice/IceMemoryMacros.h
    ../ode-0.15/OPCODE/Ice/IceOBB.h
    ../ode-0.15/OPCODE/Ice/IcePairs.h
    ../ode-0.15/OPCODE/Ice/IcePlane.h
    ../ode-0.15/OPCODE/Ice/IcePoint.h
    ../ode-0.15/OPCODE/Ice/IcePreprocessor.h
    ../ode-0.15/OPCODE/Ice/IceRandom.h
    ../ode-0.15/OPCODE/Ice/IceRay.h
    ../ode-0.15/OPCODE/Ice/IceRevisitedRadix.h
    ../ode-0.15/OPCODE/Ice/IceSegment.h
    ../ode-0.15/OPCODE/Ice/IceTriList.h
    ../ode-0.15/OPCODE/Ice/IceTriangle.h
    ../ode-0.15/OPCODE/Ice/IceTypes.h
    ../ode-0.15/OPCODE/Ice/IceUtils.h
    ../ode-0.15/OPCODE/OPC_AABBCollider.h
    ../ode-0.15/OPCODE/OPC_AABBTree.h
    ../ode-0.15/OPCODE/OPC_BaseModel.h
    ../ode-0.15/OPCODE/OPC_BoxBoxOverlap.h
    ../ode-0.15/OPCODE/OPC_Collider.h
    ../ode-0.15/OPCODE/OPC_Common.h
    ../ode-0.15/OPCODE/OPC_HybridModel.h
    ../ode-0.15/OPCODE/OPC_IceHook.h
    ../ode-0.15/OPCODE/OPC_LSSAABBOverlap.h
    ../ode-0.15/OPCODE/OPC_LSSCollider.h
    ../ode-0.15/OPCODE/OPC_LSSTriOverlap.h
    ../ode-0.15/OPCODE/OPC_MeshInterface.h
    ../ode-0.15/OPCODE/OPC_Model.h
    ../ode-0.15/OPCODE/OPC_OBBCollider.h
    ../ode-0.15/OPCODE/OPC_OptimizedTree.h
    ../ode-0.15/OPCODE/OPC_Picking.h
    ../ode-0.15/OPCODE/OPC_PlanesAABBOverlap.h
    ../ode-0.15/OPCODE/OPC_PlanesCollider.h
    ../ode-0.15/OPCODE/OPC_PlanesTriOverlap.h
    ../ode-0.15/OPCODE/OPC_RayAABBOverlap.h
    ../ode-0.15/OPCODE/OPC_RayCollider.h
    ../ode-0.15/OPCODE/OPC_RayTriOverlap.h
    ../ode-0.15/OPCODE/OPC_Settings.h
    ../ode-0.15/OPCODE/OPC_SphereAABBOverlap.h
    ../ode-0.15/OPCODE/OPC_SphereCollider.h
    ../ode-0.15/OPCODE/OPC_SphereTriOverlap.h
    ../ode-0.15/OPCODE/OPC_TreeBuilders.h
    ../ode-0.15/OPCODE/OPC_TreeCollider.h
    ../ode-0.15/OPCODE/OPC_TriBoxOverlap.h
    ../ode-0.15/OPCODE/OPC_TriTriOverlap.h
    ../ode-0.15/OPCODE/OPC_VolumeCollider.h
    ../ode-0.15/OPCODE/Opcode.h
    ../ode-0.15/OPCODE/Stdafx.h
    ../ode-0.15/include/ode/collision.h
    ../ode-0.15/include/ode/collision_space.h
    ../ode-0.15/include/ode/collision_trimesh.h
    ../ode-0.15/include/ode/common.h
    ../ode-0.15/include/ode/compatibility.h
    ../ode-0.15/include/ode/contact.h
    ../ode-0.15/include/ode/error.h
    ../ode-0.15/include/ode/export-dif.h
    ../ode-0.15/include/ode/mass.h
    ../ode-0.15/include/ode/matrix.h
    ../ode-0.15/include/ode/memory.h
    ../ode-0.15/include/ode/misc.h
    ../ode-0.15/include/ode/objects.h
    ../ode-0.15/include/ode/ode.h
    ../ode-0.15/include/ode/odeconfig.h
    ../ode-0.15/include/ode/odecpp.h
    ../ode-0.15/include/ode/odecpp_collision.h
    ../ode-0.15/include/ode/odeinit.h
    ../ode-0.15/include/ode/odemath.h
    ../ode-0.15/include/ode/odemath_legacy.h
    ../ode-0.15/include/ode/precision.h
    ../ode-0.15/include/ode/rotation.h
    ../ode-0.15/include/ode/threading.h
    ../ode-0.15/include/ode/threading_impl.h
    ../ode-0.15/include/ode/timer.h
    ../ode-0.15/include/ode/version.h
    ../ode-0.15/ode/src/array.h
    ../ode-0.15/ode/src/collision_kernel.h
    ../ode-0.15/ode/src/collision_libccd.h
    ../ode-0.15/ode/src/collision_space_internal.h
    ../ode-0.15/ode/src/collision_std.h
    ../ode-0.15/ode/src/collision_transform.h
    ../ode-0.15/ode/src/collision_trimesh_colliders.h
    ../ode-0.15/ode/src/collision_trimesh_internal.h
    ../ode-0.15/ode/src/collision_util.h
    ../ode-0.15/ode/src/config.h
    ../ode-0.15/ode/src/error.h
    ../ode-0.15/ode/src/fastdot_impl.h
    ../ode-0.15/ode/src/fastldlt_impl.h
    ../ode-0.15/ode/src/fastltsolve_impl.h
    ../ode-0.15/ode/src/fastsolve_impl.h
    ../ode-0.15/ode/src/heightfield.h
    ../ode-0.15/ode/src/joints/amotor.h
    ../ode-0.15/ode/src/joints/ball.h
    ../ode-0.15/ode/src/joints/contact.h
    ../ode-0.15/ode/src/joints/dball.h
    ../ode-0.15/ode/src/joints/dhinge.h
    ../ode-0.15/ode/src/joints/fixed.h
    ../ode-0.15/ode/src/joints/floatinghinge.h
    ../ode-0.15/ode/src/joints/hinge.h
    ../ode-0.15/ode/src/joints/hinge2.h
    ../ode-0.15/ode/src/joints/joint.h
    ../ode-0.15/ode/src/joints/joint_internal.h
    ../ode-0.15/ode/src/joints/joints.h
    ../ode-0.15/ode/src/joints/lmotor.h
    ../ode-0.15/ode/src/joints/null.h
    ../ode-0.15/ode/src/joints/piston.h
    ../ode-0.15/ode/src/joints/plane2d.h
    ../ode-0.15/ode/src/joints/pr.h
    ../ode-0.15/ode/src/joints/pu.h
    ../ode-0.15/ode/src/joints/slider.h
    ../ode-0.15/ode/src/joints/transmission.h
    ../ode-0.15/ode/src/joints/universal.h
    ../ode-0.15/ode/src/lcp.h
    ../ode-0.15/ode/src/mat.h
    ../ode-0.15/ode/src/matrix.h
    ../ode-0.15/ode/src/matrix_impl.h
    ../ode-0.15/ode/src/objects.h
    ../ode-0.15/ode/src/obstack.h
    ../ode-0.15/ode/src/odemath.h
    ../ode-0.15/ode/src/odeou.h
    ../ode-0.15/ode/src/odetls.h
    ../ode-0.15/ode/src/quickstep.h
    ../ode-0.15/ode/src/step.h
    ../ode-0.15/ode/src/threading_atomics_provs.h
    ../ode-0.15/ode/src/threading_base.h
    ../ode-0.15/ode/src/threading_fake_sync.h
    ../ode-0.15/ode/src/threading_impl.h
    ../ode-0.15/ode/src/threading_impl_posix.h
    ../ode-0.15/ode/src/threading_impl_templates.h
    ../ode-0.15/ode/src/threading_impl_win.h
    ../ode-0.15/ode/src/threadingutils.h
    ../ode-0.15/ode/src/typedefs.h
    ../ode-0.15/ode/src/util.h

    ../pystring/pystring.cpp

    ../src/AMotorJoint.cpp
    ../src/ArgParse.cpp
    ../src/BallJoint.cpp
    ../src/Body.cpp
    ../src/BoxGeom.cpp
    ../src/ButterworthFilter.cpp
    ../src/CappedCylinderGeom.cpp
    ../src/Colour.cpp
    ../src/Contact.cpp
    ../src/Controller.cpp
    ../src/ConvexGeom.cpp
    ../src/CyclicDriver.cpp
    ../src/CylinderWrapStrap.cpp
    ../src/DampedSpringMuscle.cpp
    ../src/DataFile.cpp
    ../src/DataTarget.cpp
    ../src/DataTargetMarkerCompare.cpp
    ../src/DataTargetQuaternion.cpp
    ../src/DataTargetScalar.cpp
    ../src/DataTargetVector.cpp
    ../src/Drivable.cpp
    ../src/Driver.cpp
    ../src/Filter.cpp
    ../src/FixedDriver.cpp
    ../src/FixedJoint.cpp
    ../src/FloatingHingeJoint.cpp
    ../src/FluidSac.cpp
    ../src/FluidSacIdealGas.cpp
    ../src/FluidSacIncompressible.cpp
    ../src/Geom.cpp
    ../src/Global.cpp
    ../src/GSUtil.cpp
    ../src/HingeJoint.cpp
    ../src/Joint.cpp
    ../src/LightBase.cpp
    ../src/LMotorJoint.cpp
    ../src/MAMuscle.cpp
    ../src/MAMuscleComplete.cpp
    ../src/MagicMuscle.cpp
    ../src/MagicStrap.cpp
    ../src/Marker.cpp
    ../src/MarkerEllipseDriver.cpp
    ../src/MarkerPositionDriver.cpp
    ../src/MarkerReporter.cpp
    ../src/MovingAverage.cpp
    ../src/MuJoCoPhysicsEngine.cpp
    ../src/Muscle.cpp
    ../src/NamedObject.cpp
    ../src/NPointStrap.cpp
    ../src/ObjectiveMain.cpp
    ../src/ODEPhysicsEngine.cpp
    ../src/OpenSimExporter.cpp
    ../src/ParseXML.cpp
    ../src/PGDMath.cpp
    ../src/PhysicsEngine.cpp
    ../src/PhysXPhysicsEngine.cpp
    ../src/PIDErrorInController.cpp
    ../src/PIDMuscleLengthController.cpp
    ../src/PlaneGeom.cpp
    ../src/PlaybackPhysicsEngine.cpp
    ../src/Reporter.cpp
    ../src/Simulation.cpp
    ../src/SliderJoint.cpp
    ../src/SphereGeom.cpp
    ../src/StackedBoxCarDriver.cpp
    ../src/StepDriver.cpp
    ../src/Strap.cpp
    ../src/TegotaeDriver.cpp
    ../src/ThreeHingeJointDriver.cpp
    ../src/TorqueReporter.cpp
    ../src/TrimeshGeom.cpp
    ../src/TwoCylinderWrapStrap.cpp
    ../src/TwoHingeJointDriver.cpp
    ../src/TwoPointStrap.cpp
    ../src/UniversalJoint.cpp
    ../src/XMLConverter.cpp

    ../pocketpy-2.0.4/pocketpy.c

    ../fast_double_parser/fast_double_parser/fast_double_parser.h
    ../pystring/pystring.h

    ../src/AMotorJoint.h
    ../src/ArgParse.h
    ../src/BallJoint.h
    ../src/Body.h
    ../src/BoxGeom.h
    ../src/ButterworthFilter.h
    ../src/CappedCylinderGeom.h
    ../src/Colour.h
    ../src/Contact.h
    ../src/Controller.h
    ../src/ConvexGeom.h
    ../src/CyclicDriver.h
    ../src/CylinderWrapStrap.h
    ../src/DampedSpringMuscle.h
    ../src/DataFile.h
    ../src/DataTarget.h
    ../src/DataTargetMarkerCompare.h
    ../src/DataTargetQuaternion.h
    ../src/DataTargetScalar.h
    ../src/DataTargetVector.h
    ../src/Drivable.h
    ../src/Driver.h
    ../src/Filter.h
    ../src/FixedDriver.h
    ../src/FixedJoint.h
    ../src/FloatingHingeJoint.h
    ../src/FluidSac.h
    ../src/FluidSacIdealGas.h
    ../src/FluidSacIncompressible.h
    ../src/Geom.h
    ../src/Global.h
    ../src/GSUtil.h
    ../src/HingeJoint.h
    ../src/Joint.h
    ../src/LightBase.h
    ../src/LMotorJoint.h
    ../src/MAMuscle.h
    ../src/MAMuscleComplete.h
    ../src/MagicMuscle.h
    ../src/MagicStrap.h
    ../src/Marker.h
    ../src/MarkerEllipseDriver.h
    ../src/MarkerPositionDriver.h
    ../src/MarkerReporter.h
    ../src/MovingAverage.h
    ../src/Muscle.h
    ../src/MuJoCoPhysicsEngine.h
    ../src/NamedObject.h
    ../src/NPointStrap.h
    ../src/ObjectiveMain.h
    ../src/ODEPhysicsEngine.h
    ../src/OpenSimExporter.h
    ../src/ParseXML.h
    ../src/PhysicsEngine.h
    ../src/PhysXPhysicsEngine.h
    ../src/PGDMath.h
    ../src/PIDErrorInController.h
    ../src/PIDMuscleLengthController.h
    ../src/PlaneGeom.h
    ../src/PlaybackPhysicsEngine.h
    ../src/Reporter.h
    ../src/Simulation.h
    ../src/SliderJoint.h
    ../src/SmartEnum.h
    ../src/SphereGeom.h
    ../src/StackedBoxCarDriver.h
    ../src/StepDriver.h
    ../src/Strap.h
    ../src/TegotaeDriver.h
    ../src/ThreeHingeJointDriver.h
    ../src/TorqueReporter.h
    ../src/TrimeshGeom.h
    ../src/TwoCylinderWrapStrap.h
    ../src/TwoHingeJointDriver.h
    ../src/TwoPointStrap.h
    ../src/UniversalJoint.h
    ../src/XMLConverter.h

    ../tinyply/tinyply.h
    ../pocketpy-2.0.4/pocketpy.h

    ../rapidxml/rapidxml.hpp
    ../rapidxml/rapidxml_iterators.hpp
    ../rapidxml/rapidxml_print.hpp
    ../rapidxml/rapidxml_utils.hpp

    ../src/ObjectiveMainASIOAsync.cpp
    ../src/ObjectiveMainASIOAsync.h
)

set(INCLUDE_DIRECTORIES
    ../fast_double_parser
    ../GaitSymQt
    ../PhysX
    ../pocketpy-2.0.4
    ../pystring
    ../rapidxml
    ../src
    ../tinyply
    ../PhysX/PhysX-107.0/physx/include
    ../MuJoCo/mujoco-3.3.0/include
    ../asio-1.18.2/include
    ../ode-0.15/OPCODE
    ../ode-0.15/include
    ../ode-0.15/libccd/src/
    ../ode-0.15/ode/src
)

if (use_mucojo_src)
    add_compile_definitions(MC_CPP_USE_DOUBLE_PRECISION MC_IMPLEM_ENABLE MJ_STATIC CCD_STATIC_DEFINE)
    set(PROJECT_SOURCES ${PROJECT_SOURCES}
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_array_safety.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_callback.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_callback.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_collision_box.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_collision_convex.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_collision_convex.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_collision_driver.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_collision_driver.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_collision_gjk.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_collision_gjk.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_collision_primitive.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_collision_primitive.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_collision_sdf.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_collision_sdf.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_core_constraint.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_core_constraint.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_core_smooth.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_core_smooth.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_crossplatform.cc
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_crossplatform.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_derivative.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_derivative.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_derivative_fd.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_derivative_fd.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_forward.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_forward.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_global_table.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_inverse.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_inverse.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_island.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_island.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_io.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_io.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_macro.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_name.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_name.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_passive.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_passive.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_plugin.cc
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_plugin.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_print.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_print.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_ray.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_ray.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_sensor.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_sensor.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_setconst.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_setconst.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_solver.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_solver.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_sort.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_support.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_support.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_util_blas.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_util_blas.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_util_errmem.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_util_errmem.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_util_misc.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_util_misc.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_util_solve.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_util_solve.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_util_sparse.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_util_sparse.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_util_sparse_avx.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_util_spatial.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_util_spatial.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_vis_init.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_vis_init.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_vis_interact.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_vis_interact.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_vis_state.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_vis_state.h
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_vis_visualize.c
        ../MuJoCo/mujoco-3.3.0/src/engine/engine_vis_visualize.h
        ../MuJoCo/mujoco-3.3.0/src/xml/xml_api.cc
        ../MuJoCo/mujoco-3.3.0/src/xml/xml_api.h
        ../MuJoCo/mujoco-3.3.0/src/xml/xml_base.cc
        ../MuJoCo/mujoco-3.3.0/src/xml/xml_base.h
        ../MuJoCo/mujoco-3.3.0/src/xml/xml.cc
        ../MuJoCo/mujoco-3.3.0/src/xml/xml.h
        ../MuJoCo/mujoco-3.3.0/src/xml/xml_native_reader.cc
        ../MuJoCo/mujoco-3.3.0/src/xml/xml_native_reader.h
        ../MuJoCo/mujoco-3.3.0/src/xml/xml_numeric_format.cc
        ../MuJoCo/mujoco-3.3.0/src/xml/xml_numeric_format.h
        ../MuJoCo/mujoco-3.3.0/src/xml/xml_native_writer.cc
        ../MuJoCo/mujoco-3.3.0/src/xml/xml_native_writer.h
        ../MuJoCo/mujoco-3.3.0/src/xml/xml_urdf.cc
        ../MuJoCo/mujoco-3.3.0/src/xml/xml_urdf.h
        ../MuJoCo/mujoco-3.3.0/src/xml/xml_util.cc
        ../MuJoCo/mujoco-3.3.0/src/xml/xml_util.h
        ../MuJoCo/mujoco-3.3.0/src/thread/thread_pool.cc
        ../MuJoCo/mujoco-3.3.0/src/thread/thread_pool.h
        ../MuJoCo/mujoco-3.3.0/src/thread/thread_queue.h
        ../MuJoCo/mujoco-3.3.0/src/thread/thread_task.cc
        ../MuJoCo/mujoco-3.3.0/src/thread/thread_task.h
        ../MuJoCo/mujoco-3.3.0/src/user/user_api.cc
        ../MuJoCo/mujoco-3.3.0/src/user/user_api.h
        ../MuJoCo/mujoco-3.3.0/src/user/user_cache.cc
        ../MuJoCo/mujoco-3.3.0/src/user/user_cache.h
        ../MuJoCo/mujoco-3.3.0/src/user/user_composite.cc
        ../MuJoCo/mujoco-3.3.0/src/user/user_composite.h
        ../MuJoCo/mujoco-3.3.0/src/user/user_flexcomp.cc
        ../MuJoCo/mujoco-3.3.0/src/user/user_flexcomp.h
        ../MuJoCo/mujoco-3.3.0/src/user/user_init.c
        ../MuJoCo/mujoco-3.3.0/src/user/user_mesh.cc
        ../MuJoCo/mujoco-3.3.0/src/user/user_model.cc
        ../MuJoCo/mujoco-3.3.0/src/user/user_model.h
        ../MuJoCo/mujoco-3.3.0/src/user/user_objects.cc
        ../MuJoCo/mujoco-3.3.0/src/user/user_objects.h
        ../MuJoCo/mujoco-3.3.0/src/user/user_resource.cc
        ../MuJoCo/mujoco-3.3.0/src/user/user_resource.h
        ../MuJoCo/mujoco-3.3.0/src/user/user_util.cc
        ../MuJoCo/mujoco-3.3.0/src/user/user_util.h
        ../MuJoCo/mujoco-3.3.0/src/user/user_vfs.cc
        ../MuJoCo/mujoco-3.3.0/src/user/user_vfs.h
        ../MuJoCo/mujoco-3.3.0/dependencies/tinyxml2-src/tinyxml2.cpp
        ../MuJoCo/mujoco-3.3.0/dependencies/tinyxml2-src/tinyxml2.h
        ../MuJoCo/mujoco-3.3.0/dependencies/tinyobjloader-src/tiny_obj_loader.h
        ../MuJoCo/mujoco-3.3.0/dependencies/tinyobjloader-src/tiny_obj_loader.cc
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/libqhull_r.h
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/geom_r.h
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/io_r.h
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/mem_r.h
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/merge_r.h
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/poly_r.h
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/qhull_ra.h
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/qset_r.h
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/random_r.h
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/stat_r.h
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/user_r.h
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/global_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/stat_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/geom2_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/poly2_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/merge_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/libqhull_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/geom_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/poly_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/qset_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/mem_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/random_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/usermem_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/userprintf_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/io_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/user_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/accessors_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/rboxlib_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r/userprintf_rbox_r.c
        ../MuJoCo/mujoco-3.3.0/dependencies/lodepng-src/lodepng.cpp
        ../MuJoCo/mujoco-3.3.0/dependencies/lodepng-src/lodepng.h
    )

    set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
        ../MuJoCo/mujoco-3.3.0/src
        ../MuJoCo/mujoco-3.3.0/dependencies/lodepng-src
        ../MuJoCo/mujoco-3.3.0/dependencies/marchingcubecpp-src
        ../MuJoCo/mujoco-3.3.0/dependencies/qhull-src/src/libqhull_r
        ../MuJoCo/mujoco-3.3.0/dependencies/tinyobjloader-src
        ../MuJoCo/mujoco-3.3.0/dependencies/tinyxml2-src
    )

endif()

if (use_physx_src)
    # compiler define options (get set to zero if undefined)
    # PX_SUPPORT_GPU_PHYSX
    # PX_SUPPORT_PVD
    # PX_PHYSX_STATIC_LIB [turns off a lot of DLL specific code]
    # DISABLE_CUDA_PHYSX
    # PX_SUPPORT_OMNI_PVD
    # PX_SUPPORT_PVD
    # PX_NVTX

    # build style compiler definitions (do not define any for release code)
    # PX_DEBUG    [optimisation off, SDK development asserts, very intrusive, must be used with PX_CHECKED]
    # PX_CHECKED  [code to detect invalid parameters, API race conditions, and other incorrect uses of the API]
    # PX_PROFILE  [omits the checks, but still has PVD and memory instrumentation]

    # cmake variables used to control build
    # PX_SCALAR_MATH [Disable SIMD math]
    # PX_PUBLIC_RELEASE

    add_compile_definitions(DISABLE_CUDA_PHYSX PX_PHYSX_STATIC_LIB)
    if(CMAKE_BUILD_TYPE STREQUAL Debug)
        add_compile_definitions(PX_DEBUG PX_CHECKED)
    endif()
    if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL arm64)
        add_compile_definitions(PX_SIMD_DISABLED)
    endif()

    # cmake variables used to control build
    # PX_SCALAR_MATH [Disable SIMD math]
    # PX_PUBLIC_RELEASE
    # PX_GENERATE_STATIC_LIBRARIES
    # PX_FLOAT_POINT_PRECISE_MATH [Visual C++ only]

    set(PX_FLOAT_POINT_PRECISE_MATH 1)
    set(PHYSX_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0/physx)
    set(PHYSX_SOURCE_DIR ${PHYSX_ROOT_DIR}/source)
    set (LL_SOURCE_DIR ${PHYSX_SOURCE_DIR}/foundation)
    set (COOKING_DIR ${PHYSX_SOURCE_DIR}/physxcooking/src)
    set(PX_SOURCE_DIR ${PHYSX_SOURCE_DIR}/physx/src)
    set(COMMON_SRC_DIR ${PHYSX_SOURCE_DIR}/common/src)
    set(LL_API_DIR ${PHYSX_SOURCE_DIR}/lowlevel/api)
    set(LL_COMMON_DIR ${PHYSX_SOURCE_DIR}/lowlevel/common)
    set(LL_SOFTWARE_DIR ${PHYSX_SOURCE_DIR}/lowlevel/software)
    set(LLAABB_DIR ${PHYSX_SOURCE_DIR}/lowlevelaabb)
    set(LLDYNAMICS_BASE_DIR ${PHYSX_SOURCE_DIR}/lowleveldynamics)
    set(MD_SOURCE_DIR ${PHYSX_SOURCE_DIR}/physxmetadata)
    set(CHARACTERKINEMATICS_DIR ${PHYSX_SOURCE_DIR}/physxcharacterkinematic/src)
    set(GU_SOURCE_DIR ${PHYSX_SOURCE_DIR}/geomutils)
    set(EXTENSIONS_DIR ${PHYSX_SOURCE_DIR}/physxextensions/src)
    set(SERIALIZATION_DIR ${PHYSX_SOURCE_DIR}/physxextensions/src)
    set(TASK_DIR ${PHYSX_SOURCE_DIR}/task)
    set(VEHICLE_DIR ${PHYSX_SOURCE_DIR}/physxvehicle/src)
    set(SCENEQUERY_BASE_DIR ${PHYSX_SOURCE_DIR}/scenequery)
    set(SIMULATIONCONTROLLER_BASE_DIR ${PHYSX_SOURCE_DIR}/simulationcontroller)
    if (UNIX)
        set(TARGET_BUILD_PLATFORM linux)
        set(PROJECT_SOURCES ${PROJECT_SOURCES}
            ${PX_SOURCE_DIR}/gpu/PxGpu.cpp
            ${PX_SOURCE_DIR}/gpu/PxPhysXGpuModuleLoader.cpp
            ${LL_SOURCE_DIR}/unix/FdUnixAtomic.cpp
            ${LL_SOURCE_DIR}/unix/FdUnixMutex.cpp
            ${LL_SOURCE_DIR}/unix/FdUnixSync.cpp
            ${LL_SOURCE_DIR}/unix/FdUnixThread.cpp
            ${LL_SOURCE_DIR}/unix/FdUnixPrintString.cpp
            ${LL_SOURCE_DIR}/unix/FdUnixSList.cpp
            ${LL_SOURCE_DIR}/unix/FdUnixSocket.cpp
            ${LL_SOURCE_DIR}/unix/FdUnixTime.cpp
            ${LL_SOURCE_DIR}/unix/FdUnixFPU.cpp
        )
    endif()
    if (WIN32)
        set(TARGET_BUILD_PLATFORM windows)
        set(PROJECT_SOURCES ${PROJECT_SOURCES}
            ${PX_SOURCE_DIR}/gpu/PxGpu.cpp
            ${PX_SOURCE_DIR}/gpu/PxPhysXGpuModuleLoader.cpp
            ${PX_SOURCE_DIR}/windows/NpWindowsDelayLoadHook.cpp
            ${COMMON_SRC_DIR}/windows/CmWindowsModuleUpdateLoader.cpp
            ${COMMON_SRC_DIR}/windows/CmWindowsDelayLoadHook.cpp
            ${COOKING_DIR}/windows/WindowsCookingDelayLoadHook.cpp
            ${LL_SOURCE_DIR}/windows/FdWindowsAtomic.cpp
            ${LL_SOURCE_DIR}/windows/FdWindowsMutex.cpp
            ${LL_SOURCE_DIR}/windows/FdWindowsSync.cpp
            ${LL_SOURCE_DIR}/windows/FdWindowsThread.cpp
            ${LL_SOURCE_DIR}/windows/FdWindowsPrintString.cpp
            ${LL_SOURCE_DIR}/windows/FdWindowsSList.cpp
            ${LL_SOURCE_DIR}/windows/FdWindowsSocket.cpp
            ${LL_SOURCE_DIR}/windows/FdWindowsTime.cpp
            ${LL_SOURCE_DIR}/windows/FdWindowsFPU.cpp
        )
    endif()

    set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
        ${PHYSX_SOURCE_DIR}/common/include
        ${PHYSX_SOURCE_DIR}/common/src
        ${PHYSX_SOURCE_DIR}/geomutils/include
        ${PHYSX_SOURCE_DIR}/geomutils/src
        ${PHYSX_SOURCE_DIR}/geomutils/src/ccd
        ${PHYSX_SOURCE_DIR}/geomutils/src/common
        ${PHYSX_SOURCE_DIR}/geomutils/src/contact
        ${PHYSX_SOURCE_DIR}/geomutils/src/convex
        ${PHYSX_SOURCE_DIR}/geomutils/src/distance
        ${PHYSX_SOURCE_DIR}/geomutils/src/gjk
        ${PHYSX_SOURCE_DIR}/geomutils/src/hf
        ${PHYSX_SOURCE_DIR}/geomutils/src/intersection
        ${PHYSX_SOURCE_DIR}/geomutils/src/mesh
        ${PHYSX_SOURCE_DIR}/geomutils/src/pcm
        ${PHYSX_SOURCE_DIR}/geomutils/src/sweep
        ${PHYSX_SOURCE_DIR}/lowlevel/api/include
        ${PHYSX_SOURCE_DIR}/lowlevel/common/include/collision
        ${PHYSX_SOURCE_DIR}/lowlevel/common/include/pipeline
        ${PHYSX_SOURCE_DIR}/lowlevel/common/include/utils
        ${PHYSX_SOURCE_DIR}/lowlevel/software/include
        ${PHYSX_SOURCE_DIR}/lowlevelaabb/include
        ${PHYSX_SOURCE_DIR}/lowleveldynamics/include
        ${PHYSX_SOURCE_DIR}/lowleveldynamics/shared
        ${PHYSX_SOURCE_DIR}/physx/src
        ${PHYSX_SOURCE_DIR}/physxextensions/src
        ${PHYSX_SOURCE_DIR}/physxextensions/src/serialization/File
        ${PHYSX_SOURCE_DIR}/physxextensions/src/serialization/Xml
        ${PHYSX_SOURCE_DIR}/physxgpu/include
        ${PHYSX_SOURCE_DIR}/physxmetadata/core/include
        ${PHYSX_SOURCE_DIR}/physxmetadata/extensions/include
        ${PHYSX_SOURCE_DIR}/pvd/include
        ${PHYSX_SOURCE_DIR}/scenequery/include
        ${PHYSX_SOURCE_DIR}/simulationcontroller/include
        ${PHYSX_SOURCE_DIR}/simulationcontroller/src
    )


    set(PROJECT_SOURCES ${PROJECT_SOURCES}
        ${LL_SOURCE_DIR}/FdAllocator.cpp
        ${LL_SOURCE_DIR}/FdString.cpp
        ${LL_SOURCE_DIR}/FdTempAllocator.cpp
        ${LL_SOURCE_DIR}/FdAssert.cpp
        ${LL_SOURCE_DIR}/FdMathUtils.cpp
        ${LL_SOURCE_DIR}/FdFoundation.cpp
        ${LL_SOURCE_DIR}/FdFoundation.h
        ${LL_API_DIR}/src/px_globals.cpp
        ${LL_COMMON_DIR}/src/pipeline/PxcContactCache.cpp
        ${LL_COMMON_DIR}/src/pipeline/PxcContactMethodImpl.cpp
        ${LL_COMMON_DIR}/src/pipeline/PxcMaterialMethodImpl.cpp
        ${LL_COMMON_DIR}/src/pipeline/PxcNpBatch.cpp
        ${LL_COMMON_DIR}/src/pipeline/PxcNpCacheStreamPair.cpp
        ${LL_COMMON_DIR}/src/pipeline/PxcNpContactPrepShared.cpp
        ${LL_COMMON_DIR}/src/pipeline/PxcNpMemBlockPool.cpp
        ${LL_COMMON_DIR}/src/pipeline/PxcNpThreadContext.cpp
        ${LL_SOFTWARE_DIR}/src/PxsCCD.cpp
        ${LL_SOFTWARE_DIR}/src/PxsContactManager.cpp
        ${LL_SOFTWARE_DIR}/src/PxsContext.cpp
        ${LL_SOFTWARE_DIR}/src/PxsDefaultMemoryManager.cpp
        ${LL_SOFTWARE_DIR}/src/PxsIslandSim.cpp
        ${LL_SOFTWARE_DIR}/src/PxsNphaseImplementationContext.cpp
        ${LL_SOFTWARE_DIR}/src/PxsSimpleIslandManager.cpp
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
        ${LLDYNAMICS_BASE_DIR}/src/DyAllocator.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyArticulationContactPrep.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyArticulationMimicJoint.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyFeatherstoneArticulation.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyFeatherstoneForwardDynamic.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyFeatherstoneInverseDynamic.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyConstraintPartition.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyConstraintSetup.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyConstraintSetupBlock.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyContactPrep.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyContactPrep4.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyDynamicsBase.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyDynamics.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyFrictionCorrelation.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyRigidBodyToSolverBody.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DySolverConstraints.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DySolverConstraintsBlock.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DySolverControl.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DySolverConstraint1DStep.h
        ${LLDYNAMICS_BASE_DIR}/src/DyThreadContext.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyThresholdTable.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyTGSDynamics.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyTGSContactPrep.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DyTGSContactPrepBlock.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DySolverCore.cpp
        ${LLDYNAMICS_BASE_DIR}/src/DySleep.cpp
        ${MD_SOURCE_DIR}/core/src/PxAutoGeneratedMetaDataObjects.cpp
        ${MD_SOURCE_DIR}/core/src/PxMetaDataObjects.cpp
        ${MD_SOURCE_DIR}/core/src/PxAutoGeneratedMetaDataObjects.cpp
        ${MD_SOURCE_DIR}/core/src/PxMetaDataObjects.cpp
        ${PX_SOURCE_DIR}/omnipvd/NpOmniPvd.cpp
        ${PX_SOURCE_DIR}/omnipvd/NpOmniPvdRegistrationData.cpp
        ${PX_SOURCE_DIR}/omnipvd/NpOmniPvdMetaData.cpp
        ${PX_SOURCE_DIR}/omnipvd/NpOmniPvdSimulationControllerCallbacks.cpp
        ${PX_SOURCE_DIR}/omnipvd/OmniPvdPxSampler.cpp
        ${PX_SOURCE_DIR}/omnipvd/OmniPvdChunkAlloc.cpp
        ${PX_SOURCE_DIR}/NpPvdSceneClient.cpp
        ${PX_SOURCE_DIR}/NpPvdSceneQueryCollector.cpp
        ${PX_SOURCE_DIR}/PvdMetaDataPvdBinding.cpp
        ${PX_SOURCE_DIR}/PvdPhysicsClient.cpp
        ${PHYSX_ROOT_DIR}/source/immediatemode/src/NpImmediateMode.cpp
        ${PX_SOURCE_DIR}/NpMaterial.cpp
        ${PX_SOURCE_DIR}/NpDeformableSurfaceMaterial.cpp
        ${PX_SOURCE_DIR}/NpDeformableVolumeMaterial.cpp
        ${PX_SOURCE_DIR}/NpPBDMaterial.cpp
        ${PX_SOURCE_DIR}/NpArticulationReducedCoordinate.cpp
        ${PX_SOURCE_DIR}/NpArticulationJointReducedCoordinate.cpp
        ${PX_SOURCE_DIR}/NpArticulationLink.cpp
        ${PX_SOURCE_DIR}/NpArticulationTendon.cpp
        ${PX_SOURCE_DIR}/NpArticulationMimicJoint.cpp
        ${PX_SOURCE_DIR}/NpActor.cpp
        ${PX_SOURCE_DIR}/NpAggregate.cpp
        ${PX_SOURCE_DIR}/NpDeformableAttachment.cpp
        ${PX_SOURCE_DIR}/NpDeformableElementFilter.cpp
        ${PX_SOURCE_DIR}/NpDeformableSurface.cpp
        ${PX_SOURCE_DIR}/NpDeformableVolume.cpp
        ${PX_SOURCE_DIR}/NpPBDParticleSystem.cpp
        ${PX_SOURCE_DIR}/NpParticleBuffer.cpp
        ${PX_SOURCE_DIR}/NpConstraint.cpp
        ${PX_SOURCE_DIR}/NpFactory.cpp
        ${PX_SOURCE_DIR}/NpPhysics.cpp
        ${PX_SOURCE_DIR}/NpBounds.cpp
        ${PX_SOURCE_DIR}/NpPruningStructure.cpp
        ${PX_SOURCE_DIR}/NpCheck.cpp
        ${PX_SOURCE_DIR}/NpRigidDynamic.cpp
        ${PX_SOURCE_DIR}/NpRigidStatic.cpp
        ${PX_SOURCE_DIR}/NpScene.cpp
        ${PX_SOURCE_DIR}/NpSceneFetchResults.cpp
        ${PX_SOURCE_DIR}/NpSceneQueries.cpp
        ${PX_SOURCE_DIR}/NpSerializerAdapter.cpp
        ${PX_SOURCE_DIR}/NpShape.cpp
        ${PX_SOURCE_DIR}/NpShapeManager.cpp
        ${PX_SOURCE_DIR}/NpDebugViz.cpp
        ${PX_SOURCE_DIR}/NpDirectGPUAPI.cpp
        ${CHARACTERKINEMATICS_DIR}/CctBoxController.cpp
        ${CHARACTERKINEMATICS_DIR}/CctCapsuleController.cpp
        ${CHARACTERKINEMATICS_DIR}/CctCharacterController.cpp
        ${CHARACTERKINEMATICS_DIR}/CctCharacterControllerCallbacks.cpp
        ${CHARACTERKINEMATICS_DIR}/CctCharacterControllerManager.cpp
        ${CHARACTERKINEMATICS_DIR}/CctController.cpp
        ${CHARACTERKINEMATICS_DIR}/CctObstacleContext.cpp
        ${CHARACTERKINEMATICS_DIR}/CctSweptBox.cpp
        ${CHARACTERKINEMATICS_DIR}/CctSweptCapsule.cpp
        ${CHARACTERKINEMATICS_DIR}/CctSweptVolume.cpp
        ${COMMON_SRC_DIR}/CmPtrTable.cpp
        ${COMMON_SRC_DIR}/CmCollection.cpp
        ${COMMON_SRC_DIR}/CmRadixSort.cpp
        ${COMMON_SRC_DIR}/CmSerialize.cpp
        ${COMMON_SRC_DIR}/CmVisualization.cpp
        ${GU_SOURCE_DIR}/src/GuBox.cpp
        ${GU_SOURCE_DIR}/src/GuCapsule.cpp
        ${GU_SOURCE_DIR}/src/GuCCTSweepTests.cpp
        ${GU_SOURCE_DIR}/src/GuGeometryQuery.cpp
        ${GU_SOURCE_DIR}/src/GuInternal.cpp
        ${GU_SOURCE_DIR}/src/GuMeshFactory.cpp
        ${GU_SOURCE_DIR}/src/GuMTD.cpp
        ${GU_SOURCE_DIR}/src/GuOverlapTests.cpp
        ${GU_SOURCE_DIR}/src/GuRaycastTests.cpp
        ${GU_SOURCE_DIR}/src/GuSweepMTD.cpp
        ${GU_SOURCE_DIR}/src/GuSweepSharedTests.cpp
        ${GU_SOURCE_DIR}/src/GuSweepTests.cpp
        ${GU_SOURCE_DIR}/src/GuSDF.cpp
        ${GU_SOURCE_DIR}/src/GuGjkQuery.cpp
        ${GU_SOURCE_DIR}/src/GuWindingNumber.cpp
        ${GU_SOURCE_DIR}/src/GuConvexGeometry.cpp
        ${GU_SOURCE_DIR}/src/GuConvexSupport.cpp
        ${GU_SOURCE_DIR}/src/ccd/GuCCDSweepConvexMesh.cpp
        ${GU_SOURCE_DIR}/src/ccd/GuCCDSweepPrimitives.cpp
        ${GU_SOURCE_DIR}/src/common/GuBarycentricCoordinates.cpp
        ${GU_SOURCE_DIR}/src/common/GuAdjacencies.cpp
        ${GU_SOURCE_DIR}/src/common/GuEdgeList.cpp
        ${GU_SOURCE_DIR}/src/common/GuSeparatingAxes.cpp
        ${GU_SOURCE_DIR}/src/common/GuQuantizer.cpp
        ${GU_SOURCE_DIR}/src/common/GuMeshCleaner.cpp
        ${GU_SOURCE_DIR}/src/common/GuVertexReducer.cpp
        ${GU_SOURCE_DIR}/src/common/GuMeshAnalysis.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactBoxBox.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactCapsuleBox.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactCapsuleCapsule.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactCapsuleConvex.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactCapsuleMesh.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactConvexConvex.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactConvexMesh.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactConvexCoreMesh.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactConvexCoreConvex.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactPlaneConvexCore.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactPlaneBox.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactPlaneCapsule.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactPlaneConvex.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactPlaneMesh.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactPolygonPolygon.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactMeshMesh.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactSphereBox.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactSphereCapsule.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactSphereMesh.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactSpherePlane.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactSphereSphere.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactCustomGeometry.cpp
        ${GU_SOURCE_DIR}/src/contact/GuFeatureCode.cpp
        ${GU_SOURCE_DIR}/src/convex/GuBigConvexData.cpp
        ${GU_SOURCE_DIR}/src/convex/GuConvexHelper.cpp
        ${GU_SOURCE_DIR}/src/convex/GuConvexMesh.cpp
        ${GU_SOURCE_DIR}/src/convex/GuConvexSupportTable.cpp
        ${GU_SOURCE_DIR}/src/convex/GuConvexUtilsInternal.cpp
        ${GU_SOURCE_DIR}/src/convex/GuHillClimbing.cpp
        ${GU_SOURCE_DIR}/src/convex/GuShapeConvex.cpp
        ${GU_SOURCE_DIR}/src/distance/GuDistancePointBox.cpp
        ${GU_SOURCE_DIR}/src/distance/GuDistancePointTriangle.cpp
        ${GU_SOURCE_DIR}/src/distance/GuDistanceSegmentBox.cpp
        ${GU_SOURCE_DIR}/src/distance/GuDistanceSegmentSegment.cpp
        ${GU_SOURCE_DIR}/src/distance/GuDistanceSegmentTriangle.cpp
        ${GU_SOURCE_DIR}/src/distance/GuDistanceTriangleTriangle.cpp
        ${GU_SOURCE_DIR}/src/distance/GuDistancePointTetrahedron.cpp
        ${GU_SOURCE_DIR}/src/gjk/GuEPA.cpp
        ${GU_SOURCE_DIR}/src/gjk/GuGJKSimplex.cpp
        ${GU_SOURCE_DIR}/src/gjk/GuGJKTest.cpp
        ${GU_SOURCE_DIR}/src/hf/GuHeightField.cpp
        ${GU_SOURCE_DIR}/src/hf/GuHeightFieldUtil.cpp
        ${GU_SOURCE_DIR}/src/hf/GuOverlapTestsHF.cpp
        ${GU_SOURCE_DIR}/src/hf/GuSweepsHF.cpp
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionBoxBox.cpp
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionCapsuleTriangle.cpp
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionEdgeEdge.cpp
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionRayBox.cpp
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionRayCapsule.cpp
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionRaySphere.cpp
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionSphereBox.cpp
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionTetrahedronBox.cpp
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionTriangleBox.cpp
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionTriangleTriangle.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_AABBSweep.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_BoxOverlap.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_CapsuleSweep.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_CapsuleSweepAA.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_MeshMeshOverlap.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_OBBSweep.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_Raycast.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_SphereOverlap.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_SphereSweep.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuMidphaseBV4.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuBV4.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuBV4Build.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuMeshQuery.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuMidphaseRTree.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuOverlapTestsMesh.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuRTree.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuRTreeQueries.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuSweepsMesh.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuTriangleMesh.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuTriangleMeshBV4.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuTriangleMeshRTree.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuBV32.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuBV32Build.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuTetrahedronMesh.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuTetrahedronMeshUtils.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactBoxBox.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactBoxConvex.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactCapsuleBox.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactCapsuleCapsule.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactCapsuleConvex.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactCapsuleHeightField.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactCapsuleMesh.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactConvexCommon.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactConvexConvex.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactConvexHeightField.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactConvexMesh.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactGenBoxConvex.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactGenSphereCapsule.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactPlaneBox.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactPlaneCapsule.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactPlaneConvex.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactSphereBox.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactSphereCapsule.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactSphereConvex.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactSphereHeightField.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactSphereMesh.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactSpherePlane.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactSphereSphere.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactCustomGeometry.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMShapeConvex.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMTriangleContactGen.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPersistentContactManifold.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactGenUtil.cpp
        ${GU_SOURCE_DIR}/src/sweep/GuSweepBoxBox.cpp
        ${GU_SOURCE_DIR}/src/sweep/GuSweepBoxSphere.cpp
        ${GU_SOURCE_DIR}/src/sweep/GuSweepBoxTriangle_FeatureBased.cpp
        ${GU_SOURCE_DIR}/src/sweep/GuSweepBoxTriangle_SAT.cpp
        ${GU_SOURCE_DIR}/src/sweep/GuSweepCapsuleBox.cpp
        ${GU_SOURCE_DIR}/src/sweep/GuSweepCapsuleCapsule.cpp
        ${GU_SOURCE_DIR}/src/sweep/GuSweepCapsuleTriangle.cpp
        ${GU_SOURCE_DIR}/src/sweep/GuSweepSphereCapsule.cpp
        ${GU_SOURCE_DIR}/src/sweep/GuSweepSphereSphere.cpp
        ${GU_SOURCE_DIR}/src/sweep/GuSweepSphereTriangle.cpp
        ${GU_SOURCE_DIR}/src/sweep/GuSweepTriangleUtils.cpp
        ${GU_SOURCE_DIR}/src/GuBounds.cpp
        ${GU_SOURCE_DIR}/src/GuAABBTree.cpp
        ${GU_SOURCE_DIR}/src/GuAABBTreeUpdateMap.cpp
        ${GU_SOURCE_DIR}/src/GuSqInternal.cpp
        ${GU_SOURCE_DIR}/src/GuIncrementalAABBTree.cpp
        ${GU_SOURCE_DIR}/src/GuSAH.cpp
        ${GU_SOURCE_DIR}/src/GuBVH.cpp
        ${GU_SOURCE_DIR}/src/GuIncrementalAABBPrunerCore.cpp
        ${GU_SOURCE_DIR}/src/GuIncrementalAABBPruner.cpp
        ${GU_SOURCE_DIR}/src/GuPruningPool.cpp
        ${GU_SOURCE_DIR}/src/GuBucketPruner.cpp
        ${GU_SOURCE_DIR}/src/GuMaverickNode.cpp
        ${GU_SOURCE_DIR}/src/GuExtendedBucketPruner.cpp
        ${GU_SOURCE_DIR}/src/GuSecondaryPruner.cpp
        ${GU_SOURCE_DIR}/src/GuAABBPruner.cpp
        ${GU_SOURCE_DIR}/src/GuActorShapeMap.cpp
        ${GU_SOURCE_DIR}/src/GuQuerySystem.cpp
        ${GU_SOURCE_DIR}/src/GuFactory.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuRTreeCooking.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingBVH.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingHF.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexMesh.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingTriangleMesh.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingTetrahedronMesh.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingVolumeIntegration.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingQuickHullConvexHullLib.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexPolygonsBuilder.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexMeshBuilder.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexHullUtils.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexHullLib.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexHullBuilder.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingBigConvexDataBuilder.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingSDF.cpp
        ${COOKING_DIR}/Cooking.cpp
        ${SERIALIZATION_DIR}/serialization/SnSerialization.cpp
        ${SERIALIZATION_DIR}/serialization/SnSerializationRegistry.cpp
        ${SERIALIZATION_DIR}/serialization/SnSerialUtils.cpp
        ${SERIALIZATION_DIR}/serialization/Xml/SnJointRepXSerializer.cpp
        ${SERIALIZATION_DIR}/serialization/Xml/SnRepXCoreSerializer.cpp
        ${SERIALIZATION_DIR}/serialization/Xml/SnRepXUpgrader.cpp
        ${SERIALIZATION_DIR}/serialization/Xml/SnXmlSerialization.cpp
        ${SERIALIZATION_DIR}/serialization/Xml/PsFastXml.cpp
        ${SERIALIZATION_DIR}/serialization/Binary/SnBinaryDeserialization.cpp
        ${SERIALIZATION_DIR}/serialization/Binary/SnBinarySerialization.cpp
        ${SERIALIZATION_DIR}/serialization/Binary/SnSerializationContext.cpp
        ${TASK_DIR}/src/TaskManager.cpp
        ${VEHICLE_DIR}/commands/VhCommandHelpers.cpp
        ${VEHICLE_DIR}/drivetrain/VhDrivetrainFunctions.cpp
        ${VEHICLE_DIR}/drivetrain/VhDrivetrainHelpers.cpp
        ${VEHICLE_DIR}/physxActor/VhPhysXActorFunctions.cpp
        ${VEHICLE_DIR}/physxActor/VhPhysXActorHelpers.cpp
        ${VEHICLE_DIR}/physxConstraints/VhPhysXConstraintFunctions.cpp
        ${VEHICLE_DIR}/physxConstraints/VhPhysXConstraintHelpers.cpp
        ${VEHICLE_DIR}/physxRoadGeometry/VhPhysXRoadGeometryFunctions.cpp
        ${VEHICLE_DIR}/physxRoadGeometry/VhPhysXRoadGeometryHelpers.cpp
        ${VEHICLE_DIR}/rigidBody/VhRigidBodyFunctions.cpp
        ${VEHICLE_DIR}/steering/VhSteeringFunctions.cpp
        ${VEHICLE_DIR}/suspension/VhSuspensionFunctions.cpp
        ${VEHICLE_DIR}/suspension/VhSuspensionHelpers.cpp
        ${VEHICLE_DIR}/tire/VhTireFunctions.cpp
        ${VEHICLE_DIR}/tire/VhTireHelpers.cpp
        ${VEHICLE_DIR}/wheel/VhWheelFunctions.cpp
        ${VEHICLE_DIR}/pvd/VhPvdHelpers.cpp
        ${VEHICLE_DIR}/pvd/VhPvdFunctions.cpp
        ${VEHICLE_DIR}/pvd/VhPvdWriter.cpp
        ${SCENEQUERY_BASE_DIR}/src/SqFactory.cpp
        ${SCENEQUERY_BASE_DIR}/src/SqCompoundPruner.cpp
        ${SCENEQUERY_BASE_DIR}/src/SqCompoundPruningPool.cpp
        ${SCENEQUERY_BASE_DIR}/src/SqManager.cpp
        ${SCENEQUERY_BASE_DIR}/src/SqQuery.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScActorCore.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScActorSim.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScGpuActorSim.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationCore.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationJointCore.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationTendonCore.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationJointSim.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationSim.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationTendonJointCore.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationTendonSim.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScArticulationMimicJointSim.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScBroadphase.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScDeformableSurfaceCore.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScDeformableSurfaceSim.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScDeformableVolumeCore.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScDeformableVolumeSim.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScFiltering.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScParticleSystemCore.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScParticleSystemSim.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScParticleSystemShapeCore.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScBodyCore.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScBodySim.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScConstraintBreakage.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScConstraintCore.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScConstraintInteraction.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScConstraintSim.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScElementInteractionMarker.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScElementSim.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScInteraction.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScIterators.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScKinematics.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScNPhaseCore.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScPhysics.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScRigidCore.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScRigidSim.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScScene.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScCCD.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScShapeCore.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScShapeInteraction.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScShapeSim.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScShapeSimBase.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScSimStats.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScSimulationController.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScSqBoundsManager.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScStaticCore.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScTriggerInteraction.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScVisualize.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScSleep.cpp
        ${SIMULATIONCONTROLLER_BASE_DIR}/src/ScPipeline.cpp
   )

    SET(PHYSX_EXTENSIONS_SOURCE
            ${EXTENSIONS_DIR}/ExtBroadPhase.cpp
            ${EXTENSIONS_DIR}/ExtCollection.cpp
            ${EXTENSIONS_DIR}/ExtConvexMeshExt.cpp
            ${EXTENSIONS_DIR}/ExtCpuWorkerThread.cpp
            ${EXTENSIONS_DIR}/ExtDefaultCpuDispatcher.cpp
            ${EXTENSIONS_DIR}/ExtDefaultErrorCallback.cpp
            ${EXTENSIONS_DIR}/ExtDefaultProfiler.cpp
            ${EXTENSIONS_DIR}/ExtDefaultSimulationFilterShader.cpp
            ${EXTENSIONS_DIR}/ExtDefaultStreams.cpp
            ${EXTENSIONS_DIR}/ExtExtensions.cpp
            ${EXTENSIONS_DIR}/ExtPvd.cpp
            ${EXTENSIONS_DIR}/ExtPxStringTable.cpp
            ${EXTENSIONS_DIR}/ExtRaycastCCD.cpp
            ${EXTENSIONS_DIR}/ExtRigidBodyExt.cpp
            ${EXTENSIONS_DIR}/ExtRigidActorExt.cpp
            ${EXTENSIONS_DIR}/ExtSceneQueryExt.cpp
            ${EXTENSIONS_DIR}/ExtSceneQuerySystem.cpp
            ${EXTENSIONS_DIR}/ExtCustomSceneQuerySystem.cpp
            ${EXTENSIONS_DIR}/ExtSqQuery.cpp
            ${EXTENSIONS_DIR}/ExtSqManager.cpp
            ${EXTENSIONS_DIR}/ExtSimpleFactory.cpp
            ${EXTENSIONS_DIR}/ExtSmoothNormals.cpp
            ${EXTENSIONS_DIR}/ExtDeformableSurfaceExt.cpp
            ${EXTENSIONS_DIR}/ExtDeformableVolumeExt.cpp
            ${EXTENSIONS_DIR}/ExtTriangleMeshExt.cpp
            ${EXTENSIONS_DIR}/ExtTetrahedronMeshExt.cpp
            ${EXTENSIONS_DIR}/ExtRemeshingExt.cpp
            ${EXTENSIONS_DIR}/ExtSampling.cpp
            ${EXTENSIONS_DIR}/ExtTetMakerExt.cpp
            ${EXTENSIONS_DIR}/ExtGjkQueryExt.cpp
            ${EXTENSIONS_DIR}/ExtCustomGeometryExt.cpp
            ${EXTENSIONS_DIR}/ExtConvexCoreExt.cpp
    )

    #TODO, create a propper define for whether GPU features are enabled or not!
    if (PX_GENERATE_GPU_PROJECTS AND (NOT (CMAKE_CROSSCOMPILING OR CMAKE_GENERATOR_PLATFORM STREQUAL "NX64")))
            LIST(APPEND PHYSX_EXTENSIONS_SOURCE "${EXTENSIONS_DIR}/ExtDeformableSkinning.cpp")
            LIST(APPEND PHYSX_EXTENSIONS_SOURCE "${EXTENSIONS_DIR}/ExtParticleExt.cpp")
            LIST(APPEND PHYSX_EXTENSIONS_SOURCE "${EXTENSIONS_DIR}/ExtParticleClothCooker.cpp")
    ENDIF()

    SET(PHYSX_EXTENSIONS_JOINTS_SOURCE
            ${EXTENSIONS_DIR}/ExtGearJoint.cpp
            ${EXTENSIONS_DIR}/ExtRackAndPinionJoint.cpp
            ${EXTENSIONS_DIR}/ExtD6Joint.cpp
            ${EXTENSIONS_DIR}/ExtD6JointCreate.cpp
            ${EXTENSIONS_DIR}/ExtDistanceJoint.cpp
            ${EXTENSIONS_DIR}/ExtFixedJoint.cpp
            ${EXTENSIONS_DIR}/ExtJoint.cpp
            ${EXTENSIONS_DIR}/ExtPrismaticJoint.cpp
            ${EXTENSIONS_DIR}/ExtRevoluteJoint.cpp
            ${EXTENSIONS_DIR}/ExtSphericalJoint.cpp
    )

    SET(PHYSX_EXTENSIONS_TET_SOURCE
            ${EXTENSIONS_DIR}/tet/ExtUtilities.cpp
            ${EXTENSIONS_DIR}/tet/ExtTetUnionFind.cpp
            ${EXTENSIONS_DIR}/tet/ExtDelaunayBoundaryInserter.cpp
            ${EXTENSIONS_DIR}/tet/ExtDelaunayTetrahedralizer.cpp
            ${EXTENSIONS_DIR}/tet/ExtTetSplitting.cpp
            ${EXTENSIONS_DIR}/tet/ExtFastWindingNumber.cpp
            ${EXTENSIONS_DIR}/tet/ExtMeshSimplificator.cpp
            ${EXTENSIONS_DIR}/tet/ExtBVH.cpp
            ${EXTENSIONS_DIR}/tet/ExtRemesher.cpp
            ${EXTENSIONS_DIR}/tet/ExtInsideTester.cpp
            ${EXTENSIONS_DIR}/tet/ExtOctreeTetrahedralizer.cpp
            ${EXTENSIONS_DIR}/tet/ExtVoxelTetrahedralizer.cpp
    )

    SET(PHYSX_EXTENSIONS_METADATA_SOURCE
            ${PHYSX_SOURCE_DIR}/physxmetadata/extensions/src/PxExtensionAutoGeneratedMetaDataObjects.cpp
    )

    SET(PHYSX_EXTENSIONS_OMNIPVD_SOURCE
        ${EXTENSIONS_DIR}/omnipvd/ExtOmniPvdRegistrationData.cpp
        ${EXTENSIONS_DIR}/omnipvd/OmniPvdPxExtensionsSampler.cpp
    )

    set(PROJECT_SOURCES ${PROJECT_SOURCES} ${PHYSX_EXTENSIONS_SOURCE} ${PHYSX_EXTENSIONS_JOINTS_SOURCE} ${PHYSX_EXTENSIONS_TET_SOURCE} ${PHYSX_EXTENSIONS_METADATA_SOURCE} ${PHYSX_EXTENSIONS_OMNIPVD_SOURCE})

endif()

list(REMOVE_DUPLICATES PROJECT_SOURCES)
list(REMOVE_DUPLICATES INCLUDE_DIRECTORIES)

add_executable(gaitsym5
    ${PROJECT_SOURCES}
)

add_executable(gaitsym5_asio
    ${PROJECT_SOURCES}
)

# add_executable(gaitsym5_asio_mpi
#     ${PROJECT_SOURCES}
# )

target_include_directories(gaitsym5 PRIVATE
    ${INCLUDE_DIRECTORIES}
)

target_include_directories(gaitsym5_asio PRIVATE
    ${INCLUDE_DIRECTORIES}
)

# target_include_directories(gaitsym5_asio_mpi PRIVATE
#     ${INCLUDE_DIRECTORIES}
# )

target_compile_definitions(gaitsym5 PRIVATE USE_CL)
target_compile_definitions(gaitsym5_asio PRIVATE USE_ASIO_ASYNC)
# target_compile_definitions(gaitsym5_asio_mpi PRIVATE USE_ASIO_ASYNC_MPI)

if (CMAKE_BUILD_TYPE STREQUAL Release)
#    target_compile_options(gaitsym5 PRIVATE -static -O3 -ffast-math -fno-finite-math-only)
#    target_compile_options(gaitsym5_asio PRIVATE -static -O3 -ffast-math -fno-finite-math-only)
    # -static here only matters when STATIC_BUILD is on (see the option
    # declared near the top of this file)
    if (STATIC_BUILD)
        target_compile_options(gaitsym5 PRIVATE -static -O3)
        target_compile_options(gaitsym5_asio PRIVATE -static -O3)
    else()
        target_compile_options(gaitsym5 PRIVATE -O3)
        target_compile_options(gaitsym5_asio PRIVATE -O3)
    endif()
#    target_compile_options(gaitsym5_asio_mpi PRIVATE -static -O3)
endif()

if (NOT use_mucojo_src)
    if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL Release)
        target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../MuJoCo/mujoco-3.3.0/build/lib/Release/mujoco.lib")
    else()
            message(FATAL_ERROR "Unix mucojo release library location needs to be defined in CMakeLists.txt")
    endif()
    if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL Debug)
        target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../MuJoCo/mujoco-3.3.0/build/lib/Debug/mujoco.lib")
    else()
            message(FATAL_ERROR "Unix mucojo debug library location needs to be defined in CMakeLists.txt")
    endif()
endif()

if (NOT use_physx_src)
    if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL Release)
        target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0/physx/bin/win.x86_64.vc143.md/release/PhysX_64.lib")
        target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0/physx/bin/win.x86_64.vc143.md/release/PhysXFoundation_64.lib")
        target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0/physx/bin/win.x86_64.vc143.md/release/PhysXCommon_64.lib")
        target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0/physx/bin/win.x86_64.vc143.md/release/PhysXCooking_64.lib")
        target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0/physx/bin/win.x86_64.vc143.md/release/PhysXExtensions_static_64.lib")
        target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0/physx/bin/win.x86_64.vc143.md/release/PhysXPvdSDK_static_64.lib")
    else()
            message(FATAL_ERROR "Unix physx release library location needs to be defined in CMakeLists.txt")
    endif()
    if(WIN32 MAKE_BUILD_TYPE STREQUAL Debug)
        target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0/physx/bin/win.x86_64.vc143.md/debug/PhysX_64.lib")
        target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0/physx/bin/win.x86_64.vc143.md/debug/PhysXFoundation_64.lib")
        target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0/physx/bin/win.x86_64.vc143.md/debug/PhysXCommon_64.lib")
        target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0/physx/bin/win.x86_64.vc143.md/debug/PhysXCooking_64.lib")
        target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0/physx/bin/win.x86_64.vc143.md/debug/PhysXExtensions_static_64.lib")
        target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0/physx/bin/win.x86_64.vc143.md/debug/PhysXPvdSDK_static_64.lib")
    else()
            message(FATAL_ERROR "Unix physx debug library location needs to be defined in CMakeLists.txt")
    endif()
endif()

if (UNIX AND use_physx_src)
    SET_SOURCE_FILES_PROPERTIES(${PX_SOURCE_DIR}/omnipvd/OmniPvdPxSampler.cpp PROPERTIES COMPILE_FLAGS "-fpermissive -Wno-error")
    SET_SOURCE_FILES_PROPERTIES(${LL_SOURCE_DIR}/omnipvd/OmniPvdPxExtensionsSampler.cpp PROPERTIES COMPILE_FLAGS "-fpermissive -Wno-error")
endif()

cmake_host_system_information(RESULT HOST_NAME QUERY HOSTNAME)
message("HOST_NAME variable is set to: ${HOST_NAME}")
# -static below only applies when STATIC_BUILD is on (see the option
# declared near the top of this file). The csf3 branch is unaffected by
# STATIC_BUILD - that cluster's toolchain can't produce static binaries.
if (HOST_NAME STREQUAL "submitter.itservices.manchester.ac.uk")
    message("Use: 'cmake -D CMAKE_BUILD_TYPE=Release ../terminal/ -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++'")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    if (STATIC_BUILD)
        target_link_libraries(gaitsym5 PRIVATE -static Threads::Threads dl)
        target_link_libraries(gaitsym5_asio PRIVATE -static Threads::Threads dl)
#        target_link_libraries(gaitsym5_asio_mpi PRIVATE -static Threads::Threads dl)
    else()
        target_link_libraries(gaitsym5 PRIVATE Threads::Threads dl)
        target_link_libraries(gaitsym5_asio PRIVATE Threads::Threads dl)
#        target_link_libraries(gaitsym5_asio_mpi PRIVATE Threads::Threads dl)
    endif()
elseif (HOST_NAME MATCHES ".*csf3.man.alces.network$") # cannot build static executables on csf3 but they are not needed
    target_link_libraries(gaitsym5 PRIVATE )
    target_link_libraries(gaitsym5_asio PRIVATE )
#    target_link_libraries(gaitsym5_asio_mpi PRIVATE )
else()
    if (STATIC_BUILD)
        target_link_libraries(gaitsym5 PRIVATE -static)
        target_link_libraries(gaitsym5_asio PRIVATE -static)
#        target_link_libraries(gaitsym5_asio_mpi PRIVATE -static)
    else()
        target_link_libraries(gaitsym5 PRIVATE )
        target_link_libraries(gaitsym5_asio PRIVATE )
#        target_link_libraries(gaitsym5_asio_mpi PRIVATE )
    endif()
endif()


#install(TARGETS gaitsym5 gaitsym5_asio gaitsym5_asio_mpi
install(TARGETS gaitsym5 gaitsym5_asio 
    DESTINATION ${PROJECT_SOURCE_DIR}/../bin
)

function(dump_cmake_variables)
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        if (ARGV0)
            unset(MATCHED)
            string(REGEX MATCH ${ARGV0} MATCHED ${_variableName})
            if (NOT MATCHED)
                continue()
            endif()
        endif()
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endfunction()

# handy variable dump to General Messages window on QtCreator
dump_cmake_variables()

