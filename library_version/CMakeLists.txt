cmake_minimum_required(VERSION 3.5)

project(GaitSym5 VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(PHYSX_BUILD_TYPE "release") # The build type of PhysX, i.e., one of {debug, checked, profile, release}

if(CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Building program in debug configuration")
    add_compile_definitions(_DEBUG)
    set(PHYSX_BUILD_TYPE "debug")
    link_directories("win.x86_64.vc143.mt/debug") # This is the path where PhysX libraries are installed
else()
    message("Building program in release configuration with PhysX ${PHYSX_BUILD_TYPE} configuration")
    add_compile_definitions(NDEBUG)
    link_directories("win.x86_64.vc143.mt/${PHYSX_BUILD_TYPE}") # This is the path where PhysX libraries are installed
endif()

# MuJoCo definitions
add_compile_definitions(MC_CPP_USE_DOUBLE_PRECISION MC_IMPLEM_ENABLE MJ_STATIC)

# ODE definitions
add_compile_definitions(dIDEDOUBLE dTRIMESH_ENABLED=1 dTRIMESH_OPCODE=1 dTRIMESH_GIMPACT=0 dTRIMESH_16BIT_INDICES=0
    CCD_IDEDOUBLE dLIBCCD_ENABLED=1 dTHREADING_INTF_DISABLED=1 dTRIMESH_OPCODE_USE_OLD_TRIMESH_TRIMESH_COLLIDER=0
    ICE_NO_DLL dTLS_ENABLED=0 USE_UNIX_ERRORS)
if (MSVC)
    add_compile_definitions(HAVE_MALLOC_H)
endif()

# gle definitions
add_compile_definitions(DEBUG_OUTPUT)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)

set(PROJECT_SOURCES
    ../GaitSymQt/AVIWriter.cpp
    ../GaitSymQt/AboutDialog.cpp
    ../GaitSymQt/AboutDialog.ui
    ../GaitSymQt/BasicXMLSyntaxHighlighter.cpp
    ../GaitSymQt/DialogAssembly.cpp
    ../GaitSymQt/DialogAssembly.ui
    ../GaitSymQt/DialogBodyBuilder.cpp
    ../GaitSymQt/DialogBodyBuilder.ui
    ../GaitSymQt/DialogCreateMirrorElements.cpp
    ../GaitSymQt/DialogCreateMirrorElements.ui
    ../GaitSymQt/DialogCreateTestingDrivers.cpp
    ../GaitSymQt/DialogCreateTestingDrivers.ui
    ../GaitSymQt/DialogDrivers.cpp
    ../GaitSymQt/DialogDrivers.ui
    ../GaitSymQt/DialogGeoms.cpp
    ../GaitSymQt/DialogGeoms.ui
    ../GaitSymQt/DialogGlobal.cpp
    ../GaitSymQt/DialogGlobal.ui
    ../GaitSymQt/DialogInfo.cpp
    ../GaitSymQt/DialogInfo.ui
    ../GaitSymQt/DialogJoints.cpp
    ../GaitSymQt/DialogJoints.ui
    ../GaitSymQt/DialogMarkerImportExport.cpp
    ../GaitSymQt/DialogMarkerImportExport.ui
    ../GaitSymQt/DialogMarkers.cpp
    ../GaitSymQt/DialogMarkers.ui
    ../GaitSymQt/DialogMuscles.cpp
    ../GaitSymQt/DialogMuscles.ui
    ../GaitSymQt/DialogOutputSelect.cpp
    ../GaitSymQt/DialogOutputSelect.ui
    ../GaitSymQt/DialogPreferences.cpp
    ../GaitSymQt/DialogPreferences.ui
    ../GaitSymQt/DialogProperties.cpp
    ../GaitSymQt/DialogProperties.ui
    ../GaitSymQt/DialogRename.cpp
    ../GaitSymQt/DialogRename.ui
    ../GaitSymQt/DoubleValidator.cpp
    ../GaitSymQt/DrawBody.cpp
    ../GaitSymQt/DrawCustom.cpp
    ../GaitSymQt/DrawFluidSac.cpp
    ../GaitSymQt/DrawGeom.cpp
    ../GaitSymQt/DrawJoint.cpp
    ../GaitSymQt/DrawMarker.cpp
    ../GaitSymQt/DrawMuscle.cpp
    ../GaitSymQt/Drawable.cpp
    ../GaitSymQt/ElementTreeWidget.cpp
    ../GaitSymQt/FacetedAxes.cpp
    ../GaitSymQt/FacetedBox.cpp
    ../GaitSymQt/FacetedCappedCylinder.cpp
    ../GaitSymQt/FacetedCheckerboard.cpp
    ../GaitSymQt/FacetedConicSegment.cpp
    ../GaitSymQt/FacetedObject.cpp
    ../GaitSymQt/FacetedPolyline.cpp
    ../GaitSymQt/FacetedRect.cpp
    ../GaitSymQt/FacetedSphere.cpp
    ../GaitSymQt/GLUtils.cpp
    ../GaitSymQt/IntersectionHits.cpp
    ../GaitSymQt/LineEditDouble.cpp
    ../GaitSymQt/LineEditPath.cpp
    ../GaitSymQt/LineEditUniqueName.cpp
    ../GaitSymQt/MainWindow.cpp
    ../GaitSymQt/MainWindow.ui
    ../GaitSymQt/MeshStore.cpp
    ../GaitSymQt/Preferences.cpp
    ../GaitSymQt/SimulationWidget.cpp
    ../GaitSymQt/StrokeFont.cpp
    ../GaitSymQt/TextEditDialog.cpp
    ../GaitSymQt/TextEditDialog.ui
    ../GaitSymQt/TrackBall.cpp
    ../GaitSymQt/UniqueNameValidator.cpp
    ../GaitSymQt/ViewControlWidget.cpp
    ../GaitSymQt/main.cpp

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
    ../glextrusion/ex_angle.c
    ../glextrusion/ex_cut_round.c
    ../glextrusion/ex_raw.c
    ../glextrusion/extrude.c
    ../glextrusion/intersect.c
    ../glextrusion/qmesh.c
    ../glextrusion/rot_prince.c
    ../glextrusion/rotate.c
    ../glextrusion/round_cap.c
    ../glextrusion/segment.c
    ../glextrusion/texgen.c
    ../glextrusion/urotate.c
    ../glextrusion/view.c
    ../libgwavi/avi-utils.c
    ../libgwavi/fileio.c
    ../libgwavi/gwavi.c

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
    ../src/LMotorJoint.cpp
    ../src/MAMuscle.cpp
    ../src/MAMuscleComplete.cpp
    ../src/Marker.cpp
    ../src/MarkerEllipseDriver.cpp
    ../src/MarkerPositionDriver.cpp
    ../src/MovingAverage.cpp
    ../src/Muscle.cpp
    ../src/NamedObject.cpp
    ../src/NPointStrap.cpp
    ../src/ObjectiveMain.cpp
    ../src/ODEPhysicsEngine.cpp
    ../src/ParseXML.cpp
    ../src/PGDMath.cpp
    ../src/PhysicsEngine.cpp
    ../src/PhysXPhysicsEngine.cpp
    ../src/PIDErrorInController.cpp
    ../src/PIDMuscleLengthController.cpp
    ../src/PlaneGeom.cpp
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

    ../fast_double_parser/fast_double_parser.h
    ../GaitSymQt/AboutDialog.h
    ../GaitSymQt/AVIWriter.h
    ../GaitSymQt/BasicXMLSyntaxHighlighter.h
    ../GaitSymQt/DialogAssembly.h
    ../GaitSymQt/DialogBodyBuilder.h
    ../GaitSymQt/DialogCreateMirrorElements.h
    ../GaitSymQt/DialogCreateTestingDrivers.h
    ../GaitSymQt/DialogDrivers.h
    ../GaitSymQt/DialogGeoms.h
    ../GaitSymQt/DialogGlobal.h
    ../GaitSymQt/DialogInfo.h
    ../GaitSymQt/DialogJoints.h
    ../GaitSymQt/DialogMarkerImportExport.h
    ../GaitSymQt/DialogMarkers.h
    ../GaitSymQt/DialogMuscles.h
    ../GaitSymQt/DialogOutputSelect.h
    ../GaitSymQt/DialogPreferences.h
    ../GaitSymQt/DialogProperties.h
    ../GaitSymQt/DialogRename.h
    ../GaitSymQt/DoubleValidator.h
    ../GaitSymQt/Drawable.h
    ../GaitSymQt/DrawBody.h
    ../GaitSymQt/DrawCustom.h
    ../GaitSymQt/DrawFluidSac.h
    ../GaitSymQt/DrawGeom.h
    ../GaitSymQt/DrawJoint.h
    ../GaitSymQt/DrawMarker.h
    ../GaitSymQt/DrawMuscle.h
    ../GaitSymQt/ElementTreeWidget.h
    ../GaitSymQt/FacetedAxes.h
    ../GaitSymQt/FacetedBox.h
    ../GaitSymQt/FacetedCappedCylinder.h
    ../GaitSymQt/FacetedCheckerboard.h
    ../GaitSymQt/FacetedConicSegment.h
    ../GaitSymQt/FacetedObject.h
    ../GaitSymQt/FacetedPolyline.h
    ../GaitSymQt/FacetedRect.h
    ../GaitSymQt/FacetedSphere.h
    ../GaitSymQt/GLUtils.h
    ../GaitSymQt/IntersectionHits.h
    ../GaitSymQt/LineEditDouble.h
    ../GaitSymQt/LineEditPath.h
    ../GaitSymQt/LineEditUniqueName.h
    ../GaitSymQt/MainWindow.h
    ../GaitSymQt/MeshStore.h
    ../GaitSymQt/Preferences.h
    ../GaitSymQt/SimulationWidget.h
    ../GaitSymQt/StrokeFont.h
    ../GaitSymQt/TextEditDialog.h
    ../GaitSymQt/TrackBall.h
    ../GaitSymQt/UniqueNameValidator.h
    ../GaitSymQt/ViewControlWidget.h
    ../glextrusion/copy.h
    ../glextrusion/extrude.h
    ../glextrusion/gle.h
    ../glextrusion/intersect.h
    ../glextrusion/port.h
    ../glextrusion/rot.h
    ../glextrusion/segment.h
    ../glextrusion/tube_gc.h
    ../glextrusion/vvector.h
    ../libgwavi/avi-utils.h
    ../libgwavi/fileio.h
    ../libgwavi/gwavi.h
    ../libgwavi/gwavi_private.h
    ../pocketpy/pocketpy.h
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
    ../src/LMotorJoint.h
    ../src/MAMuscle.h
    ../src/MAMuscleComplete.h
    ../src/Marker.h
    ../src/MarkerEllipseDriver.h
    ../src/MarkerPositionDriver.h
    ../src/MovingAverage.h
    ../src/MPIStuff.h
    ../src/Muscle.h
    ../src/NamedObject.h
    ../src/NPointStrap.h
    ../src/ObjectiveMain.h
    ../src/ODEPhysicsEngine.h
    ../src/ParseXML.h
    ../src/PhysicsEngine.h
    ../src/PhysXPhysicsEngine.h
    ../src/PGDMath.h
    ../src/PIDErrorInController.h
    ../src/PIDMuscleLengthController.h
    ../src/PlaneGeom.h
    ../src/Reporter.h
    ../src/SimpleStrap.h
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

    ../rapidxml/rapidxml.hpp
    ../rapidxml/rapidxml_iterators.hpp
    ../rapidxml/rapidxml_print.hpp
    ../rapidxml/rapidxml_utils.hpp

    ../GaitSymQt/app.rc
    ../GaitSymQt/resources.qrc
    ../GaitSymQt/AboutDialog.ui
    ../GaitSymQt/DialogAssembly.ui
    ../GaitSymQt/DialogBodyBuilder.ui
    ../GaitSymQt/DialogCreateMirrorElements.ui
    ../GaitSymQt/DialogCreateTestingDrivers.ui
    ../GaitSymQt/DialogDrivers.ui
    ../GaitSymQt/DialogGeoms.ui
    ../GaitSymQt/DialogGlobal.ui
    ../GaitSymQt/DialogInfo.ui
    ../GaitSymQt/DialogJoints.ui
    ../GaitSymQt/DialogMarkerImportExport.ui
    ../GaitSymQt/DialogMarkers.ui
    ../GaitSymQt/DialogMuscles.ui
    ../GaitSymQt/DialogOutputSelect.ui
    ../GaitSymQt/DialogPreferences.ui
    ../GaitSymQt/DialogProperties.ui
    ../GaitSymQt/DialogRename.ui
    ../GaitSymQt/MainWindow.ui
    ../GaitSymQt/TextEditDialog.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GaitSym5
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET GaitSym5 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(GaitSym5 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(GaitSym5
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(GaitSym5 PRIVATE
    ../fast_double_parser
    ../GaitSymQt
    ../glextrusion
    ../libgwavi
    ../PhysX/PhysX-105.1-physx-5.3.0/physx/include
    ../pocketpy
    ../pystring
    ../rapidxml
    ../src
    ../tinyply
    ../ode-0.15/OPCODE
    ../ode-0.15/include
    ../ode-0.15/ode/src
    ../ode-0.15/libccd/src/
)

target_link_libraries(GaitSym5 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(GaitSym5 PRIVATE Qt6::OpenGL)
target_link_libraries(GaitSym5 PRIVATE Qt6::OpenGLWidgets)

target_link_libraries(GaitSym5 PRIVATE
    PhysXExtensions_static_64
    PhysX_static_64
    PhysXPvdSDK_static_64
    PhysXCommon_static_64
    PhysXFoundation_static_64
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.GaitSym5)
endif()
set_target_properties(GaitSym5 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS GaitSym5
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GaitSym5)
endif()

if (MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_definitions(_USE_MATH_DEFINES)
endif ()

find_package(Qt6 REQUIRED COMPONENTS Xml)
target_link_libraries(GaitSym5 PRIVATE Qt6::Xml)

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
