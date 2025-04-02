# run "cmake -D CMAKE_BUILD_TYPE=Release ../terminal/" for a release version
# run "cmake -D CMAKE_BUILD_TYPE=Debug ../terminal/" for a debug version

cmake_minimum_required(VERSION 3.5)

# decide whether to use libraries or source code
set(use_mucojo_src 1)
set(use_physx_src 1)

# this command stops the CMAKE_BUILD_TYPE from being cached
SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)

project(gaitsym VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# customise the behaviour
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # generates detailed compile_commands.json file
if(CMAKE_BUILD_TYPE STREQUAL Release)
    add_compile_definitions(QT_NO_DEBUG_OUTPUT) # stops qDebug from generating any output
    message(STATUS "QT_NO_DEBUG_OUTPUT defined")
endif()

# MuJoCo definitions
add_compile_definitions(MC_CPP_USE_DOUBLE_PRECISION MC_IMPLEM_ENABLE MJ_STATIC CCD_STATIC_DEFINE)

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
    ../ode-0.15/libccd/src/vec3.c    ../ode-0.15/OPCODE/Ice/IceAABB.cpp
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

    set(PHYSX_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0/physx)
    #set(PHYSX_ROOT_DIR ../PhysX/PhysX-107.0/physx)
    if (UNIX)
        set(TARGET_BUILD_PLATFORM linux)
    endif()
    if (WIN32)
        set(TARGET_BUILD_PLATFORM windows)
    endif()
    set(PROJECT_CMAKE_FILES_DIR ../../custom_cmake) # these have been copied from physx/source/compiler/cmake
    set(PX_GENERATE_STATIC_LIBRARIES 1)
    set(PX_FLOAT_POINT_PRECISE_MATH 1)

    include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/PhysXFoundation.cmake)
    include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/LowLevel.cmake)
    include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/LowLevelAABB.cmake)
    include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/LowLevelDynamics.cmake)
    include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/PhysX.cmake)
    include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/PhysXCharacterKinematic.cmake)
    include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/PhysXCommon.cmake)
    include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/PhysXCooking.cmake)
    include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/PhysXExtensions.cmake)
    include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/PhysXPvdSDK.cmake)
    include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/PhysXTask.cmake)
    include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/PhysXVehicle.cmake)
    include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/SceneQuery.cmake)
    include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/SimulationController.cmake)

endif()

list(REMOVE_DUPLICATES PROJECT_SOURCES)
list(REMOVE_DUPLICATES INCLUDE_DIRECTORIES)

add_executable(gaitsym5
    ${PROJECT_SOURCES}
)

add_executable(gaitsym5_asio
    ${PROJECT_SOURCES}
)

target_include_directories(gaitsym5 PRIVATE
    ${INCLUDE_DIRECTORIES}
)

target_include_directories(gaitsym5_asio PRIVATE
    ${INCLUDE_DIRECTORIES}
)

target_compile_definitions(gaitsym5 PRIVATE USE_CL)
target_compile_definitions(gaitsym5_asio PRIVATE USE_ASIO_ASYNC)

if (CMAKE_BUILD_TYPE STREQUAL Release)
#    target_compile_options(gaitsym5 PRIVATE -static -O3 -ffast-math -fno-finite-math-only)
#    target_compile_options(gaitsym5_asio PRIVATE -static -O3 -ffast-math -fno-finite-math-only)
    target_compile_options(gaitsym5 PRIVATE -static -O3)
    target_compile_options(gaitsym5_asio PRIVATE -static -O3)
endif()

if(use_physx_src)
    set(LINK_LIBS
        ${CMAKE_CURRENT_BINARY_DIR}/libPhysXCharacterKinematic_static.a
        ${CMAKE_CURRENT_BINARY_DIR}/libPhysXCommon_static.a
        ${CMAKE_CURRENT_BINARY_DIR}/libPhysXCooking_static.a
        ${CMAKE_CURRENT_BINARY_DIR}/libPhysXExtensions_static.a
        ${CMAKE_CURRENT_BINARY_DIR}/libPhysXFoundation_static.a
        ${CMAKE_CURRENT_BINARY_DIR}/libPhysXPvdSDK_static.a
        ${CMAKE_CURRENT_BINARY_DIR}/libPhysXVehicle2_static.a
        ${CMAKE_CURRENT_BINARY_DIR}/libPhysX_static.a
    )
    target_link_libraries(gaitsym5 PRIVATE "$<LINK_GROUP:RESCAN,${LINK_LIBS}>") # the rescan option effectively adds --start-group/--end-group for a GNU compiler
    target_link_libraries(gaitsym5_asio PRIVATE "$<LINK_GROUP:RESCAN,${LINK_LIBS}>") # the rescan option effectively adds --start-group/--end-group for a GNU compiler
endif()

cmake_host_system_information(RESULT HOST_NAME QUERY HOSTNAME)
message("HOST_NAME variable is set to: ${HOST_NAME}")
if (HOST_NAME STREQUAL "submitter.itservices.manchester.ac.uk")
    message("Use: 'cmake -D CMAKE_BUILD_TYPE=Release ../terminal/ -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++'")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(gaitsym5 PRIVATE -static Threads::Threads dl)
    target_link_libraries(gaitsym5_asio PRIVATE -static Threads::Threads dl)
else()
    target_link_libraries(gaitsym5 PRIVATE -static)
    target_link_libraries(gaitsym5_asio PRIVATE -static)
endif()


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

