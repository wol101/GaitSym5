TEMPLATE = app
CONFIG += console c++17 c11
CONFIG -= app_bundle
CONFIG -= qt


INCLUDEPATH += \
    ../mujoco-3.0.1/build/_deps/marchingcubecpp-src \
    ../mujoco-3.0.1/include \
    ../mujoco-3.0.1/src \
    ../mujoco-3.0.1/build/_deps/ccd-src/src \
    ../mujoco-3.0.1/build/_deps/ccd-build/src \
    ../mujoco-3.0.1/build/_deps/lodepng-src \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r \
    ../mujoco-3.0.1/build/_deps/tinyobjloader-src \
    ../mujoco-3.0.1/build/_deps/tinyxml2-src

DEFINES += \
    WIN32 \
    _WINDOWS \
    MUJOCO_TINYOBJLOADER_IMPL \
    _GNU_SOURCE \
    CCD_STATIC_DEFINE \
    MUJOCO_DLL_EXPORTS \
    MC_IMPLEM_ENABLE \
    mjUSEPLATFORMSIMD \
    _CRT_SECURE_NO_WARNINGS \
    _CRT_SECURE_NO_DEPRECATE \
    EIGEN_MPL2_ONLY \
    TINYXML2_DEBUG \
    CMAKE_INTDIR="Debug" \
    mujoco_EXPORTS

SOURCES += \
    main.cpp \
    ../mujoco-3.0.1/src/engine/engine_callback.c \
    ../mujoco-3.0.1/src/engine/engine_collision_box.c \
    ../mujoco-3.0.1/src/engine/engine_collision_convex.c \
    ../mujoco-3.0.1/src/engine/engine_collision_driver.c \
    ../mujoco-3.0.1/src/engine/engine_collision_primitive.c \
    ../mujoco-3.0.1/src/engine/engine_collision_sdf.c \
    ../mujoco-3.0.1/src/engine/engine_core_constraint.c \
    ../mujoco-3.0.1/src/engine/engine_core_smooth.c \
    ../mujoco-3.0.1/src/engine/engine_crossplatform.cc \
    ../mujoco-3.0.1/src/engine/engine_derivative.c \
    ../mujoco-3.0.1/src/engine/engine_derivative_fd.c \
    ../mujoco-3.0.1/src/engine/engine_forward.c \
    ../mujoco-3.0.1/src/engine/engine_inverse.c \
    ../mujoco-3.0.1/src/engine/engine_island.c \
    ../mujoco-3.0.1/src/engine/engine_io.c \
    ../mujoco-3.0.1/src/engine/engine_passive.c \
    ../mujoco-3.0.1/src/engine/engine_plugin.cc \
    ../mujoco-3.0.1/src/engine/engine_print.c \
    ../mujoco-3.0.1/src/engine/engine_ray.c \
    ../mujoco-3.0.1/src/engine/engine_resource.c \
    ../mujoco-3.0.1/src/engine/engine_sensor.c \
    ../mujoco-3.0.1/src/engine/engine_setconst.c \
    ../mujoco-3.0.1/src/engine/engine_solver.c \
    ../mujoco-3.0.1/src/engine/engine_support.c \
    ../mujoco-3.0.1/src/engine/engine_util_blas.c \
    ../mujoco-3.0.1/src/engine/engine_util_errmem.c \
    ../mujoco-3.0.1/src/engine/engine_util_misc.c \
    ../mujoco-3.0.1/src/engine/engine_util_solve.c \
    ../mujoco-3.0.1/src/engine/engine_util_sparse.c \
    ../mujoco-3.0.1/src/engine/engine_util_spatial.c \
    ../mujoco-3.0.1/src/engine/engine_vfs.c \
    ../mujoco-3.0.1/src/engine/engine_vis_init.c \
    ../mujoco-3.0.1/src/engine/engine_vis_interact.c \
    ../mujoco-3.0.1/src/engine/engine_vis_state.c \
    ../mujoco-3.0.1/src/engine/engine_vis_visualize.c \
    ../mujoco-3.0.1/src/thread/thread_pool.cc \
    ../mujoco-3.0.1/src/thread/thread_task.cc \
    ../mujoco-3.0.1/src/user/user_composite.cc \
    ../mujoco-3.0.1/src/user/user_flexcomp.cc \
    ../mujoco-3.0.1/src/user/user_mesh.cc \
    ../mujoco-3.0.1/src/user/user_model.cc \
    ../mujoco-3.0.1/src/user/user_objects.cc \
    ../mujoco-3.0.1/src/user/user_util.cc \
    ../mujoco-3.0.1/src/xml/xml_api.cc \
    ../mujoco-3.0.1/src/xml/xml_base.cc \
    ../mujoco-3.0.1/src/xml/xml.cc \
    ../mujoco-3.0.1/src/xml/xml_native_reader.cc \
    ../mujoco-3.0.1/src/xml/xml_numeric_format.cc \
    ../mujoco-3.0.1/src/xml/xml_native_writer.cc \
    ../mujoco-3.0.1/src/xml/xml_urdf.cc \
    ../mujoco-3.0.1/src/xml/xml_util.cc \
    ../mujoco-3.0.1/build/_deps/ccd-src/src/ccd.c \
    ../mujoco-3.0.1/build/_deps/ccd-src/src/support.c \
    ../mujoco-3.0.1/build/_deps/ccd-src/src/mpr.c \
    ../mujoco-3.0.1/build/_deps/ccd-src/src/vec3.c \
    ../mujoco-3.0.1/build/_deps/ccd-src/src/polytope.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/accessors_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/geom2_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/geom_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/global_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/io_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/libqhull_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/mem_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/merge_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/poly2_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/poly_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/qset_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/random_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/rboxlib_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/stat_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/user_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/usermem_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/userprintf_r.c \
    ../mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/userprintf_rbox_r.c \
    ../mujoco-3.0.1/build/_deps/lodepng-src/lodepng.cpp \
    ../mujoco-3.0.1/build/_deps/tinyxml2-src/tinyxml2.cpp

