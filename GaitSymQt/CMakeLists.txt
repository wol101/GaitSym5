cmake_minimum_required(VERSION 3.5)

# decide whether to use libraries or source code
set(use_mucojo_src 1)
set(use_physx_src 1)

# set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_OSX_ARCHITECTURES x86_64) # this works better set in the project options
# set(_CMAKE_APPLE_ARCHS_DEFAULT x86_64)

project(GaitSym5 VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

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

# MuJoCo definitions
if (use_mucojo_src)
    add_compile_definitions(MC_CPP_USE_DOUBLE_PRECISION MC_IMPLEM_ENABLE MJ_STATIC CCD_STATIC_DEFINE)
endif()

# physx definitions
if (use_physx_src)
    # options
    # PX_SUPPORT_GPU_PHYSX
    # PX_SUPPORT_PVD
    # PX_ENABLE_FEATURES_UNDER_CONSTRUCTION
    # PX_PHYSX_STATIC_LIB [turns off a lot of DLL specific code]
    # DISABLE_CUDA_PHYSX
    add_compile_definitions(PX_PHYSX_STATIC_LIB)

    # build style (do not define any for release code)
    # PX_DEBUG    [optimisation off, SDK development asserts, very intrusive]
    # PX_CHECKED  [code to detect invalid parameters, API race conditions, and other incorrect uses of the API]
    # PX_PROFILE  [omits the checks, but still has PVD and memory instrumentation]
    if(CMAKE_BUILD_TYPE STREQUAL Debug)
        add_compile_definitions(PX_CHECKED)
        message(STATUS "PX_CHECKED defined")
        add_compile_definitions(PX_SUPPORT_PVD)
        message(STATUS "PX_SUPPORT_PVD defined")
    endif()
    if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL arm64)
        add_compile_definitions(PX_SIMD_DISABLED)
        message(STATUS "PX_SIMD_DISABLED defined")
    endif()
endif()

# glextrusion definitions
add_compile_definitions(TRIANGLE_GENERATOR __GLE_DOUBLE)

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
    ../GaitSymQt/DialogConvertFile.cpp
    ../GaitSymQt/DialogConvertFile.ui
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
    ../GaitSymQt/DialogMarkerExport.cpp
    ../GaitSymQt/DialogMarkerExport.ui
    ../GaitSymQt/DialogMarkerImport.cpp
    ../GaitSymQt/DialogMarkerImport.ui
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
    ../GaitSymQt/DrawDataTarget.cpp
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
    ../GaitSymQt/FacetedPolyCone.cpp
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

    ../threepp/src/threepp/cameras/Camera.cpp
    ../threepp/src/threepp/cameras/PerspectiveCamera.cpp
    ../threepp/src/threepp/cameras/OrthographicCamera.cpp
    ../threepp/src/threepp/controls/FlyControls.cpp
    ../threepp/src/threepp/controls/OrbitControls.cpp
    ../threepp/src/threepp/core/BufferGeometry.cpp
    ../threepp/src/threepp/core/Clock.cpp
    ../threepp/src/threepp/core/EventDispatcher.cpp
    ../threepp/src/threepp/core/Layers.cpp
    ../threepp/src/threepp/core/Object3D.cpp
    ../threepp/src/threepp/core/Raycaster.cpp
    ../threepp/src/threepp/core/Uniform.cpp
    ../threepp/src/threepp/extras/ShapeUtils.cpp
    ../threepp/src/threepp/extras/core/Curve.cpp
    ../threepp/src/threepp/extras/core/CurvePath.cpp
    ../threepp/src/threepp/extras/core/Font.cpp
    ../threepp/src/threepp/extras/core/Path.cpp
    ../threepp/src/threepp/extras/core/Shape.cpp
    ../threepp/src/threepp/extras/core/ShapePath.cpp
    ../threepp/src/threepp/extras/curves/CatmullRomCurve3.cpp
    ../threepp/src/threepp/extras/curves/CubicBezierCurve.cpp
    ../threepp/src/threepp/extras/curves/CubicBezierCurve3.cpp
    ../threepp/src/threepp/extras/curves/EllipseCurve.cpp
    ../threepp/src/threepp/extras/curves/LineCurve.cpp
    ../threepp/src/threepp/extras/curves/QuadraticBezierCurve.cpp
    ../threepp/src/threepp/extras/curves/QuadraticBezierCurve3.cpp
    ../threepp/src/threepp/extras/curves/SplineCurve.cpp
    ../threepp/src/threepp/geometries/BoxGeometry.cpp
    ../threepp/src/threepp/geometries/CapsuleGeometry.cpp
    ../threepp/src/threepp/geometries/CircleGeometry.cpp
    ../threepp/src/threepp/geometries/ConeGeometry.cpp
    ../threepp/src/threepp/geometries/ConvexGeometry.cpp
    ../threepp/src/threepp/geometries/CylinderGeometry.cpp
    ../threepp/src/threepp/geometries/DecalGeometry.cpp
    ../threepp/src/threepp/geometries/EdgesGeometry.cpp
    ../threepp/src/threepp/geometries/ExtrudeGeometry.cpp
    ../threepp/src/threepp/geometries/IcosahedronGeometry.cpp
    ../threepp/src/threepp/geometries/LatheGeometry.cpp
    ../threepp/src/threepp/geometries/OctahedronGeometry.cpp
    ../threepp/src/threepp/geometries/PlaneGeometry.cpp
    ../threepp/src/threepp/geometries/PolyhedronGeometry.cpp
    ../threepp/src/threepp/geometries/RingGeometry.cpp
    ../threepp/src/threepp/geometries/ShapeGeometry.cpp
    ../threepp/src/threepp/geometries/SphereGeometry.cpp
    ../threepp/src/threepp/geometries/TextGeometry.cpp
    ../threepp/src/threepp/geometries/TorusGeometry.cpp
    ../threepp/src/threepp/geometries/TorusKnotGeometry.cpp
    ../threepp/src/threepp/geometries/TubeGeometry.cpp
    ../threepp/src/threepp/geometries/WireframeGeometry.cpp
    ../threepp/src/threepp/helpers/ArrowHelper.cpp
    ../threepp/src/threepp/helpers/AxesHelper.cpp
    ../threepp/src/threepp/helpers/Box3Helper.cpp
    ../threepp/src/threepp/helpers/BoxHelper.cpp
    ../threepp/src/threepp/helpers/CameraHelper.cpp
    ../threepp/src/threepp/helpers/DirectionalLightHelper.cpp
    ../threepp/src/threepp/helpers/GridHelper.cpp
    ../threepp/src/threepp/helpers/HemisphereLightHelper.cpp
    ../threepp/src/threepp/helpers/PlaneHelper.cpp
    ../threepp/src/threepp/helpers/PointLightHelper.cpp
    ../threepp/src/threepp/helpers/PolarGridHelper.cpp
    ../threepp/src/threepp/helpers/SkeletonHelper.cpp
    ../threepp/src/threepp/helpers/SpotLightHelper.cpp
    ../threepp/src/threepp/input/PeripheralsEventSource.cpp
    ../threepp/src/threepp/loaders/FontLoader.cpp
    ../threepp/src/threepp/loaders/ImageLoader.cpp
    ../threepp/src/threepp/loaders/MTLLoader.cpp
    ../threepp/src/threepp/loaders/OBJLoader.cpp
    ../threepp/src/threepp/loaders/STLLoader.cpp
    ../threepp/src/threepp/loaders/TextureLoader.cpp
    ../threepp/src/threepp/materials/LineBasicMaterial.cpp
    ../threepp/src/threepp/materials/Material.cpp
    ../threepp/src/threepp/materials/MeshBasicMaterial.cpp
    ../threepp/src/threepp/materials/MeshLambertMaterial.cpp
    ../threepp/src/threepp/materials/MeshNormalMaterial.cpp
    ../threepp/src/threepp/materials/MeshPhongMaterial.cpp
    ../threepp/src/threepp/materials/MeshStandardMaterial.cpp
    ../threepp/src/threepp/materials/PointsMaterial.cpp
    ../threepp/src/threepp/materials/RawShaderMaterial.cpp
    ../threepp/src/threepp/materials/ShaderMaterial.cpp
    ../threepp/src/threepp/materials/SpriteMaterial.cpp
    ../threepp/src/threepp/math/Box2.cpp
    ../threepp/src/threepp/math/Box3.cpp
    ../threepp/src/threepp/math/Capsule.cpp
    ../threepp/src/threepp/math/Color.cpp
    ../threepp/src/threepp/math/Cylindrical.cpp
    ../threepp/src/threepp/math/Euler.cpp
    ../threepp/src/threepp/math/Frustum.cpp
    ../threepp/src/threepp/math/ImprovedNoise.cpp
    ../threepp/src/threepp/math/Line3.cpp
    ../threepp/src/threepp/math/MathUtils.cpp
    ../threepp/src/threepp/math/Matrix3.cpp
    ../threepp/src/threepp/math/Matrix4.cpp
    ../threepp/src/threepp/math/Plane.cpp
    ../threepp/src/threepp/math/Ray.cpp
    ../threepp/src/threepp/math/Sphere.cpp
    ../threepp/src/threepp/math/Spherical.cpp
    ../threepp/src/threepp/math/SphericalHarmonics3.cpp
    ../threepp/src/threepp/math/Triangle.cpp
    ../threepp/src/threepp/math/Vector2.cpp
    ../threepp/src/threepp/math/Vector3.cpp
    ../threepp/src/threepp/math/Vector4.cpp
    ../threepp/src/threepp/math/Quaternion.cpp
    ../threepp/src/threepp/lights/AmbientLight.cpp
    ../threepp/src/threepp/lights/DirectionalLight.cpp
    ../threepp/src/threepp/lights/HemisphereLight.cpp
    ../threepp/src/threepp/lights/Light.cpp
    ../threepp/src/threepp/lights/LightShadow.cpp
    ../threepp/src/threepp/lights/PointLight.cpp
    ../threepp/src/threepp/lights/PointLightShadow.cpp
    ../threepp/src/threepp/lights/SpotLight.cpp
    ../threepp/src/threepp/lights/SpotLightShadow.cpp
    ../threepp/src/threepp/scenes/Scene.cpp
    ../threepp/src/threepp/scenes/Fog.cpp
    ../threepp/src/threepp/scenes/FogExp2.cpp
    ../threepp/src/threepp/objects/Group.cpp
    ../threepp/src/threepp/objects/HUD.cpp
    ../threepp/src/threepp/objects/Line.cpp
    ../threepp/src/threepp/objects/LineLoop.cpp
    ../threepp/src/threepp/objects/LineSegments.cpp
    ../threepp/src/threepp/objects/LOD.cpp
    ../threepp/src/threepp/objects/InstancedMesh.cpp
    ../threepp/src/threepp/objects/Mesh.cpp
    ../threepp/src/threepp/objects/ObjectWithMaterials.cpp
    ../threepp/src/threepp/objects/ParticleSystem.cpp
    ../threepp/src/threepp/objects/Points.cpp
    ../threepp/src/threepp/objects/Skeleton.cpp
    ../threepp/src/threepp/objects/SkinnedMesh.cpp
    ../threepp/src/threepp/objects/Sky.cpp
    ../threepp/src/threepp/objects/Sprite.cpp
    ../threepp/src/threepp/objects/Reflector.cpp
    ../threepp/src/threepp/objects/Water.cpp
    ../threepp/src/threepp/textures/Texture.cpp
    ../threepp/src/threepp/textures/DataTexture3D.cpp
    ../threepp/src/threepp/utils/BufferGeometryUtils.cpp
    ../threepp/src/threepp/utils/StringUtils.cpp
    ../threepp/src/threepp/renderers/GLRenderer.cpp
    ../threepp/src/threepp/renderers/GLRenderTarget.cpp
    ../threepp/src/threepp/renderers/gl/GLAttributes.cpp
    ../threepp/src/threepp/renderers/gl/GLBackground.cpp
    ../threepp/src/threepp/renderers/gl/GLBindingStates.cpp
    ../threepp/src/threepp/renderers/gl/GLBufferRenderer.cpp
    ../threepp/src/threepp/renderers/gl/GLClipping.cpp
    ../threepp/src/threepp/renderers/gl/GLCubeMaps.cpp
    ../threepp/src/threepp/renderers/gl/GLGeometries.cpp
    ../threepp/src/threepp/renderers/gl/GLInfo.cpp
    ../threepp/src/threepp/renderers/gl/GLLights.cpp
    ../threepp/src/threepp/renderers/gl/GLObjects.cpp
    ../threepp/src/threepp/renderers/gl/GLProgram.cpp
    ../threepp/src/threepp/renderers/gl/GLPrograms.cpp
    ../threepp/src/threepp/renderers/gl/GLMaterials.cpp
    ../threepp/src/threepp/renderers/gl/GLRenderLists.cpp
    ../threepp/src/threepp/renderers/gl/GLRenderStates.cpp
    ../threepp/src/threepp/renderers/gl/GLShadowMap.cpp
    ../threepp/src/threepp/renderers/gl/GLState.cpp
    ../threepp/src/threepp/renderers/gl/GLTextures.cpp
    ../threepp/src/threepp/renderers/gl/GLUniforms.cpp
    ../threepp/src/threepp/renderers/gl/ProgramParameters.cpp
    ../threepp/src/threepp/renderers/shaders/ShaderLib.cpp
    ../threepp/src/external/glad/glad.c
    ../threepp/src/threepp/utils/LoadGlad.cpp
    ../threepp/generated/threepp/EmbeddedFonts.cpp
    ../threepp/generated/threepp/renderers/shaders/ShaderChunk.cpp

    ../pystring/pystring.cpp

    ../glextrusion/GLEmulator.cpp
    ../glextrusion/extrude.cpp
    ../glextrusion/ex_alpha.cpp
    ../glextrusion/ex_angle.cpp
    ../glextrusion/ex_cut_round.cpp
    ../glextrusion/ex_raw.cpp
    ../glextrusion/intersect.cpp
    ../glextrusion/qmesh.cpp
    ../glextrusion/rotate.cpp
    ../glextrusion/rot_prince.cpp
    ../glextrusion/round_cap.cpp
    ../glextrusion/segment.cpp
    ../glextrusion/texgen.cpp
    ../glextrusion/urotate.cpp
    ../glextrusion/view.cpp

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
    ../GaitSymQt/AboutDialog.h
    ../GaitSymQt/AVIWriter.h
    ../GaitSymQt/BasicXMLSyntaxHighlighter.h
    ../GaitSymQt/DialogAssembly.h
    ../GaitSymQt/DialogBodyBuilder.h
    ../GaitSymQt/DialogConvertFile.h
    ../GaitSymQt/DialogCreateMirrorElements.h
    ../GaitSymQt/DialogCreateTestingDrivers.h
    ../GaitSymQt/DialogDrivers.h
    ../GaitSymQt/DialogGeoms.h
    ../GaitSymQt/DialogGlobal.h
    ../GaitSymQt/DialogInfo.h
    ../GaitSymQt/DialogJoints.h
    ../GaitSymQt/DialogMarkerExport.h
    ../GaitSymQt/DialogMarkerImport.h
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
    ../GaitSymQt/DrawDataTarget.h
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
    ../GaitSymQt/FacetedPolyCone.h
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

    ../glextrusion/GLEmulator.h
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

    ../pocketpy-2.0.4/pocketpy.h

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

    ../rapidxml/rapidxml.hpp
    ../rapidxml/rapidxml_iterators.hpp
    ../rapidxml/rapidxml_print.hpp
    ../rapidxml/rapidxml_utils.hpp

    ../GaitSymQt/app.rc
    ../GaitSymQt/resources.qrc
)

set(INCLUDE_DIRECTORIES
    ../fast_double_parser
    ../GaitSymQt
    ../glextrusion
    ../libgwavi
    ../PhysX
    ../pocketpy-2.0.4
    ../pystring
    ../rapidxml
    ../src
    ../tinyply
    ../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/include
    ../MuJoCo/mujoco-3.3.0/include
    ../ode-0.15/OPCODE
    ../ode-0.15/include
    ../ode-0.15/libccd/src/
    ../ode-0.15/ode/src
    ../threepp/include
    ../threepp/src
    ../threepp/src/external/quickhull/
    ../threepp/src/external/glad
    ../threepp/src/external/nlohmann
    ../threepp/src/external/stb
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
    SET(PHYSX_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx)
    SET(PHYSX_SOURCE_DIR ${PHYSX_ROOT_DIR}/source)
    SET(PX_SOURCE_DIR ${PHYSX_SOURCE_DIR}/physx/src)
    SET(MD_SOURCE_DIR ${PHYSX_SOURCE_DIR}/physxmetadata)

    if (UNIX)
        SET(PHYSX_GPU_HEADERS
            ${PHYSX_ROOT_DIR}/include/gpu/PxGpu.h
            ${PHYSX_ROOT_DIR}/include/gpu/PxPhysicsGpu.h
        )

        SET(PHYSX_CUDACONTEXT_MANAGER_GPU_HEADERS
                ${PHYSX_ROOT_DIR}/include/cudamanager/PxCudaContextManager.h
                ${PHYSX_ROOT_DIR}/include/cudamanager/PxCudaContext.h
                ${PHYSX_ROOT_DIR}/include/cudamanager/PxCudaTypes.h
        )

        SET(PHYSX_PLATFORM_INCLUDES
        )

        SET(PHYSX_PLATFORM_OBJECT_FILES
            ${PHYSX_SOURCE_DIR}/foundation/unix/FdUnixAtomic.cpp
            ${PHYSX_SOURCE_DIR}/foundation/unix/FdUnixFPU.cpp
            ${PHYSX_SOURCE_DIR}/foundation/unix/FdUnixMutex.cpp
            ${PHYSX_SOURCE_DIR}/foundation/unix/FdUnixPrintString.cpp
            ${PHYSX_SOURCE_DIR}/foundation/unix/FdUnixSList.cpp
            ${PHYSX_SOURCE_DIR}/foundation/unix/FdUnixSocket.cpp
            ${PHYSX_SOURCE_DIR}/foundation/unix/FdUnixSync.cpp
            ${PHYSX_SOURCE_DIR}/foundation/unix/FdUnixThread.cpp
            ${PHYSX_SOURCE_DIR}/foundation/unix/FdUnixTime.cpp
        )

        SET(PHYSX_PLATFORM_SRC_FILES
            ${PHYSX_GPU_HEADERS}
            ${PHYSX_CUDACONTEXT_MANAGER_GPU_HEADERS}
            ${PX_SOURCE_DIR}/gpu/PxGpu.cpp
            ${PX_SOURCE_DIR}/gpu/PxPhysXGpuModuleLoader.cpp

            ${PHYSX_PLATFORM_OBJECT_FILES}
        )
    endif()

    if (WIN32)
        SET(PHYSX_PLATFORM_INCLUDES
        )

        SET(PHYSX_GPU_HEADERS
            ${PHYSX_ROOT_DIR}/include/gpu/PxGpu.h
            ${PHYSX_ROOT_DIR}/include/gpu/PxPhysicsGpu.h
        )

        SET(PHYSX_CUDACONTEXT_MANAGER_GPU_HEADERS
            ${PHYSX_ROOT_DIR}/include/cudamanager/PxCudaContextManager.h
            ${PHYSX_ROOT_DIR}/include/cudamanager/PxCudaContext.h
            ${PHYSX_ROOT_DIR}/include/cudamanager/PxCudaTypes.h
        )

        SET(PHYSX_COMMON_WINDOWS_HEADERS
            ${PHYSX_ROOT_DIR}/include/common/windows/PxWindowsDelayLoadHook.h
        )

        SET(PHYSX_GPU_SOURCE
            ${PX_SOURCE_DIR}/gpu/PxGpu.cpp
            ${PX_SOURCE_DIR}/gpu/PxPhysXGpuModuleLoader.cpp
        )

        SET(PHYSX_WINDOWS_SOURCE
            ${PX_SOURCE_DIR}/windows/NpWindowsDelayLoadHook.cpp
        )

        SET(PHYSX_PLATFORM_OBJECT_FILES
            ${PHYSX_SOURCE_DIR}/foundation/windows/FdWindowsAtomic.cpp
            ${PHYSX_SOURCE_DIR}/foundation/windows/FdWindowsFPU.cpp
            ${PHYSX_SOURCE_DIR}/foundation/windows/FdWindowsMutex.cpp
            ${PHYSX_SOURCE_DIR}/foundation/windows/FdWindowsPrintString.cpp
            ${PHYSX_SOURCE_DIR}/foundation/windows/FdWindowsSList.cpp
            ${PHYSX_SOURCE_DIR}/foundation/windows/FdWindowsSocket.cpp
            ${PHYSX_SOURCE_DIR}/foundation/windows/FdWindowsSync.cpp
            ${PHYSX_SOURCE_DIR}/foundation/windows/FdWindowsThread.cpp
            ${PHYSX_SOURCE_DIR}/foundation/windows/FdWindowsTime.cpp
        )

        SET(PHYSX_PLATFORM_SRC_FILES
            ${PHYSX_GPU_HEADERS}
            ${PHYSX_CUDACONTEXT_MANAGER_GPU_HEADERS}
            ${PHYSX_COMMON_WINDOWS_HEADERS}
            ${PHYSX_GPU_SOURCE}
            ${PHYSX_WINDOWS_SOURCE}
            ${PHYSX_PLATFORM_OBJECT_FILES}
        )
    endif()

    SET(PHYSX_HEADERS
        ${PHYSX_ROOT_DIR}/include/PxActor.h
        ${PHYSX_ROOT_DIR}/include/PxAggregate.h
        ${PHYSX_ROOT_DIR}/include/PxArticulationFlag.h
        ${PHYSX_ROOT_DIR}/include/PxArticulationJointReducedCoordinate.h
        ${PHYSX_ROOT_DIR}/include/PxArticulationLink.h
        ${PHYSX_ROOT_DIR}/include/PxArticulationReducedCoordinate.h
        ${PHYSX_ROOT_DIR}/include/PxArticulationTendon.h
        ${PHYSX_ROOT_DIR}/include/PxArticulationTendonData.h
        ${PHYSX_ROOT_DIR}/include/PxArticulationMimicJoint.h
        ${PHYSX_ROOT_DIR}/include/PxBroadPhase.h
        ${PHYSX_ROOT_DIR}/include/PxClient.h
        ${PHYSX_ROOT_DIR}/include/PxConeLimitedConstraint.h
        ${PHYSX_ROOT_DIR}/include/PxConstraint.h
        ${PHYSX_ROOT_DIR}/include/PxConstraintDesc.h
        ${PHYSX_ROOT_DIR}/include/PxContact.h
        ${PHYSX_ROOT_DIR}/include/PxContactModifyCallback.h
        ${PHYSX_ROOT_DIR}/include/PxDeformableAttachment.h
        ${PHYSX_ROOT_DIR}/include/PxDeformableElementFilter.h
        ${PHYSX_ROOT_DIR}/include/PxDeformableBody.h
        ${PHYSX_ROOT_DIR}/include/PxDeformableBodyFlag.h
        ${PHYSX_ROOT_DIR}/include/PxDeformableSurface.h
        ${PHYSX_ROOT_DIR}/include/PxDeformableSurfaceFlag.h
        ${PHYSX_ROOT_DIR}/include/PxDeformableVolume.h
        ${PHYSX_ROOT_DIR}/include/PxDeformableVolumeFlag.h
        ${PHYSX_ROOT_DIR}/include/PxDeletionListener.h
        ${PHYSX_ROOT_DIR}/include/PxFEMParameter.h #deprecated
        ${PHYSX_ROOT_DIR}/include/PxFiltering.h
        ${PHYSX_ROOT_DIR}/include/PxForceMode.h
        ${PHYSX_ROOT_DIR}/include/PxImmediateMode.h
        ${PHYSX_ROOT_DIR}/include/PxLockedData.h
        ${PHYSX_ROOT_DIR}/include/PxNodeIndex.h
        ${PHYSX_ROOT_DIR}/include/PxParticleBuffer.h
        ${PHYSX_ROOT_DIR}/include/PxParticleGpu.h
        ${PHYSX_ROOT_DIR}/include/PxParticleSolverType.h
        ${PHYSX_ROOT_DIR}/include/PxParticleSystem.h
        ${PHYSX_ROOT_DIR}/include/PxParticleSystemFlag.h
        ${PHYSX_ROOT_DIR}/include/PxPBDParticleSystem.h
        ${PHYSX_ROOT_DIR}/include/PxPhysics.h
        ${PHYSX_ROOT_DIR}/include/PxPhysicsAPI.h
        ${PHYSX_ROOT_DIR}/include/PxPhysicsSerialization.h
        ${PHYSX_ROOT_DIR}/include/PxPhysXConfig.h
        ${PHYSX_ROOT_DIR}/include/PxPruningStructure.h
        ${PHYSX_ROOT_DIR}/include/PxQueryFiltering.h
        ${PHYSX_ROOT_DIR}/include/PxQueryReport.h
        ${PHYSX_ROOT_DIR}/include/PxRigidActor.h
        ${PHYSX_ROOT_DIR}/include/PxRigidBody.h
        ${PHYSX_ROOT_DIR}/include/PxRigidDynamic.h
        ${PHYSX_ROOT_DIR}/include/PxRigidStatic.h
        ${PHYSX_ROOT_DIR}/include/PxScene.h
        ${PHYSX_ROOT_DIR}/include/PxSceneDesc.h
        ${PHYSX_ROOT_DIR}/include/PxSceneLock.h
        ${PHYSX_ROOT_DIR}/include/PxSceneQueryDesc.h
        ${PHYSX_ROOT_DIR}/include/PxSceneQuerySystem.h
        ${PHYSX_ROOT_DIR}/include/PxShape.h
        ${PHYSX_ROOT_DIR}/include/PxSimulationEventCallback.h
        ${PHYSX_ROOT_DIR}/include/PxSimulationStatistics.h
        ${PHYSX_ROOT_DIR}/include/PxSoftBody.h #deprecated
        ${PHYSX_ROOT_DIR}/include/PxSoftBodyFlag.h #deprecated
        ${PHYSX_ROOT_DIR}/include/PxSparseGridParams.h
        ${PHYSX_ROOT_DIR}/include/PxVisualizationParameter.h
        ${PHYSX_ROOT_DIR}/include/PxIsosurfaceExtraction.h
        ${PHYSX_ROOT_DIR}/include/PxSmoothing.h
        ${PHYSX_ROOT_DIR}/include/PxAnisotropy.h
        ${PHYSX_ROOT_DIR}/include/PxParticleNeighborhoodProvider.h
        ${PHYSX_ROOT_DIR}/include/PxArrayConverter.h
        ${PHYSX_ROOT_DIR}/include/PxSDFBuilder.h
        ${PHYSX_ROOT_DIR}/include/PxResidual.h
        ${PHYSX_ROOT_DIR}/include/PxDirectGPUAPI.h
        ${PHYSX_ROOT_DIR}/include/PxDeformableSkinning.h
    )

    SET(PHYSX_MATERIAL_HEADERS
        ${PHYSX_ROOT_DIR}/include/PxBaseMaterial.h
        ${PHYSX_ROOT_DIR}/include/PxDeformableMaterial.h
        ${PHYSX_ROOT_DIR}/include/PxDeformableSurfaceMaterial.h
        ${PHYSX_ROOT_DIR}/include/PxDeformableVolumeMaterial.h
        ${PHYSX_ROOT_DIR}/include/PxFEMMaterial.h #deprecated
        ${PHYSX_ROOT_DIR}/include/PxFEMSoftBodyMaterial.h #deprecated
        ${PHYSX_ROOT_DIR}/include/PxParticleMaterial.h
        ${PHYSX_ROOT_DIR}/include/PxPBDMaterial.h
        ${PHYSX_ROOT_DIR}/include/PxMaterial.h
    )

    SET(PHYSX_COMMON_HEADERS
        ${PHYSX_ROOT_DIR}/include/common/PxBase.h
        ${PHYSX_ROOT_DIR}/include/common/PxCollection.h
        ${PHYSX_ROOT_DIR}/include/common/PxCoreUtilityTypes.h
        ${PHYSX_ROOT_DIR}/include/common/PxInsertionCallback.h
        ${PHYSX_ROOT_DIR}/include/common/PxPhysXCommonConfig.h
        ${PHYSX_ROOT_DIR}/include/common/PxProfileZone.h
        ${PHYSX_ROOT_DIR}/include/common/PxRenderBuffer.h
        ${PHYSX_ROOT_DIR}/include/common/PxRenderOutput.h
        ${PHYSX_ROOT_DIR}/include/common/PxSerialFramework.h
        ${PHYSX_ROOT_DIR}/include/common/PxSerializer.h
        ${PHYSX_ROOT_DIR}/include/common/PxStringTable.h
        ${PHYSX_ROOT_DIR}/include/common/PxTolerancesScale.h
        ${PHYSX_ROOT_DIR}/include/common/PxTypeInfo.h
    )

    SET(PHYSX_OMNIPVD_HEADERS
        ${PHYSX_ROOT_DIR}/include/omnipvd/PxOmniPvd.h
    )

    SET(PHYSX_PVD_HEADERS
        ${PHYSX_ROOT_DIR}/include/pvd/PxPvdSceneClient.h
        ${PHYSX_ROOT_DIR}/include/pvd/PxPvd.h
        ${PHYSX_ROOT_DIR}/include/pvd/PxPvdTransport.h
    )

    SET(PHYSX_COLLISION_HEADERS
        ${PHYSX_ROOT_DIR}/include/collision/PxCollisionDefs.h
    )

    SET(PHYSX_SOLVER_HEADERS
        ${PHYSX_ROOT_DIR}/include/solver/PxSolverDefs.h
    )
    SOURCE_GROUP(include\\solver FILES ${PHYSX_SOLVER_HEADERS})

    SET(PHYSX_METADATA_HEADERS
        ${MD_SOURCE_DIR}/core/include/PvdMetaDataDefineProperties.h
        ${MD_SOURCE_DIR}/core/include/PvdMetaDataExtensions.h
        ${MD_SOURCE_DIR}/core/include/PvdMetaDataPropertyVisitor.h
        ${MD_SOURCE_DIR}/core/include/PxAutoGeneratedMetaDataObjectNames.h
        ${MD_SOURCE_DIR}/core/include/PxAutoGeneratedMetaDataObjects.h
        ${MD_SOURCE_DIR}/core/include/PxMetaDataCompare.h
        ${MD_SOURCE_DIR}/core/include/PxMetaDataCppPrefix.h
        ${MD_SOURCE_DIR}/core/include/PxMetaDataObjects.h
        ${MD_SOURCE_DIR}/core/include/RepXMetaDataPropertyVisitor.h
    )

    SET(PHYSX_METADATA_SOURCE
        ${MD_SOURCE_DIR}/core/src/PxAutoGeneratedMetaDataObjects.cpp
        ${MD_SOURCE_DIR}/core/src/PxMetaDataObjects.cpp
    )

    SET(PHYSX_OMNIPVD_SOURCE
        ${PX_SOURCE_DIR}/omnipvd/NpOmniPvd.h
        ${PX_SOURCE_DIR}/omnipvd/NpOmniPvd.cpp
        ${PX_SOURCE_DIR}/omnipvd/NpOmniPvdRegistrationData.h
        ${PX_SOURCE_DIR}/omnipvd/NpOmniPvdRegistrationData.cpp
        ${PX_SOURCE_DIR}/omnipvd/NpOmniPvdSetData.h
        ${PX_SOURCE_DIR}/omnipvd/NpOmniPvdMetaData.h
        ${PX_SOURCE_DIR}/omnipvd/NpOmniPvdMetaData.cpp
        ${PX_SOURCE_DIR}/omnipvd/NpOmniPvdSimulationControllerCallbacks.h
        ${PX_SOURCE_DIR}/omnipvd/NpOmniPvdSimulationControllerCallbacks.cpp
        ${PX_SOURCE_DIR}/omnipvd/OmniPvdPxSampler.cpp
        ${PX_SOURCE_DIR}/omnipvd/OmniPvdPxSampler.h
        ${PX_SOURCE_DIR}/omnipvd/OmniPvdChunkAlloc.cpp
        ${PX_SOURCE_DIR}/omnipvd/OmniPvdChunkAlloc.h
        ${PX_SOURCE_DIR}/omnipvd/OmniPvdTypes.h
    )

    SET(PHYSX_PVD_SOURCE
        ${PX_SOURCE_DIR}/NpPvdSceneClient.cpp
        ${PX_SOURCE_DIR}/NpPvdSceneClient.h
        ${PX_SOURCE_DIR}/NpPvdSceneQueryCollector.cpp
        ${PX_SOURCE_DIR}/NpPvdSceneQueryCollector.h
        ${PX_SOURCE_DIR}/PvdMetaDataPvdBinding.cpp
        ${PX_SOURCE_DIR}/PvdPhysicsClient.cpp
        ${PX_SOURCE_DIR}/PvdMetaDataBindingData.h
        ${PX_SOURCE_DIR}/PvdMetaDataPvdBinding.h
        ${PX_SOURCE_DIR}/PvdPhysicsClient.h
        ${PX_SOURCE_DIR}/PvdTypeNames.h
    )

    SET(PHYSX_IMMEDIATEMODE_SOURCE
        ${PHYSX_ROOT_DIR}/source/immediatemode/src/NpImmediateMode.cpp
    )

    SET(PHYSX_MATERIALS_SOURCE
        ${PX_SOURCE_DIR}/NpMaterial.cpp
        ${PX_SOURCE_DIR}/NpDeformableSurfaceMaterial.cpp
        ${PX_SOURCE_DIR}/NpDeformableVolumeMaterial.cpp
        ${PX_SOURCE_DIR}/NpPBDMaterial.cpp
        ${PX_SOURCE_DIR}/NpMaterial.h
        ${PX_SOURCE_DIR}/NpDeformableSurfaceMaterial.h
        ${PX_SOURCE_DIR}/NpDeformableVolumeMaterial.h
        ${PX_SOURCE_DIR}/NpPBDMaterial.h
    )

    SET(PHYSX_ARTICULATIONS_SOURCE
        ${PX_SOURCE_DIR}/NpArticulationReducedCoordinate.cpp
        ${PX_SOURCE_DIR}/NpArticulationJointReducedCoordinate.cpp
        ${PX_SOURCE_DIR}/NpArticulationLink.cpp
        ${PX_SOURCE_DIR}/NpArticulationTendon.cpp
        ${PX_SOURCE_DIR}/NpArticulationMimicJoint.cpp
        ${PX_SOURCE_DIR}/NpArticulationReducedCoordinate.h
        ${PX_SOURCE_DIR}/NpArticulationJointReducedCoordinate.h
        ${PX_SOURCE_DIR}/NpArticulationLink.h
        ${PX_SOURCE_DIR}/NpArticulationTendon.h
        ${PX_SOURCE_DIR}/NpArticulationMimicJoint.h
    )

    SET(PHYSX_CORE_SOURCE
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
        ${PX_SOURCE_DIR}/NpBounds.h
        ${PX_SOURCE_DIR}/NpBounds.cpp
        ${PX_SOURCE_DIR}/NpPruningStructure.h
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
        ${PX_SOURCE_DIR}/NpBase.h
        ${PX_SOURCE_DIR}/NpActor.h
        ${PX_SOURCE_DIR}/NpActorTemplate.h
        ${PX_SOURCE_DIR}/NpAggregate.h
        ${PX_SOURCE_DIR}/NpDeformableSurface.h
        ${PX_SOURCE_DIR}/NpDeformableVolume.h
        ${PX_SOURCE_DIR}/NpDeformableAttachment.h
        ${PX_SOURCE_DIR}/NpDeformableElementFilter.h
        ${PX_SOURCE_DIR}/NpPBDParticleSystem.h
        ${PX_SOURCE_DIR}/NpParticleBuffer.h
        ${PX_SOURCE_DIR}/NpConnector.h
        ${PX_SOURCE_DIR}/NpConstraint.h
        ${PX_SOURCE_DIR}/NpFactory.h
        ${PX_SOURCE_DIR}/NpMaterialManager.h
        ${PX_SOURCE_DIR}/NpPhysics.h
        ${PX_SOURCE_DIR}/NpPhysicsInsertionCallback.h
        ${PX_SOURCE_DIR}/NpPtrTableStorageManager.h
        ${PX_SOURCE_DIR}/NpCheck.h
        ${PX_SOURCE_DIR}/NpRigidActorTemplate.h
        ${PX_SOURCE_DIR}/NpRigidActorTemplateInternal.h
        ${PX_SOURCE_DIR}/NpRigidBodyTemplate.h
        ${PX_SOURCE_DIR}/NpRigidDynamic.h
        ${PX_SOURCE_DIR}/NpRigidStatic.h
        ${PX_SOURCE_DIR}/NpScene.h
        ${PX_SOURCE_DIR}/NpSceneQueries.h
        ${PX_SOURCE_DIR}/NpSceneAccessor.h
        ${PX_SOURCE_DIR}/NpShape.h
        ${PX_SOURCE_DIR}/NpShapeManager.h
        ${PX_SOURCE_DIR}/NpDebugViz.h
        ${PX_SOURCE_DIR}/NpDebugViz.cpp
        ${PX_SOURCE_DIR}/NpDirectGPUAPI.h
        ${PX_SOURCE_DIR}/NpDirectGPUAPI.cpp
    )

    SET(GPUDEPENDENCIES_INCLUDES
        ${PHYSX_SOURCE_DIR}/lowlevel/common/include/pipeline/PxcNpMemBlockPool.h
        ${PHYSX_SOURCE_DIR}/lowlevel/common/include/pipeline/PxcNpCacheStreamPair.h
        ${PHYSX_SOURCE_DIR}/lowlevel/common/include/pipeline/PxcNpThreadContext.h
        ${PHYSX_SOURCE_DIR}/lowlevelaabb/include/BpFiltering.h
        ${PHYSX_ROOT_DIR}/include/common/PxRenderOutput.h
        ${PHYSX_SOURCE_DIR}/geomutils/include/GuBounds.h
        ${PHYSX_SOURCE_DIR}/geomutils/include/GuConvexSupport.h
        ${PHYSX_SOURCE_DIR}/geomutils/include/GuRefGjkEpa.h
    )

    SET(GPUDEPENDENCIES_SOURCE
        ${PHYSX_SOURCE_DIR}/lowlevel/common/src/pipeline/PxcNpMemBlockPool.cpp
        ${PHYSX_SOURCE_DIR}/lowlevel/common/src/pipeline/PxcNpThreadContext.cpp
        ${PHYSX_SOURCE_DIR}/lowlevel/common/src/pipeline/PxcNpCacheStreamPair.cpp
        ${PHYSX_SOURCE_DIR}/lowlevelaabb/src/BpFiltering.cpp
        ${PHYSX_SOURCE_DIR}/lowlevelaabb/src/BpBroadPhaseUpdate.cpp
        ${PHYSX_SOURCE_DIR}/lowlevelaabb/src/BpAABBManagerBase.cpp
        ${PHYSX_SOURCE_DIR}/geomutils/src/GuBounds.cpp
        ${PHYSX_SOURCE_DIR}/geomutils/src/convex/GuConvexUtilsInternal.cpp
        ${PHYSX_SOURCE_DIR}/geomutils/src/mesh/GuTetrahedronMeshUtils.cpp
        ${PHYSX_SOURCE_DIR}/geomutils/src/GuConvexGeometry.cpp
    )

    SET(PHYSXFOUNDATION_SOURCE
        ${PHYSX_SOURCE_DIR}/foundation/FdAllocator.cpp
        ${PHYSX_SOURCE_DIR}/foundation/FdString.cpp
        ${PHYSX_SOURCE_DIR}/foundation/FdTempAllocator.cpp
        ${PHYSX_SOURCE_DIR}/foundation/FdAssert.cpp
        ${PHYSX_SOURCE_DIR}/foundation/FdMathUtils.cpp
        ${PHYSX_SOURCE_DIR}/foundation/FdFoundation.cpp
    )

    SET(PHYSX_SOURCE_DIR ${PHYSX_ROOT_DIR}/source)
    SET(COMMON_SRC_DIR ${PHYSX_SOURCE_DIR}/common/src)
    SET(COMMON_INCLUDE_DIR ${PHYSX_SOURCE_DIR}/common/include)
    SET(GU_SOURCE_DIR ${PHYSX_SOURCE_DIR}/geomutils)

    SET(PHYSX_COMMON_SOURCE
        ${COMMON_SRC_DIR}/CmPtrTable.cpp
        ${COMMON_SRC_DIR}/CmRenderBuffer.h
        ${COMMON_SRC_DIR}/CmBlockArray.h
        ${COMMON_SRC_DIR}/CmCollection.h
        ${COMMON_SRC_DIR}/CmCollection.cpp
        ${COMMON_SRC_DIR}/CmConeLimitHelper.h
        ${COMMON_SRC_DIR}/CmFlushPool.h
        ${COMMON_SRC_DIR}/CmIDPool.h
        ${COMMON_SRC_DIR}/CmMatrix34.h
        ${COMMON_SRC_DIR}/CmPool.h
        ${COMMON_SRC_DIR}/CmPreallocatingPool.h
        ${COMMON_SRC_DIR}/CmPriorityQueue.h
        ${COMMON_SRC_DIR}/CmPtrTable.h
        ${COMMON_SRC_DIR}/CmRadixSort.h
        ${COMMON_SRC_DIR}/CmRadixSort.cpp
        ${COMMON_SRC_DIR}/CmRandom.h
        ${COMMON_SRC_DIR}/CmRefCountable.h
        ${COMMON_SRC_DIR}/CmScaling.h
        ${COMMON_SRC_DIR}/CmSerialize.h
        ${COMMON_SRC_DIR}/CmSerialize.cpp
        ${COMMON_SRC_DIR}/CmSpatialVector.h
        ${COMMON_SRC_DIR}/CmTask.h
        ${COMMON_SRC_DIR}/CmTransformUtils.h
        ${COMMON_SRC_DIR}/CmUtils.h
        ${COMMON_SRC_DIR}/CmVisualization.h
        ${COMMON_SRC_DIR}/CmVisualization.cpp
    )

    SET(PHYSXCOMMON_COMMON_INTERNAL_HEADERS
        ${COMMON_INCLUDE_DIR}/omnipvd/CmOmniPvdAutoGenClearDefines.h
        ${COMMON_INCLUDE_DIR}/omnipvd/CmOmniPvdAutoGenCreateRegistrationStruct.h
        ${COMMON_INCLUDE_DIR}/omnipvd/CmOmniPvdAutoGenRegisterData.h
        ${COMMON_INCLUDE_DIR}/omnipvd/CmOmniPvdAutoGenSetData.h
    )

    SET(PHYSXCOMMON_COMMON_HEADERS
        ${PHYSX_ROOT_DIR}/include/common/PxBase.h
        ${PHYSX_ROOT_DIR}/include/common/PxCollection.h
        ${PHYSX_ROOT_DIR}/include/common/PxCoreUtilityTypes.h
        ${PHYSX_ROOT_DIR}/include/common/PxInsertionCallback.h
        ${PHYSX_ROOT_DIR}/include/common/PxPhysXCommonConfig.h
        ${PHYSX_ROOT_DIR}/include/common/PxRenderBuffer.h
        ${PHYSX_ROOT_DIR}/include/common/PxRenderOutput.h
        ${PHYSX_ROOT_DIR}/include/common/PxSerialFramework.h
        ${PHYSX_ROOT_DIR}/include/common/PxSerializer.h
        ${PHYSX_ROOT_DIR}/include/common/PxStringTable.h
        ${PHYSX_ROOT_DIR}/include/common/PxTolerancesScale.h
        ${PHYSX_ROOT_DIR}/include/common/PxTypeInfo.h
        ${PHYSX_ROOT_DIR}/include/common/PxProfileZone.h
    )

    SET(PHYSXCOMMON_GEOMETRY_HEADERS
        ${PHYSX_ROOT_DIR}/include/geometry/PxBoxGeometry.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxCapsuleGeometry.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxConvexMesh.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxConvexMeshGeometry.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxCustomGeometry.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxConvexCoreGeometry.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxGeometry.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxGeometryInternal.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxGeometryHelpers.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxGeometryHit.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxGeometryQuery.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxGeometryQueryFlags.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxGeometryQueryContext.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxHeightField.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxHeightFieldDesc.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxHeightFieldFlag.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxHeightFieldGeometry.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxHeightFieldSample.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxMeshQuery.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxMeshScale.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxPlaneGeometry.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxReportCallback.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxSimpleTriangleMesh.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxSphereGeometry.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxTriangle.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxTriangleMesh.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxTriangleMeshGeometry.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxBVH.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxBVHBuildStrategy.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxTetrahedron.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxTetrahedronMesh.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxTetrahedronMeshGeometry.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxParticleSystemGeometry.h
        ${PHYSX_ROOT_DIR}/include/geometry/PxGjkQuery.h
    )

    SET(PHYSXCOMMON_GEOMUTILS_HEADERS
        ${PHYSX_ROOT_DIR}/include/geomutils/PxContactBuffer.h
        ${PHYSX_ROOT_DIR}/include/geomutils/PxContactPoint.h
    )

    SET(PHYSXCOMMON_COLLISION_HEADERS
        ${PHYSX_ROOT_DIR}/include/collision/PxCollisionDefs.h
    )

    SET(PHYSXCOMMON_GU_HEADERS
        ${GU_SOURCE_DIR}/include/GuBox.h
        ${GU_SOURCE_DIR}/include/GuSphere.h
        ${GU_SOURCE_DIR}/include/GuSegment.h
        ${GU_SOURCE_DIR}/include/GuCapsule.h
        ${GU_SOURCE_DIR}/include/GuCenterExtents.h
        ${GU_SOURCE_DIR}/include/GuBounds.h
        ${GU_SOURCE_DIR}/include/GuDistanceSegmentBox.h
        ${GU_SOURCE_DIR}/include/GuDistanceSegmentSegment.h
        ${GU_SOURCE_DIR}/include/GuIntersectionBoxBox.h
        ${GU_SOURCE_DIR}/include/GuIntersectionTetrahedronBox.h
        ${GU_SOURCE_DIR}/include/GuIntersectionTriangleBoxRef.h
        ${GU_SOURCE_DIR}/include/GuIntersectionTriangleTriangle.h
        ${GU_SOURCE_DIR}/include/GuRaycastTests.h
        ${GU_SOURCE_DIR}/include/GuOverlapTests.h
        ${GU_SOURCE_DIR}/include/GuSweepTests.h
        ${GU_SOURCE_DIR}/include/GuCachedFuncs.h
        ${GU_SOURCE_DIR}/include/GuPruner.h
        ${GU_SOURCE_DIR}/include/GuPrunerTypedef.h
        ${GU_SOURCE_DIR}/include/GuPrunerPayload.h
        ${GU_SOURCE_DIR}/include/GuPrunerMergeData.h
        ${GU_SOURCE_DIR}/include/GuSqInternal.h
        ${GU_SOURCE_DIR}/include/GuActorShapeMap.h
        ${GU_SOURCE_DIR}/include/GuQuerySystem.h
        ${GU_SOURCE_DIR}/include/GuFactory.h
        ${GU_SOURCE_DIR}/include/GuDistancePointTetrahedron.h
        ${GU_SOURCE_DIR}/include/GuDistancePointTriangle.h
        ${GU_SOURCE_DIR}/include/GuIntersectionTriangleBox.h
        ${GU_SOURCE_DIR}/include/GuIntersectionTetrahedronTetrahedron.h
        ${GU_SOURCE_DIR}/include/GuCooking.h
        ${GU_SOURCE_DIR}/include/GuConvexSupport.h
        ${GU_SOURCE_DIR}/include/GuConvexGeometry.h
        ${GU_SOURCE_DIR}/include/GuRefGjkEpa.h
    )

    #SET(PHYSXCOMMON_GU_PXHEADERS
    #	${PHYSX_ROOT_DIR}/include/geomutils/GuContactBuffer.h
    #	${PHYSX_ROOT_DIR}/include/geomutils/GuContactPoint.h
    #)

    SET(PHYSXCOMMON_GU_SOURCE
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
        ${GU_SOURCE_DIR}/src/GuGeometryChecks.h
        ${GU_SOURCE_DIR}/src/GuInternal.h
        ${GU_SOURCE_DIR}/src/GuMeshFactory.h
        ${GU_SOURCE_DIR}/src/GuMTD.h
        ${GU_SOURCE_DIR}/src/GuSweepMTD.h
        ${GU_SOURCE_DIR}/src/GuSweepSharedTests.h
        ${GU_SOURCE_DIR}/src/GuSDF.h
        ${GU_SOURCE_DIR}/src/GuSDF.cpp
        ${GU_SOURCE_DIR}/src/GuGjkQuery.cpp
        ${GU_SOURCE_DIR}/src/GuWindingNumber.cpp
        ${GU_SOURCE_DIR}/src/GuWindingNumber.h
        ${GU_SOURCE_DIR}/src/GuWindingNumberCluster.h
        ${GU_SOURCE_DIR}/src/GuWindingNumberT.h
        ${GU_SOURCE_DIR}/src/GuConvexGeometry.cpp
        ${GU_SOURCE_DIR}/src/GuConvexSupport.cpp
    )

    SET(PHYSXCOMMON_GU_CCD_SOURCE
            ${GU_SOURCE_DIR}/src/ccd/GuCCDSweepConvexMesh.cpp
            ${GU_SOURCE_DIR}/src/ccd/GuCCDSweepPrimitives.cpp
            ${GU_SOURCE_DIR}/src/ccd/GuCCDSweepConvexMesh.h
    )

    SET(PHYSXCOMMON_GU_COMMON_SOURCE
        ${GU_SOURCE_DIR}/src/common/GuBarycentricCoordinates.cpp
        ${GU_SOURCE_DIR}/src/common/GuBarycentricCoordinates.h
        ${GU_SOURCE_DIR}/src/common/GuBoxConversion.h
        ${GU_SOURCE_DIR}/src/common/GuEdgeCache.h
        ${GU_SOURCE_DIR}/src/common/GuAdjacencies.h
        ${GU_SOURCE_DIR}/src/common/GuAdjacencies.cpp
        ${GU_SOURCE_DIR}/src/common/GuEdgeList.h
        ${GU_SOURCE_DIR}/src/common/GuEdgeList.cpp
        ${GU_SOURCE_DIR}/src/common/GuSeparatingAxes.h
        ${GU_SOURCE_DIR}/src/common/GuSeparatingAxes.cpp
        ${GU_SOURCE_DIR}/src/common/GuQuantizer.h
        ${GU_SOURCE_DIR}/src/common/GuQuantizer.cpp
        ${GU_SOURCE_DIR}/src/common/GuMeshCleaner.h
        ${GU_SOURCE_DIR}/src/common/GuMeshCleaner.cpp
        ${GU_SOURCE_DIR}/src/common/GuVertexReducer.h
        ${GU_SOURCE_DIR}/src/common/GuVertexReducer.cpp
        ${GU_SOURCE_DIR}/src/common/GuMeshAnalysis.h
        ${GU_SOURCE_DIR}/src/common/GuMeshAnalysis.cpp
    )

    SET(PHYSXCOMMON_GU_CONTACT_SOURCE
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
        ${GU_SOURCE_DIR}/src/contact/GuContactMeshMesh.h
        ${GU_SOURCE_DIR}/src/contact/GuContactReduction.h
        ${GU_SOURCE_DIR}/src/contact/GuCollisionSDF.h
        ${GU_SOURCE_DIR}/src/contact/GuContactSphereBox.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactSphereCapsule.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactSphereMesh.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactSpherePlane.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactSphereSphere.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactCustomGeometry.cpp
        ${GU_SOURCE_DIR}/src/contact/GuFeatureCode.cpp
        ${GU_SOURCE_DIR}/src/contact/GuContactMethodImpl.h
        ${GU_SOURCE_DIR}/src/contact/GuContactPolygonPolygon.h
        ${GU_SOURCE_DIR}/src/contact/GuFeatureCode.h
    )

    SET(PHYSXCOMMON_GU_CONVEX_SOURCE
        ${GU_SOURCE_DIR}/src/convex/GuBigConvexData.cpp
        ${GU_SOURCE_DIR}/src/convex/GuConvexHelper.cpp
        ${GU_SOURCE_DIR}/src/convex/GuConvexMesh.cpp
        ${GU_SOURCE_DIR}/src/convex/GuConvexSupportTable.cpp
        ${GU_SOURCE_DIR}/src/convex/GuConvexUtilsInternal.cpp
        ${GU_SOURCE_DIR}/src/convex/GuHillClimbing.cpp
        ${GU_SOURCE_DIR}/src/convex/GuShapeConvex.cpp
        ${GU_SOURCE_DIR}/src/convex/GuBigConvexData.h
        ${GU_SOURCE_DIR}/src/convex/GuBigConvexData2.h
        ${GU_SOURCE_DIR}/src/convex/GuConvexEdgeFlags.h
        ${GU_SOURCE_DIR}/src/convex/GuConvexHelper.h
        ${GU_SOURCE_DIR}/src/convex/GuConvexMesh.h
        ${GU_SOURCE_DIR}/src/convex/GuConvexMeshData.h
        ${GU_SOURCE_DIR}/src/convex/GuConvexSupportTable.h
        ${GU_SOURCE_DIR}/src/convex/GuConvexUtilsInternal.h
        ${GU_SOURCE_DIR}/src/convex/GuCubeIndex.h
        ${GU_SOURCE_DIR}/src/convex/GuHillClimbing.h
        ${GU_SOURCE_DIR}/src/convex/GuShapeConvex.h
    )

    SET(PHYSXCOMMON_GU_DISTANCE_SOURCE
        ${GU_SOURCE_DIR}/src/distance/GuDistancePointBox.cpp
        ${GU_SOURCE_DIR}/src/distance/GuDistancePointTriangle.cpp
        ${GU_SOURCE_DIR}/src/distance/GuDistanceSegmentBox.cpp
        ${GU_SOURCE_DIR}/src/distance/GuDistanceSegmentSegment.cpp
        ${GU_SOURCE_DIR}/src/distance/GuDistanceSegmentTriangle.cpp
        ${GU_SOURCE_DIR}/src/distance/GuDistanceTriangleTriangle.cpp
        ${GU_SOURCE_DIR}/src/distance/GuDistancePointTetrahedron.cpp
        ${GU_SOURCE_DIR}/src/distance/GuDistancePointBox.h
        ${GU_SOURCE_DIR}/src/distance/GuDistancePointSegment.h
        ${GU_SOURCE_DIR}/src/distance/GuDistanceSegmentTriangle.h
        ${GU_SOURCE_DIR}/src/distance/GuDistanceTriangleTriangle.h
    )

    SET(PHYSXCOMMON_GU_GJK_SOURCE
        ${GU_SOURCE_DIR}/src/gjk/GuEPA.cpp
        ${GU_SOURCE_DIR}/src/gjk/GuGJKSimplex.cpp
        ${GU_SOURCE_DIR}/src/gjk/GuGJKTest.cpp
        ${GU_SOURCE_DIR}/src/gjk/GuEPA.h
        ${GU_SOURCE_DIR}/src/gjk/GuEPAFacet.h
        ${GU_SOURCE_DIR}/src/gjk/GuGJK.h
        ${GU_SOURCE_DIR}/src/gjk/GuGJKPenetration.h
        ${GU_SOURCE_DIR}/src/gjk/GuGJKRaycast.h
        ${GU_SOURCE_DIR}/src/gjk/GuGJKSimplex.h
        ${GU_SOURCE_DIR}/src/gjk/GuGJKTest.h
        ${GU_SOURCE_DIR}/src/gjk/GuGJKType.h
        ${GU_SOURCE_DIR}/src/gjk/GuGJKUtil.h
        ${GU_SOURCE_DIR}/src/gjk/GuVecBox.h
        ${GU_SOURCE_DIR}/src/gjk/GuVecCapsule.h
        ${GU_SOURCE_DIR}/src/gjk/GuVecConvex.h
        ${GU_SOURCE_DIR}/src/gjk/GuVecConvexHull.h
        ${GU_SOURCE_DIR}/src/gjk/GuVecConvexHullNoScale.h
        ${GU_SOURCE_DIR}/src/gjk/GuVecPlane.h
        ${GU_SOURCE_DIR}/src/gjk/GuVecSphere.h
        ${GU_SOURCE_DIR}/src/gjk/GuVecTriangle.h
        ${GU_SOURCE_DIR}/src/gjk/GuVecTetrahedron.h
    )

    SET(PHYSXCOMMON_GU_HF_SOURCE
        ${GU_SOURCE_DIR}/src/hf/GuHeightField.cpp
        ${GU_SOURCE_DIR}/src/hf/GuHeightFieldUtil.cpp
        ${GU_SOURCE_DIR}/src/hf/GuOverlapTestsHF.cpp
        ${GU_SOURCE_DIR}/src/hf/GuSweepsHF.cpp
        ${GU_SOURCE_DIR}/src/hf/GuEntityReport.h
        ${GU_SOURCE_DIR}/src/hf/GuHeightField.h
        ${GU_SOURCE_DIR}/src/hf/GuHeightFieldData.h
        ${GU_SOURCE_DIR}/src/hf/GuHeightFieldUtil.h
    )

    SET(PHYSXCOMMON_GU_INTERSECTION_SOURCE
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
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionCapsuleTriangle.h
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionEdgeEdge.h
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionRay.h
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionRayBox.h
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionRayCapsule.h
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionRayPlane.h
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionRaySphere.h
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionRayTriangle.h
        ${GU_SOURCE_DIR}/src/intersection/GuIntersectionSphereBox.h
    )
    SOURCE_GROUP(geomutils\\src\\intersection FILES ${PHYSXCOMMON_GU_INTERSECTION_SOURCE})

    SET(PXCOMMON_BVH4_FILES
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
    )

    SET(PHYSXCOMMON_GU_MESH_SOURCE
        ${GU_SOURCE_DIR}/src/mesh/GuBV4.cpp
        ${GU_SOURCE_DIR}/src/mesh/GuBV4Build.cpp

        ${PXCOMMON_BVH4_FILES}

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
        ${GU_SOURCE_DIR}/src/mesh/GuBV32.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV32Build.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4Build.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4Settings.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_AABBAABBSweepTest.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_BoxBoxOverlapTest.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_BoxOverlap_Internal.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_BoxSweep_Internal.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_BoxSweep_Params.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_CapsuleSweep_Internal.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_Common.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_Internal.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_ProcessStreamNoOrder_OBBOBB.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_ProcessStreamNoOrder_SegmentAABB.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_ProcessStreamNoOrder_SegmentAABB_Inflated.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_ProcessStreamNoOrder_SphereAABB.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_ProcessStreamOrdered_OBBOBB.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_ProcessStreamOrdered_SegmentAABB.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_ProcessStreamOrdered_SegmentAABB_Inflated.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_Slabs.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_Slabs_KajiyaNoOrder.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_Slabs_KajiyaOrdered.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_Slabs_SwizzledNoOrder.h
        ${GU_SOURCE_DIR}/src/mesh/GuBV4_Slabs_SwizzledOrdered.h
        ${GU_SOURCE_DIR}/src/mesh/GuBVConstants.h
        ${GU_SOURCE_DIR}/src/mesh/GuMeshData.h
        ${GU_SOURCE_DIR}/src/mesh/GuMidphaseInterface.h
        ${GU_SOURCE_DIR}/src/mesh/GuRTree.h
        ${GU_SOURCE_DIR}/src/mesh/GuSweepConvexTri.h
        ${GU_SOURCE_DIR}/src/mesh/GuSweepMesh.h
        ${GU_SOURCE_DIR}/src/mesh/GuTriangle.h
        ${GU_SOURCE_DIR}/src/mesh/GuTriangleCache.h
        ${GU_SOURCE_DIR}/src/mesh/GuTriangleMesh.h
        ${GU_SOURCE_DIR}/src/mesh/GuTriangleMeshBV4.h
        ${GU_SOURCE_DIR}/src/mesh/GuTriangleMeshRTree.h
        ${GU_SOURCE_DIR}/src/mesh/GuTetrahedron.h
        ${GU_SOURCE_DIR}/src/mesh/GuTetrahedronMesh.h
        ${GU_SOURCE_DIR}/src/mesh/GuTetrahedronMeshUtils.h
        ${GU_SOURCE_DIR}/src/mesh/GuTriangleRefinement.h
    )

    SET(PHYSXCOMMON_GU_PCM_SOURCE
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
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactConvexCommon.h
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactGen.h
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactGenUtil.h
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactGenUtil.cpp
        ${GU_SOURCE_DIR}/src/pcm/GuPCMContactMeshCallback.h
        ${GU_SOURCE_DIR}/src/pcm/GuPCMShapeConvex.h
        ${GU_SOURCE_DIR}/src/pcm/GuPCMTriangleContactGen.h
        ${GU_SOURCE_DIR}/src/pcm/GuPersistentContactManifold.h
    )

    SET(PHYSXCOMMON_GU_SWEEP_SOURCE
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
        ${GU_SOURCE_DIR}/src/sweep/GuSweepBoxBox.h
        ${GU_SOURCE_DIR}/src/sweep/GuSweepBoxSphere.h
        ${GU_SOURCE_DIR}/src/sweep/GuSweepBoxTriangle_FeatureBased.h
        ${GU_SOURCE_DIR}/src/sweep/GuSweepBoxTriangle_SAT.h
        ${GU_SOURCE_DIR}/src/sweep/GuSweepCapsuleBox.h
        ${GU_SOURCE_DIR}/src/sweep/GuSweepCapsuleCapsule.h
        ${GU_SOURCE_DIR}/src/sweep/GuSweepCapsuleTriangle.h
        ${GU_SOURCE_DIR}/src/sweep/GuSweepSphereCapsule.h
        ${GU_SOURCE_DIR}/src/sweep/GuSweepSphereSphere.h
        ${GU_SOURCE_DIR}/src/sweep/GuSweepSphereTriangle.h
        ${GU_SOURCE_DIR}/src/sweep/GuSweepTriangleUtils.h
    )

    SET(PHYSXCOMMON_GU_PRUNERS_SOURCE
        ${GU_SOURCE_DIR}/src/GuBounds.cpp
        ${GU_SOURCE_DIR}/src/GuQuery.h
        ${GU_SOURCE_DIR}/src/GuAABBTree.cpp
        ${GU_SOURCE_DIR}/src/GuAABBTree.h
        ${GU_SOURCE_DIR}/src/GuAABBTreeUpdateMap.h
        ${GU_SOURCE_DIR}/src/GuAABBTreeUpdateMap.cpp
        ${GU_SOURCE_DIR}/src/GuAABBTreeBounds.h
        ${GU_SOURCE_DIR}/src/GuAABBTreeNode.h
        ${GU_SOURCE_DIR}/src/GuAABBTreeBuildStats.h
        ${GU_SOURCE_DIR}/src/GuAABBTreeQuery.h
        ${GU_SOURCE_DIR}/src/GuSqInternal.cpp
        ${GU_SOURCE_DIR}/src/GuIncrementalAABBTree.h
        ${GU_SOURCE_DIR}/src/GuIncrementalAABBTree.cpp
        ${GU_SOURCE_DIR}/src/GuSAH.cpp
        ${GU_SOURCE_DIR}/src/GuSAH.h
        ${GU_SOURCE_DIR}/src/GuBVH.cpp
        ${GU_SOURCE_DIR}/src/GuBVH.h
        ${GU_SOURCE_DIR}/src/GuBVHTestsSIMD.h
        ${GU_SOURCE_DIR}/src/GuIncrementalAABBPrunerCore.h
        ${GU_SOURCE_DIR}/src/GuIncrementalAABBPrunerCore.cpp
        ${GU_SOURCE_DIR}/src/GuIncrementalAABBPruner.h
        ${GU_SOURCE_DIR}/src/GuIncrementalAABBPruner.cpp
        ${GU_SOURCE_DIR}/src/GuPruningPool.h
        ${GU_SOURCE_DIR}/src/GuPruningPool.cpp
        ${GU_SOURCE_DIR}/src/GuBucketPruner.h
        ${GU_SOURCE_DIR}/src/GuBucketPruner.cpp
        ${GU_SOURCE_DIR}/src/GuMaverickNode.h
        ${GU_SOURCE_DIR}/src/GuMaverickNode.cpp
        ${GU_SOURCE_DIR}/src/GuExtendedBucketPruner.h
        ${GU_SOURCE_DIR}/src/GuExtendedBucketPruner.cpp
        ${GU_SOURCE_DIR}/src/GuSecondaryPruner.h
        ${GU_SOURCE_DIR}/src/GuSecondaryPruner.cpp
        ${GU_SOURCE_DIR}/src/GuAABBPruner.h
        ${GU_SOURCE_DIR}/src/GuAABBPruner.cpp
        ${GU_SOURCE_DIR}/src/GuActorShapeMap.cpp
        ${GU_SOURCE_DIR}/src/GuCallbackAdapter.h
        ${GU_SOURCE_DIR}/src/GuQuerySystem.cpp
        ${GU_SOURCE_DIR}/src/GuFactory.cpp
    )

    SET(PHYSXCOMMON_GU_COOKING_SOURCE
        ${GU_SOURCE_DIR}/src/cooking/GuRTreeCooking.h
        ${GU_SOURCE_DIR}/src/cooking/GuRTreeCooking.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingBVH.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingHF.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingGrbTriangleMesh.h
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexMesh.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingTriangleMesh.h
        ${GU_SOURCE_DIR}/src/cooking/GuCookingTriangleMesh.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingTetrahedronMesh.h
        ${GU_SOURCE_DIR}/src/cooking/GuCookingTetrahedronMesh.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingVolumeIntegration.h
        ${GU_SOURCE_DIR}/src/cooking/GuCookingVolumeIntegration.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingQuickHullConvexHullLib.h
        ${GU_SOURCE_DIR}/src/cooking/GuCookingQuickHullConvexHullLib.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexPolygonsBuilder.h
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexPolygonsBuilder.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexMeshBuilder.h
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexMeshBuilder.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexHullUtils.h
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexHullUtils.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexHullLib.h
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexHullLib.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexHullBuilder.h
        ${GU_SOURCE_DIR}/src/cooking/GuCookingConvexHullBuilder.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingBigConvexDataBuilder.h
        ${GU_SOURCE_DIR}/src/cooking/GuCookingBigConvexDataBuilder.cpp
        ${GU_SOURCE_DIR}/src/cooking/GuCookingSDF.h
        ${GU_SOURCE_DIR}/src/cooking/GuCookingSDF.cpp
    )

    SET(PROJECT_SOURCES ${PROJECT_SOURCES}
        ${PHYSX_COMMON_SOURCE}
        ${PHYSXCOMMON_COMMON_INTERNAL_HEADERS}

        ${PHYSXCOMMON_COMMON_HEADERS}
        ${PHYSXCOMMON_GEOMETRY_HEADERS}
        ${PHYSXCOMMON_GEOMUTILS_HEADERS}
        ${PHYSXCOMMON_COLLISION_HEADERS}

        ${PXCOMMON_PLATFORM_SRC_FILES}

        ${PHYSXCOMMON_GU_HEADERS}
    #	${PHYSXCOMMON_GU_PXHEADERS}

        ${PHYSXCOMMON_GU_SOURCE}
        ${PHYSXCOMMON_GU_CCD_SOURCE}
        ${PHYSXCOMMON_GU_COMMON_SOURCE}
        ${PHYSXCOMMON_GU_CONTACT_SOURCE}
        ${PHYSXCOMMON_GU_CONVEX_SOURCE}
        ${PHYSXCOMMON_GU_DISTANCE_SOURCE}
        ${PHYSXCOMMON_GU_GJK_SOURCE}
        ${PHYSXCOMMON_GU_HF_SOURCE}
        ${PHYSXCOMMON_GU_INTERSECTION_SOURCE}
        ${PHYSXCOMMON_GU_MESH_SOURCE}
        ${PHYSXCOMMON_GU_PCM_SOURCE}
        ${PHYSXCOMMON_GU_SWEEP_SOURCE}
        ${PHYSXCOMMON_GU_PRUNERS_SOURCE}
        ${PHYSXCOMMON_GU_COOKING_SOURCE}
    )

    set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
        ${PXCOMMON_PLATFORM_INCLUDES}

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

        ${PHYSX_SOURCE_DIR}/physxgpu/include
    )

    SET(LL_SOURCE_DIR ${PHYSX_SOURCE_DIR}/physxextensions/src)

    SET(PHYSX_EXTENSIONS_SOURCE
        ${LL_SOURCE_DIR}/ExtBroadPhase.cpp
        ${LL_SOURCE_DIR}/ExtCollection.cpp
        ${LL_SOURCE_DIR}/ExtConvexMeshExt.cpp
        ${LL_SOURCE_DIR}/ExtCpuWorkerThread.cpp
        ${LL_SOURCE_DIR}/ExtDefaultCpuDispatcher.cpp
        ${LL_SOURCE_DIR}/ExtDefaultErrorCallback.cpp
        ${LL_SOURCE_DIR}/ExtDefaultProfiler.cpp
        ${LL_SOURCE_DIR}/ExtDefaultSimulationFilterShader.cpp
        ${LL_SOURCE_DIR}/ExtDefaultStreams.cpp
        ${LL_SOURCE_DIR}/ExtExtensions.cpp
        ${LL_SOURCE_DIR}/ExtPvd.cpp
        ${LL_SOURCE_DIR}/ExtPxStringTable.cpp
        ${LL_SOURCE_DIR}/ExtRaycastCCD.cpp
        ${LL_SOURCE_DIR}/ExtRigidBodyExt.cpp
        ${LL_SOURCE_DIR}/ExtRigidActorExt.cpp
        ${LL_SOURCE_DIR}/ExtSceneQueryExt.cpp
        ${LL_SOURCE_DIR}/ExtSceneQuerySystem.cpp
        ${LL_SOURCE_DIR}/ExtCustomSceneQuerySystem.cpp
        ${LL_SOURCE_DIR}/ExtSqQuery.cpp
        ${LL_SOURCE_DIR}/ExtSqQuery.h
        ${LL_SOURCE_DIR}/ExtSqManager.cpp
        ${LL_SOURCE_DIR}/ExtSqManager.h
        ${LL_SOURCE_DIR}/ExtSimpleFactory.cpp
        ${LL_SOURCE_DIR}/ExtSmoothNormals.cpp
        ${LL_SOURCE_DIR}/ExtDeformableSurfaceExt.cpp
        ${LL_SOURCE_DIR}/ExtDeformableVolumeExt.cpp
        ${LL_SOURCE_DIR}/ExtTriangleMeshExt.cpp
        ${LL_SOURCE_DIR}/ExtTetrahedronMeshExt.cpp
        ${LL_SOURCE_DIR}/ExtRemeshingExt.cpp
        ${LL_SOURCE_DIR}/ExtCpuWorkerThread.h
        ${LL_SOURCE_DIR}/ExtDefaultCpuDispatcher.h
        ${LL_SOURCE_DIR}/ExtDefaultProfiler.h
        ${LL_SOURCE_DIR}/ExtInertiaTensor.h
        ${LL_SOURCE_DIR}/ExtPlatform.h
        ${LL_SOURCE_DIR}/ExtPvd.h
        ${LL_SOURCE_DIR}/ExtSerialization.h
        ${LL_SOURCE_DIR}/ExtSharedQueueEntryPool.h
        ${LL_SOURCE_DIR}/ExtTaskQueueHelper.h
        ${LL_SOURCE_DIR}/ExtSampling.cpp
        ${LL_SOURCE_DIR}/ExtTetMakerExt.cpp
        ${LL_SOURCE_DIR}/ExtGjkQueryExt.cpp
        ${LL_SOURCE_DIR}/ExtCustomGeometryExt.cpp
        ${LL_SOURCE_DIR}/ExtConvexCoreExt.cpp
    )

    #TODO, create a propper define for whether GPU features are enabled or not!
    if (PX_GENERATE_GPU_PROJECTS AND (NOT (CMAKE_CROSSCOMPILING OR CMAKE_GENERATOR_PLATFORM STREQUAL "NX64")))
        LIST(APPEND PHYSX_EXTENSIONS_SOURCE "${LL_SOURCE_DIR}/ExtDeformableSkinning.cpp")
        LIST(APPEND PHYSX_EXTENSIONS_SOURCE "${LL_SOURCE_DIR}/ExtParticleExt.cpp")
        LIST(APPEND PHYSX_EXTENSIONS_SOURCE "${LL_SOURCE_DIR}/ExtParticleClothCooker.cpp")
    ENDIF()

    SET(PHYSX_EXTENSIONS_JOINTS_SOURCE
        ${LL_SOURCE_DIR}/ExtGearJoint.cpp
        ${LL_SOURCE_DIR}/ExtGearJoint.h
        ${LL_SOURCE_DIR}/ExtRackAndPinionJoint.cpp
        ${LL_SOURCE_DIR}/ExtRackAndPinionJoint.h
        ${LL_SOURCE_DIR}/ExtD6Joint.cpp
        ${LL_SOURCE_DIR}/ExtD6JointCreate.cpp
        ${LL_SOURCE_DIR}/ExtDistanceJoint.cpp
        ${LL_SOURCE_DIR}/ExtFixedJoint.cpp
        ${LL_SOURCE_DIR}/ExtJoint.cpp
        ${LL_SOURCE_DIR}/ExtPrismaticJoint.cpp
        ${LL_SOURCE_DIR}/ExtRevoluteJoint.cpp
        ${LL_SOURCE_DIR}/ExtSphericalJoint.cpp
        ${LL_SOURCE_DIR}/ExtConstraintHelper.h
        ${LL_SOURCE_DIR}/ExtD6Joint.h
        ${LL_SOURCE_DIR}/ExtDistanceJoint.h
        ${LL_SOURCE_DIR}/ExtFixedJoint.h
        ${LL_SOURCE_DIR}/ExtJoint.h
        ${LL_SOURCE_DIR}/ExtJointData.h
        ${LL_SOURCE_DIR}/ExtJointMetaDataExtensions.h
        ${LL_SOURCE_DIR}/ExtPrismaticJoint.h
        ${LL_SOURCE_DIR}/ExtRevoluteJoint.h
        ${LL_SOURCE_DIR}/ExtSphericalJoint.h
    )

    SET(PHYSX_EXTENSIONS_TET_SOURCE
        ${LL_SOURCE_DIR}/tet/ExtUtilities.h
        ${LL_SOURCE_DIR}/tet/ExtUtilities.cpp
        ${LL_SOURCE_DIR}/tet/ExtTetUnionFind.h
        ${LL_SOURCE_DIR}/tet/ExtTetUnionFind.cpp
        ${LL_SOURCE_DIR}/tet/ExtDelaunayBoundaryInserter.cpp
        ${LL_SOURCE_DIR}/tet/ExtDelaunayBoundaryInserter.h
        ${LL_SOURCE_DIR}/tet/ExtDelaunayTetrahedralizer.cpp
        ${LL_SOURCE_DIR}/tet/ExtDelaunayTetrahedralizer.h
        ${LL_SOURCE_DIR}/tet/ExtVec3.h
        ${LL_SOURCE_DIR}/tet/ExtTetSplitting.cpp
        ${LL_SOURCE_DIR}/tet/ExtTetSplitting.h
        ${LL_SOURCE_DIR}/tet/ExtTetTetraMesh.h
        ${LL_SOURCE_DIR}/tet/ExtFastWindingNumber.cpp
        ${LL_SOURCE_DIR}/tet/ExtFastWindingNumber.h
        ${LL_SOURCE_DIR}/tet/ExtRandomAccessHeap.h
        ${LL_SOURCE_DIR}/tet/ExtQuadric.h
        ${LL_SOURCE_DIR}/tet/ExtMeshSimplificator.h
        ${LL_SOURCE_DIR}/tet/ExtMeshSimplificator.cpp
        ${LL_SOURCE_DIR}/tet/ExtBVH.cpp
        ${LL_SOURCE_DIR}/tet/ExtBVH.h
        ${LL_SOURCE_DIR}/tet/ExtRemesher.cpp
        ${LL_SOURCE_DIR}/tet/ExtRemesher.h
        ${LL_SOURCE_DIR}/tet/ExtMarchingCubesTable.h
        ${LL_SOURCE_DIR}/tet/ExtMultiList.h
        ${LL_SOURCE_DIR}/tet/ExtInsideTester.cpp
        ${LL_SOURCE_DIR}/tet/ExtInsideTester.h
        ${LL_SOURCE_DIR}/tet/ExtOctreeTetrahedralizer.cpp
        ${LL_SOURCE_DIR}/tet/ExtOctreeTetrahedralizer.h
        ${LL_SOURCE_DIR}/tet/ExtVoxelTetrahedralizer.cpp
        ${LL_SOURCE_DIR}/tet/ExtVoxelTetrahedralizer.h
    )

    SET(PHYSX_EXTENSIONS_METADATA_SOURCE
        ${PHYSX_SOURCE_DIR}/physxmetadata/extensions/src/PxExtensionAutoGeneratedMetaDataObjects.cpp
        ${PHYSX_SOURCE_DIR}/physxmetadata/extensions/include/PxExtensionAutoGeneratedMetaDataObjectNames.h
        ${PHYSX_SOURCE_DIR}/physxmetadata/extensions/include/PxExtensionAutoGeneratedMetaDataObjects.h
        ${PHYSX_SOURCE_DIR}/physxmetadata/extensions/include/PxExtensionMetaDataObjects.h
    )

    SET(PHYSX_EXTENSIONS_OMNIPVD_SOURCE
        ${LL_SOURCE_DIR}/omnipvd/ExtOmniPvdRegistrationData.cpp
        ${LL_SOURCE_DIR}/omnipvd/ExtOmniPvdRegistrationData.h
        ${LL_SOURCE_DIR}/omnipvd/ExtOmniPvdSetData.h
        ${LL_SOURCE_DIR}/omnipvd/OmniPvdPxExtensionsTypes.h
        ${LL_SOURCE_DIR}/omnipvd/OmniPvdPxExtensionsSampler.h
        ${LL_SOURCE_DIR}/omnipvd/OmniPvdPxExtensionsSampler.cpp
    )

    SET(PHYSX_EXTENSIONS_HEADERS
        ${PHYSX_ROOT_DIR}/include/extensions/PxBroadPhaseExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxCollectionExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxConvexMeshExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxCudaHelpersExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxDefaultAllocator.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxDefaultCpuDispatcher.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxDefaultErrorCallback.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxDefaultProfiler.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxDefaultSimulationFilterShader.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxDefaultStreams.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxDeformableSurfaceExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxDeformableVolumeExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxExtensionsAPI.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxMassProperties.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxRaycastCCD.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxRepXSerializer.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxRepXSimpleType.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxRigidActorExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxRigidBodyExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxSceneQueryExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxSceneQuerySystemExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxCustomSceneQuerySystem.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxSerialization.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxShapeExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxSimpleFactory.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxSmoothNormals.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxSoftBodyExt.h #deprecated
        ${PHYSX_ROOT_DIR}/include/extensions/PxStringTableExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxTriangleMeshExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxTetrahedronMeshExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxRemeshingExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxTriangleMeshAnalysisResult.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxTetrahedronMeshAnalysisResult.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxTetMakerExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxGjkQueryExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxCustomGeometryExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxSamplingExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxConvexCoreExt.h
    )

    #TODO, create a propper define for whether GPU features are enabled or not!
    if (PX_GENERATE_GPU_PROJECTS AND (NOT (CMAKE_CROSSCOMPILING OR CMAKE_GENERATOR_PLATFORM STREQUAL "NX64")))
        LIST(APPEND PHYSX_EXTENSIONS_HEADERS "${PHYSX_ROOT_DIR}/include/extensions/PxDeformableSkinningExt.h")
        LIST(APPEND PHYSX_EXTENSIONS_HEADERS "${PHYSX_ROOT_DIR}/include/extensions/PxParticleClothCooker.h")
        LIST(APPEND PHYSX_EXTENSIONS_HEADERS "${PHYSX_ROOT_DIR}/include/extensions/PxParticleExt.h")
    ENDIF()

    SET(PHYSX_JOINT_HEADERS
        ${PHYSX_ROOT_DIR}/include/extensions/PxConstraintExt.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxD6Joint.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxD6JointCreate.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxDistanceJoint.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxFixedJoint.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxGearJoint.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxRackAndPinionJoint.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxJoint.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxJointLimit.h
    #	${PHYSX_ROOT_DIR}/include/extensions/PxJointRepXSerializer.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxPrismaticJoint.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxRevoluteJoint.h
        ${PHYSX_ROOT_DIR}/include/extensions/PxSphericalJoint.h
    )

    SET(PHYSX_FILEBUF_HEADERS
        ${PHYSX_ROOT_DIR}/include/filebuf/PxFileBuf.h
    )

    SET(PHYSX_EXTENSIONS_SERIALIZATION_SOURCE
        ${LL_SOURCE_DIR}/serialization/SnSerialization.cpp
        ${LL_SOURCE_DIR}/serialization/SnSerializationRegistry.cpp
        ${LL_SOURCE_DIR}/serialization/SnSerializationRegistry.h
        ${LL_SOURCE_DIR}/serialization/SnSerialUtils.cpp
        ${LL_SOURCE_DIR}/serialization/SnSerialUtils.h
    )

    SET(PHYSX_EXTENSIONS_SERIALIZATION_XML_SOURCE
        ${LL_SOURCE_DIR}/serialization/Xml/SnJointRepXSerializer.cpp
        ${LL_SOURCE_DIR}/serialization/Xml/SnJointRepXSerializer.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnRepXCoreSerializer.cpp
        ${LL_SOURCE_DIR}/serialization/Xml/SnRepXUpgrader.cpp
        ${LL_SOURCE_DIR}/serialization/Xml/SnXmlSerialization.cpp
        ${LL_SOURCE_DIR}/serialization/Xml/SnPxStreamOperators.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnRepX1_0Defaults.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnRepX3_1Defaults.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnRepX3_2Defaults.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnRepXCollection.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnRepXCoreSerializer.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnRepXSerializerImpl.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnRepXUpgrader.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnSimpleXmlWriter.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnXmlDeserializer.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnXmlImpl.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnXmlMemoryAllocator.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnXmlMemoryPool.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnXmlMemoryPoolStreams.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnXmlReader.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnXmlSerializer.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnXmlSimpleXmlWriter.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnXmlStringToType.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnXmlVisitorReader.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnXmlVisitorWriter.h
        ${LL_SOURCE_DIR}/serialization/Xml/SnXmlWriter.h
        ${LL_SOURCE_DIR}/serialization/Xml/PsFastXml.h
        ${LL_SOURCE_DIR}/serialization/Xml/PsFastXml.cpp
    )

    SET(PHYSX_EXTENSIONS_SERIALIZATION_FILE_SOURCE
        ${LL_SOURCE_DIR}/serialization/File/SnFile.h
    )

    SET(PHYSX_EXTENSIONS_SERIALIZATION_BINARY_SOURCE
        ${LL_SOURCE_DIR}/serialization/Binary/SnBinaryDeserialization.cpp
        ${LL_SOURCE_DIR}/serialization/Binary/SnBinarySerialization.cpp
        ${LL_SOURCE_DIR}/serialization/Binary/SnSerializationContext.cpp
        ${LL_SOURCE_DIR}/serialization/Binary/SnSerializationContext.h
    )

    SET(PROJECT_SOURCES ${PROJECT_SOURCES}
        ${PHYSXEXTENSIONS_PLATFORM_SRC_FILES}

        ${PHYSX_EXTENSIONS_SOURCE}
        ${PHYSX_EXTENSIONS_TET_SOURCE}
        ${PHYSX_EXTENSIONS_JOINTS_SOURCE}
        ${PHYSX_EXTENSIONS_METADATA_SOURCE}
        ${PHYSX_EXTENSIONS_OMNIPVD_SOURCE}

        ${PHYSX_EXTENSIONS_HEADERS}
        ${PHYSX_JOINT_HEADERS}
        ${PHYSX_FILEBUF_HEADERS}

        ${PHYSX_EXTENSIONS_SERIALIZATION_SOURCE}
        ${PHYSX_EXTENSIONS_SERIALIZATION_XML_SOURCE}
        ${PHYSX_EXTENSIONS_SERIALIZATION_FILE_SOURCE}
        ${PHYSX_EXTENSIONS_SERIALIZATION_BINARY_SOURCE}
    )

    set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
        PRIVATE ${PHYSXEXTENSIONS_PLATFORM_INCLUDES}

        PRIVATE ${PHYSX_ROOT_DIR}/include

        PRIVATE ${PHYSX_SOURCE_DIR}/common/include
        PRIVATE ${PHYSX_SOURCE_DIR}/common/src

        PRIVATE ${PHYSX_SOURCE_DIR}/geomutils/include
        PRIVATE ${PHYSX_SOURCE_DIR}/geomutils/src
        PRIVATE ${PHYSX_SOURCE_DIR}/geomutils/src/intersection
        PRIVATE ${PHYSX_SOURCE_DIR}/geomutils/src/mesh

        PRIVATE ${PHYSX_SOURCE_DIR}/physxmetadata/core/include
        PRIVATE ${PHYSX_SOURCE_DIR}/physxmetadata/extensions/include

        PRIVATE ${PHYSX_SOURCE_DIR}/physxextensions/src
        PRIVATE ${PHYSX_SOURCE_DIR}/physxextensions/src/serialization/Xml
        PRIVATE ${PHYSX_SOURCE_DIR}/physxextensions/src/serialization/Binary
        PRIVATE ${PHYSX_SOURCE_DIR}/physxextensions/src/serialization/File

        PRIVATE ${PHYSX_SOURCE_DIR}/physx/src

        PRIVATE ${PHYSX_SOURCE_DIR}/pvd/include

        PRIVATE ${PHYSX_SOURCE_DIR}/scenequery/include
    )


    SET(PROJECT_SOURCES ${PROJECT_SOURCES}
        ${PHYSX_HEADERS}
        ${PHYSX_COMMON_HEADERS}
        ${PHYSX_MATERIAL_HEADERS}
        ${PHYSX_PVD_HEADERS}
        ${PHYSX_OMNIPVD_HEADERS}
        ${PHYSX_OMNIPVD_SOURCE}
        ${PHYSX_PVD_SOURCE}
        ${PHYSX_SOLVER_HEADERS}
        ${PHYSX_COLLISION_HEADERS}

        ${PHYSX_METADATA_HEADERS}
        ${PHYSX_METADATA_SOURCE}

        ${PHYSX_CORE_SOURCE}
        ${PHYSX_BUFFERING_SOURCE}
        ${PHYSX_IMMEDIATEMODE_SOURCE}
        ${PHYSX_MATERIALS_SOURCE}
        ${PHYSX_ARTICULATIONS_SOURCE}

        ${PHYSX_PLATFORM_SRC_FILES}

        ${GPUDEPENDENCIES_INCLUDES}
        ${GPUDEPENDENCIES_SOURCE}
        ${GPUDEPENDENCIES_PLATFORM_SOURCE}
        ${PHYSXFOUNDATION_SOURCE}
    )

    set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
        ${PHYSX_PLATFORM_INCLUDES}

        ${PHYSX_ROOT_DIR}/include

        ${PHYSX_SOURCE_DIR}/common/include
        ${PHYSX_SOURCE_DIR}/common/src

        ${PHYSX_SOURCE_DIR}/physx/src
        ${PHYSX_SOURCE_DIR}/physx/src/device
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

        ${PHYSX_SOURCE_DIR}/lowlevel/api/include
        ${PHYSX_SOURCE_DIR}/lowlevel/software/include
        ${PHYSX_SOURCE_DIR}/lowlevel/common/include/pipeline
        ${PHYSX_SOURCE_DIR}/lowlevel/common/include/utils

        ${PHYSX_SOURCE_DIR}/lowlevelaabb/include

        ${PHYSX_SOURCE_DIR}/lowleveldynamics/include
        ${PHYSX_SOURCE_DIR}/lowleveldynamics/shared

        ${PHYSX_SOURCE_DIR}/simulationcontroller/include
        ${PHYSX_SOURCE_DIR}/simulationcontroller/src

        ${PHYSX_SOURCE_DIR}/scenequery/include

        ${PHYSX_SOURCE_DIR}/physxmetadata/core/include

        ${PHYSX_SOURCE_DIR}/immediatemode/include

        ${PHYSX_SOURCE_DIR}/pvd/include

        ${PHYSX_SOURCE_DIR}/gpucommon/include
        ${PHYSX_SOURCE_DIR}/gpucommon/src/CUDA

        ${PHYSX_SOURCE_DIR}/omnipvd
        ${PHYSX_ROOT_DIR}/pvdruntime/include
    )
endif()

qt_add_executable(GaitSym5
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_include_directories(GaitSym5 PRIVATE
    ${INCLUDE_DIRECTORIES}
)

target_link_libraries(GaitSym5 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(GaitSym5 PRIVATE Qt6::OpenGL)
target_link_libraries(GaitSym5 PRIVATE Qt6::OpenGLWidgets)

if(WIN32)
  target_link_libraries(GaitSym5 PRIVATE wsock32 ws2_32)
endif()

if (NOT use_mucojo_src AND CMAKE_BUILD_TYPE STREQUAL Release)
    target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../MuJoCo/mujoco-3.3.0/build/lib/Release/mujoco.lib")
endif()
if (NOT use_mucojo_src AND CMAKE_BUILD_TYPE STREQUAL Debug)
    target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../MuJoCo/mujoco-3.3.0/build/lib/Debug/mujoco.lib")
endif()

if(NOT use_physx_src AND CMAKE_BUILD_TYPE STREQUAL Release)
    target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/release/PhysX_64.lib")
    target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/release/PhysXFoundation_64.lib")
    target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/release/PhysXCommon_64.lib")
    target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/release/PhysXCooking_64.lib")
    target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/release/PhysXExtensions_static_64.lib")
    target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/release/PhysXPvdSDK_static_64.lib")
endif()

if(NOT use_physx_src AND MAKE_BUILD_TYPE STREQUAL Debug)
    target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/debug/PhysX_64.lib")
    target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/debug/PhysXFoundation_64.lib")
    target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/debug/PhysXCommon_64.lib")
    target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/debug/PhysXCooking_64.lib")
    target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/debug/PhysXExtensions_static_64.lib")
    target_link_libraries(GaitSym5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/debug/PhysXPvdSDK_static_64.lib")
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

qt_finalize_executable(GaitSym5)

if (MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_definitions(_USE_MATH_DEFINES)
    # add_compile_options(/P)
endif ()

find_package(Qt6 REQUIRED COMPONENTS Xml)
target_link_libraries(GaitSym5 PRIVATE Qt6::Xml)

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL Release) # for release builds make sure all the dlls are in place
    if (NOT use_mucojo_src)
        add_custom_command(TARGET GaitSym5 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../MuJoCo/mujoco-3.3.0/build/bin/Release/mujoco.dll" ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()
    if (NOT use_physx_src)
        add_custom_command(TARGET GaitSym5 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/release/PhysX_64.dll" ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/release/PhysXFoundation_64.dll" ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/release/PhysXCommon_64.dll" ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/physx/bin/win.x86_64.vc143.md/release/PhysXCooking_64.dll" ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()
    add_custom_command(TARGET GaitSym5 POST_BUILD
        COMMAND "${CMAKE_PREFIX_PATH}/bin/windeployqt.exe" "${CMAKE_CURRENT_BINARY_DIR}/GaitSym5.exe" # this copies the Qt dlls and a fairly minimal set of plugins
    )
endif()

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL Debug) # for release builds make sure all the dlls are in place
    if (NOT use_mucojo_src)
        add_custom_command(TARGET GaitSym5 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../MuJoCo/mujoco-3.3.0/build/bin/Debug/mujoco.dll" ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()
    if (NOT use_physx_src)
        add_custom_command(TARGET GaitSym5 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/debug/bin/win.x86_64.vc143.md/release/PhysX_64.dll" ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/debug/bin/win.x86_64.vc143.md/release/PhysXFoundation_64.dll" ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/debug/bin/win.x86_64.vc143.md/release/PhysXCommon_64.dll" ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../PhysX/PhysX-107.0-physx-5.6.0-flow-2.2.0-blast-5.0.6/debug/bin/win.x86_64.vc143.md/release/PhysXCooking_64.dll" ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()
    add_custom_command(TARGET GaitSym5 POST_BUILD
        COMMAND "${CMAKE_PREFIX_PATH}/bin/windeployqt.exe" "${CMAKE_CURRENT_BINARY_DIR}/GaitSym5.exe" # this copies the Qt dlls and a fairly minimal set of plugins
    )
endif()


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
