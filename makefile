# Note use "make -j 8" to use 8 threads
# get the system and architecture
ifeq ($(OS),Windows_NT)
	SYSTEM := WIN32
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		ARCH := AMD64
	endif
	ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
		ARCH := AMD64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		ARCH := IA32
	endif
	UNAME_S := $(shell uname -s)
	ifneq (,$(findstring CYGWIN,$(UNAME_S)))
		SYSTEM := CYGWIN
	endif
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		SYSTEM := LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		SYSTEM := OSX
	endif
	UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		ARCH := AMD64
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
		ARCH := IA32
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		ARCH := ARM
	endif
	ifneq ($(filter ppc%,$(UNAME_P)),)
		ARCH := PPC
	endif
endif
HOST := $(shell hostname)
cc_AVAIL := $(shell command -v cc 2> /dev/null)
gcc_AVAIL := $(shell command -v gcc 2> /dev/null)
CC_AVAIL := $(shell command -v CC 2> /dev/null)
gpp_AVAIL := $(shell command -v g++ 2> /dev/null)

ifeq ($(SYSTEM),OSX)
	CXXFLAGS = -Wall -ffast-math -DEXPERIMENTAL -std=c++17 \
	-DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
	-DBYTE_ORDER_LITTLE_ENDIAN -DUSE_UNIX_ERRORS -DHAVE_ALLOCA_H -DMAKEFILE_OSX -DNDEBUG
	LDFLAGS =
	LIBS = -lpthread -lm -framework CoreServices
endif

ifeq ($(SYSTEM),LINUX)
	CXXFLAGS = -static -O3 -ffast-math -DEXPERIMENTAL -std=c++17 \
	-DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
	-DBYTE_ORDER_LITTLE_ENDIAN -DHAVE_MALLOC_H -DUSE_UNIX_ERRORS -DMAKEFILE_LINUX -DNDEBUG
	LDFLAGS = -static
ifdef gcc_AVAIL
	CC = gcc
endif
ifdef gpp_AVAIL
	CXX = g++
endif
ifdef cc_AVAIL
	CC = cc
endif
ifdef CC_AVAIL
	CXX = CC
endif
	LIBS = -lpthread -lm
endif

ifeq ($(SYSTEM),CYGWIN)
	# cygwin64 gcc version
	CXXFLAGS = -O3 -g -ffast-math -Wa,-mbig-obj -Dexprtk_disable_enhanced_features -std=c++17 \
	-DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
	-DBYTE_ORDER_LITTLE_ENDIAN -DHAVE_MALLOC_H -DUSE_UNIX_ERRORS -DHAVE_ALLOCA_H \
	-DALLOCA_H_NEEDED -DNEED_BCOPY -DSTRINGS_H_NEEDED -DMAKEFILE_CYGWIN -DNDEBUG
	LDFLAGS =
	CXX = g++
	CC = gcc
	LIBS = -lpthread -lm
endif

ifeq ($(HOST),submitter.itservices.manchester.ac.uk)
	#CXXFLAGS = -static -ffast-math -O3 -DEXPERIMENTAL \
	#CXXFLAGS = -static -O2 -DEXPERIMENTAL -std=c++17 \
	CXXFLAGS = -static -O3 -ffast-math -DEXPERIMENTAL -std=c++17 \
	-DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
	-DBYTE_ORDER_LITTLE_ENDIAN -DHAVE_MALLOC_H -DUSE_UNIX_ERRORS -DMAKEFILE_SUBMITTER -DNDEBUG
	LDFLAGS = -static
	# condor_compile is tricky with pthread so best avoided and just use vanilla universe
	# CXX = condor_compile g++
	# CC = condor_compile gcc
	CXX = g++
	CC = gcc
	LIBS = -lpthread -lm
endif

ifeq ($(HOST),l-u-roboticssuite.it.manchester.ac.uk)
	#CXXFLAGS = -static -ffast-math -O3 -DEXPERIMENTAL
	CXXFLAGS = -static -O3 -ffast-math -DEXPERIMENTAL -std=c++17 \
	-DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
	-DBYTE_ORDER_LITTLE_ENDIAN -DHAVE_MALLOC_H -DUSE_UNIX_ERRORS -DMAKEFILE_ROBOTICS -DNDEBUG
	LDFLAGS = -static
	# condor_compile is tricky with pthread so best avoided and just use vanilla universe
	# CXX = condor_compile g++
	# CC = condor_compile gcc
	CXX = g++
	CC = gcc
	LIBS = -lpthread -lm
endif

# this looks for the substring csf
ifneq (,$(findstring csf,$(HOST)))
	# -static libraries are not available on csf
	CXXFLAGS = -O3 -ffast-math -DEXPERIMENTAL -std=c++17 \
	-DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
	-DBYTE_ORDER_LITTLE_ENDIAN -DHAVE_MALLOC_H -DUSE_UNIX_ERRORS -DMAKEFILE_CSF -DNDEBUG
	LDFLAGS =
	CXX = g++
	CC = gcc
	LIBS = -lpthread -lm
endif

# vpath %.cpp src
# vpath %.c src

INC_DIRS += \
-I fast_double_parser \
-I GaitSymQt \
-I glextrusion \
-I libgwavi \
-I PhysX \
-I pocketpy \
-I pystring \
-I rapidxml \
-I src \
-I tinyply \
-I asio-1.18.2/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/common/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/gjk \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/fastxml/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/convex \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/distance \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/common \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/intersection \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/common/src \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/hf \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/sweep \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/api/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/common/include/pipeline \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/software/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/common/include/collision \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevelaabb/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/common/include/utils \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxgpu/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/device \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/scenequery/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/ccd \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/pvd/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/File \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxmetadata/core/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/physxmetadata/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxmetadata/extensions/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Xml \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/filebuf/include \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Binary \
-I PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src \
-I MuJoCo/mujoco-3.0.1/build/_deps/marchingcubecpp-src \
-I MuJoCo/mujoco-3.0.1/include \
-I MuJoCo/mujoco-3.0.1/src \
-I MuJoCo/mujoco-3.0.1/build/_deps/ccd-src/src \
-I MuJoCo/mujoco-3.0.1/build/_deps/ccd-build/src \
-I MuJoCo/mujoco-3.0.1/build/_deps/lodepng-src \
-I MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r \
-I MuJoCo/mujoco-3.0.1/build/_deps/tinyobjloader-src \
-I MuJoCo/mujoco-3.0.1/build/_deps/tinyxml2-src \
-I ode-0.15/OPCODE \
-I ode-0.15/include \
-I ode-0.15/ode/src

GAITSYMSRC = \
ArgParse.cpp\
AMotorJoint.cpp\
BallJoint.cpp\
Body.cpp\
BoxGeom.cpp\
ButterworthFilter.cpp\
CappedCylinderGeom.cpp\
Colour.cpp\
Contact.cpp\
Controller.cpp\
ConvexGeom.cpp\
CyclicDriver.cpp\
CylinderWrapStrap.cpp\
DampedSpringMuscle.cpp\
DataFile.cpp\
DataTarget.cpp\
DataTargetMarkerCompare.cpp\
DataTargetQuaternion.cpp\
DataTargetScalar.cpp\
DataTargetVector.cpp\
Drivable.cpp\
Driver.cpp\
Filter.cpp\
FixedDriver.cpp\
FixedJoint.cpp\
FloatingHingeJoint.cpp\
FluidSac.cpp\
FluidSacIdealGas.cpp\
FluidSacIncompressible.cpp\
Geom.cpp\
Global.cpp\
GSUtil.cpp\
HingeJoint.cpp\
Joint.cpp\
LMotorJoint.cpp\
MAMuscleComplete.cpp\
MAMuscle.cpp\
Marker.cpp\
MarkerPositionDriver.cpp\
MarkerEllipseDriver.cpp\
MovingAverage.cpp\
Muscle.cpp\
NamedObject.cpp\
NPointStrap.cpp\
ObjectiveMain.cpp\
ObjectiveMainASIOAsync.cpp\
ParseXML.cpp\
PIDErrorInController.cpp\
PIDMuscleLengthController.cpp\
PlaneGeom.cpp\
Reporter.cpp\
Simulation.cpp\
SliderJoint.cpp\
SphereGeom.cpp\
StackedBoxCarDriver.cpp\
StepDriver.cpp\
Strap.cpp\
TegotaeDriver.cpp\
ThreeHingeJointDriver.cpp\
TwoHingeJointDriver.cpp\
TorqueReporter.cpp\
TrimeshGeom.cpp\
TwoCylinderWrapStrap.cpp\
TwoPointStrap.cpp\
UniversalJoint.cpp\
XMLConverter.cpp

LIBCCDSRC = \
alloc.c \
ccd.c \
mpr.c \
polytope.c \
support.c \
vec3.c

ODESRC = \
array.cpp \
box.cpp \
capsule.cpp \
collision_convex_trimesh.cpp \
collision_cylinder_box.cpp \
collision_cylinder_plane.cpp \
collision_cylinder_sphere.cpp \
collision_cylinder_trimesh.cpp \
collision_kernel.cpp \
collision_libccd.cpp \
collision_quadtreespace.cpp \
collision_sapspace.cpp \
collision_space.cpp \
collision_transform.cpp \
collision_trimesh_box.cpp \
collision_trimesh_ccylinder.cpp \
collision_trimesh_disabled.cpp \
collision_trimesh_distance.cpp \
collision_trimesh_gimpact.cpp \
collision_trimesh_opcode.cpp \
collision_trimesh_plane.cpp \
collision_trimesh_ray.cpp \
collision_trimesh_sphere.cpp \
collision_trimesh_trimesh_new.cpp \
collision_trimesh_trimesh.cpp \
collision_util.cpp \
convex.cpp \
cylinder.cpp \
error.cpp \
export-dif.cpp \
fastdot.cpp \
fastldlt.cpp \
fastlsolve.cpp \
fastltsolve.cpp \
heightfield.cpp \
lcp.cpp \
mass.cpp \
mat.cpp \
matrix.cpp \
memory.cpp \
misc.cpp \
nextafterf.c \
objects.cpp \
obstack.cpp \
ode.cpp \
odeinit.cpp \
odemath.cpp \
odeou.cpp \
odetls.cpp \
plane.cpp \
quickstep.cpp \
ray.cpp \
rotation.cpp \
sphere.cpp \
step.cpp \
threading_base.cpp \
threading_impl.cpp \
threading_pool_posix.cpp \
threading_pool_win.cpp \
timer.cpp \
util.cpp

ODEJOINTSSRC = \
amotor.cpp \
ball.cpp \
contact.cpp \
dball.cpp \
dhinge.cpp \
fixed.cpp \
floatinghinge.cpp \
hinge.cpp \
hinge2.cpp \
joint.cpp \
lmotor.cpp \
null.cpp \
piston.cpp \
plane2d.cpp \
pr.cpp \
pu.cpp \
slider.cpp \
transmission.cpp \
universal.cpp

OPCODEICESRC = \
IceAABB.cpp \
IceContainer.cpp \
IceHPoint.cpp \
IceIndexedTriangle.cpp \
IceMatrix3x3.cpp \
IceMatrix4x4.cpp \
IceOBB.cpp \
IcePlane.cpp \
IcePoint.cpp \
IceRandom.cpp \
IceRay.cpp \
IceRevisitedRadix.cpp \
IceSegment.cpp \
IceTriangle.cpp \
IceUtils.cpp

OPCODESRC = \
OPC_AABBCollider.cpp \
OPC_AABBTree.cpp \
OPC_BaseModel.cpp \
OPC_Collider.cpp \
OPC_Common.cpp \
OPC_HybridModel.cpp \
OPC_LSSCollider.cpp \
OPC_MeshInterface.cpp \
OPC_Model.cpp \
OPC_OBBCollider.cpp \
OPC_OptimizedTree.cpp \
OPC_Picking.cpp \
OPC_PlanesCollider.cpp \
OPC_RayCollider.cpp \
OPC_SphereCollider.cpp \
OPC_TreeBuilders.cpp \
OPC_TreeCollider.cpp \
OPC_VolumeCollider.cpp \
Opcode.cpp \
StdAfx.cpp

PYSTRINGSRC = \
pystring.cpp

PHYSXSRC = \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/common/src/CmCollection.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/common/src/CmPtrTable.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/common/src/CmRadixSort.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/common/src/CmSerialize.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/common/src/CmVisualization.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/fastxml/src/PsFastXml.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/foundation/FdAllocator.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/foundation/FdAssert.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/foundation/FdFoundation.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/foundation/FdMathUtils.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/foundation/FdString.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/foundation/FdTempAllocator.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/ccd/GuCCDSweepConvexMesh.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/ccd/GuCCDSweepPrimitives.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/common/GuAdjacencies.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/common/GuBarycentricCoordinates.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/common/GuEdgeList.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/common/GuMeshAnalysis.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/common/GuMeshCleaner.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/common/GuQuantizer.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/common/GuSeparatingAxes.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/common/GuVertexReducer.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactBoxBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactCapsuleBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactCapsuleCapsule.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactCapsuleConvex.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactCapsuleMesh.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactConvexConvex.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactConvexMesh.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactCustomGeometry.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactPlaneBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactPlaneCapsule.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactPlaneConvex.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactPolygonPolygon.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactSphereBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactSphereCapsule.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactSphereMesh.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactSpherePlane.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuContactSphereSphere.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/contact/GuFeatureCode.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/convex/GuBigConvexData.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/convex/GuConvexHelper.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/convex/GuConvexMesh.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/convex/GuConvexSupportTable.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/convex/GuConvexUtilsInternal.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/convex/GuHillClimbing.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/convex/GuShapeConvex.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/cooking/GuCookingBigConvexDataBuilder.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/cooking/GuCookingBVH.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/cooking/GuCookingConvexHullBuilder.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/cooking/GuCookingConvexHullLib.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/cooking/GuCookingConvexHullUtils.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/cooking/GuCookingConvexMesh.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/cooking/GuCookingConvexMeshBuilder.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/cooking/GuCookingConvexPolygonsBuilder.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/cooking/GuCookingHF.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/cooking/GuCookingQuickHullConvexHullLib.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/cooking/GuCookingTetrahedronMesh.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/cooking/GuCookingTriangleMesh.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/cooking/GuCookingVolumeIntegration.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/cooking/GuRTreeCooking.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/distance/GuDistancePointBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/distance/GuDistancePointTetrahedron.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/distance/GuDistancePointTriangle.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/distance/GuDistanceSegmentBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/distance/GuDistanceSegmentSegment.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/distance/GuDistanceSegmentTriangle.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/distance/GuDistanceTriangleTriangle.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/gjk/GuEPA.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/gjk/GuGJKSimplex.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/gjk/GuGJKTest.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuAABBPruner.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuAABBTree.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuAABBTreeUpdateMap.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuActorShapeMap.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuBounds.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuBucketPruner.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuBVH.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuCapsule.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuCCTSweepTests.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuCookingSDF.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuExtendedBucketPruner.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuFactory.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuGeometryQuery.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuGjkQuery.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuIncrementalAABBPruner.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuIncrementalAABBPrunerCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuIncrementalAABBTree.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuInternal.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuMaverickNode.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuMeshFactory.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuMetaData.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuMTD.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuOverlapTests.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuPruningPool.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuQuerySystem.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuRaycastTests.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuSAH.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuSDF.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuSecondaryPruner.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuSqInternal.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuSweepMTD.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuSweepSharedTests.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuSweepTests.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/GuWindingNumber.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/hf/GuHeightField.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/hf/GuHeightFieldUtil.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/hf/GuOverlapTestsHF.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/hf/GuSweepsHF.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/intersection/GuIntersectionBoxBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/intersection/GuIntersectionCapsuleTriangle.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/intersection/GuIntersectionEdgeEdge.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/intersection/GuIntersectionRayBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/intersection/GuIntersectionRayCapsule.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/intersection/GuIntersectionRaySphere.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/intersection/GuIntersectionSphereBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/intersection/GuIntersectionTetrahedronBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/intersection/GuIntersectionTriangleBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/intersection/GuIntersectionTriangleTriangle.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuBV32.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuBV32Build.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuBV4.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuBV4Build.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuBV4_AABBSweep.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuBV4_BoxOverlap.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuBV4_CapsuleSweep.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuBV4_CapsuleSweepAA.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuBV4_MeshMeshOverlap.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuBV4_OBBSweep.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuBV4_Raycast.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuBV4_SphereOverlap.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuBV4_SphereSweep.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuMeshQuery.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuMidphaseBV4.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuMidphaseRTree.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuOverlapTestsMesh.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuRTree.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuRTreeQueries.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuSweepsMesh.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuTetrahedronMesh.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuTetrahedronMeshUtils.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuTriangleMesh.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuTriangleMeshBV4.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/mesh/GuTriangleMeshRTree.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactBoxBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactBoxConvex.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactCapsuleBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactCapsuleCapsule.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactCapsuleConvex.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactCapsuleHeightField.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactCapsuleMesh.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactConvexCommon.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactConvexConvex.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactConvexHeightField.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactConvexMesh.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactCustomGeometry.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactGenBoxConvex.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactGenSphereCapsule.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactGenUtil.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactPlaneBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactPlaneCapsule.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactPlaneConvex.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactSphereBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactSphereCapsule.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactSphereConvex.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactSphereHeightField.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactSphereMesh.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactSpherePlane.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMContactSphereSphere.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMShapeConvex.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPCMTriangleContactGen.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/pcm/GuPersistentContactManifold.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/sweep/GuSweepBoxBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/sweep/GuSweepBoxSphere.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/sweep/GuSweepBoxTriangle_FeatureBased.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/sweep/GuSweepBoxTriangle_SAT.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/sweep/GuSweepCapsuleBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/sweep/GuSweepCapsuleCapsule.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/sweep/GuSweepCapsuleTriangle.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/sweep/GuSweepSphereCapsule.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/sweep/GuSweepSphereSphere.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/sweep/GuSweepSphereTriangle.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/geomutils/src/sweep/GuSweepTriangleUtils.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/immediatemode/src/NpImmediateMode.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/api/src/px_globals.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/common/src/pipeline/PxcContactCache.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/common/src/pipeline/PxcContactMethodImpl.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/common/src/pipeline/PxcMaterialMethodImpl.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/common/src/pipeline/PxcNpBatch.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/common/src/pipeline/PxcNpCacheStreamPair.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/common/src/pipeline/PxcNpContactPrepShared.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/common/src/pipeline/PxcNpMemBlockPool.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/common/src/pipeline/PxcNpThreadContext.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/software/src/PxsCCD.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/software/src/PxsContactManager.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/software/src/PxsContext.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/software/src/PxsDefaultMemoryManager.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/software/src/PxsIslandSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/software/src/PxsNphaseImplementationContext.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevel/software/src/PxsSimpleIslandManager.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevelaabb/src/BpAABBManager.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevelaabb/src/BpAABBManagerBase.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevelaabb/src/BpBroadPhase.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevelaabb/src/BpBroadPhaseABP.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevelaabb/src/BpBroadPhaseMBP.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevelaabb/src/BpBroadPhaseSap.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevelaabb/src/BpBroadPhaseSapAux.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevelaabb/src/BpBroadPhaseShared.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevelaabb/src/BpBroadPhaseUpdate.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowlevelaabb/src/BpFiltering.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyArticulationContactPrep.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyArticulationContactPrepPF.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyConstraintPartition.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyConstraintSetup.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyConstraintSetupBlock.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyContactPrep.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyContactPrep4.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyContactPrep4PF.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyContactPrepPF.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyDynamics.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyFeatherstoneArticulation.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyFeatherstoneForwardDynamic.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyFeatherstoneInverseDynamic.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyFrictionCorrelation.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyRigidBodyToSolverBody.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DySleep.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DySolverConstraints.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DySolverConstraintsBlock.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DySolverControl.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DySolverControlPF.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DySolverPFConstraints.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DySolverPFConstraintsBlock.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyTGSContactPrep.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyTGSContactPrepBlock.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyTGSDynamics.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyThreadContext.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/lowleveldynamics/src/DyThresholdTable.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/gpu/PxGpu.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/gpu/PxPhysXGpuModuleLoader.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpActor.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpAggregate.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpArticulationJointReducedCoordinate.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpArticulationLink.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpArticulationReducedCoordinate.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpArticulationSensor.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpArticulationTendon.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpBounds.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpCheck.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpConstraint.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpDebugViz.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpFactory.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpFEMCloth.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpFEMClothMaterial.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpFEMSoftBodyMaterial.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpFLIPMaterial.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpHairSystem.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpMaterial.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpMetaData.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpMPMMaterial.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpParticleSystem.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpPBDMaterial.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpPhysics.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpPruningStructure.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpPvdSceneClient.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpPvdSceneQueryCollector.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpRigidDynamic.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpRigidStatic.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpScene.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpSceneFetchResults.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpSceneQueries.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpSerializerAdapter.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpShape.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpShapeManager.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/NpSoftBody.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/omnipvd/NpOmniPvd.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/omnipvd/NpOmniPvdRegistrationData.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/omnipvd/OmniPvdChunkAlloc.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/omnipvd/OmniPvdPxSampler.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/PvdMetaDataPvdBinding.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physx/src/PvdPhysicsClient.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxcharacterkinematic/src/CctBoxController.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxcharacterkinematic/src/CctCapsuleController.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxcharacterkinematic/src/CctCharacterController.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxcharacterkinematic/src/CctCharacterControllerCallbacks.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxcharacterkinematic/src/CctCharacterControllerManager.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxcharacterkinematic/src/CctController.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxcharacterkinematic/src/CctObstacleContext.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxcharacterkinematic/src/CctSweptBox.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxcharacterkinematic/src/CctSweptCapsule.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxcharacterkinematic/src/CctSweptVolume.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxcooking/src/Cooking.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtBroadPhase.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtCollection.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtContactJoint.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtConvexMeshExt.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtCpuWorkerThread.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtCustomGeometryExt.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtCustomSceneQuerySystem.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtD6Joint.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtD6JointCreate.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtDefaultCpuDispatcher.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtDefaultErrorCallback.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtDefaultSimulationFilterShader.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtDefaultStreams.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtDistanceJoint.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtExtensions.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtFixedJoint.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtGearJoint.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtGjkQueryExt.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtJoint.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtMetaData.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtParticleClothCooker.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtPrismaticJoint.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtPvd.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtPxStringTable.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtRackAndPinionJoint.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtRaycastCCD.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtRemeshingExt.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtRevoluteJoint.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtRigidActorExt.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtRigidBodyExt.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtSampling.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtSceneQueryExt.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtSceneQuerySystem.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtSimpleFactory.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtSmoothNormals.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtSoftBodyExt.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtSphericalJoint.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtSqManager.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtSqQuery.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtTetMakerExt.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtTetrahedronMeshExt.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/ExtTriangleMeshExt.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/omnipvd/ExtOmniPvdRegistrationData.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/omnipvd/OmniPvdPxExtensionsSampler.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Binary/SnBinaryDeserialization.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Binary/SnBinarySerialization.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Binary/SnConvX.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Binary/SnConvX_Align.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Binary/SnConvX_Convert.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Binary/SnConvX_Error.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Binary/SnConvX_MetaData.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Binary/SnConvX_Output.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Binary/SnConvX_Union.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Binary/SnSerializationContext.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/SnSerialization.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/SnSerializationRegistry.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/SnSerialUtils.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Xml/SnJointRepXSerializer.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Xml/SnRepXCoreSerializer.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Xml/SnRepXUpgrader.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/serialization/Xml/SnXmlSerialization.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/tet/ExtBVH.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/tet/ExtDelaunayBoundaryInserter.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/tet/ExtDelaunayTetrahedralizer.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/tet/ExtFastWindingNumber.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/tet/ExtInsideTester.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/tet/ExtMeshSimplificator.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/tet/ExtOctreeTetrahedralizer.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/tet/ExtRemesher.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/tet/ExtTetSplitting.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/tet/ExtTetUnionFind.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/tet/ExtUtilities.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxextensions/src/tet/ExtVoxelTetrahedralizer.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxmetadata/core/src/PxAutoGeneratedMetaDataObjects.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxmetadata/core/src/PxMetaDataObjects.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxmetadata/extensions/src/PxExtensionAutoGeneratedMetaDataObjects.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/physxmetadata/src/PxVehicleAutoGeneratedMetaDataObjects.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/physxmetadata/src/PxVehicleMetaDataObjects.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/PxVehicleComponents.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/PxVehicleDrive.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/PxVehicleDrive4W.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/PxVehicleDriveNW.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/PxVehicleDriveTank.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/PxVehicleMetaData.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/PxVehicleNoDrive.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/PxVehicleSDK.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/PxVehicleSerialization.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/PxVehicleSuspWheelTire4.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/PxVehicleTireFriction.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/PxVehicleUpdate.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/PxVehicleWheels.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/VehicleUtilControl.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/VehicleUtilSetup.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle/src/VehicleUtilTelemetry.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/commands/VhCommandHelpers.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/drivetrain/VhDrivetrainFunctions.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/drivetrain/VhDrivetrainHelpers.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/physxActor/VhPhysXActorFunctions.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/physxActor/VhPhysXActorHelpers.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/physxConstraints/VhPhysXConstraintFunctions.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/physxConstraints/VhPhysXConstraintHelpers.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/physxRoadGeometry/VhPhysXRoadGeometryFunctions.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/physxRoadGeometry/VhPhysXRoadGeometryHelpers.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/pvd/VhPvdFunctions.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/pvd/VhPvdHelpers.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/pvd/VhPvdWriter.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/rigidBody/VhRigidBodyFunctions.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/steering/VhSteeringFunctions.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/suspension/VhSuspensionFunctions.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/suspension/VhSuspensionHelpers.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/tire/VhTireFunctions.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/tire/VhTireHelpers.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/physxvehicle2/src/wheel/VhWheelFunctions.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/pvd/src/PxProfileEventImpl.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/pvd/src/PxPvd.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/pvd/src/PxPvdDataStream.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/pvd/src/PxPvdDefaultFileTransport.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/pvd/src/PxPvdDefaultSocketTransport.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/pvd/src/PxPvdImpl.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/pvd/src/PxPvdMemClient.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/pvd/src/PxPvdObjectModelMetaData.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/pvd/src/PxPvdObjectRegistrar.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/pvd/src/PxPvdProfileZoneClient.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/pvd/src/PxPvdUserRenderer.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/scenequery/src/SqCompoundPruner.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/scenequery/src/SqCompoundPruningPool.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/scenequery/src/SqFactory.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/scenequery/src/SqManager.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/scenequery/src/SqQuery.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScActorCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScActorSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScArticulationCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScArticulationJointCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScArticulationJointSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScArticulationSensorSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScArticulationSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScArticulationTendonCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScArticulationTendonJointCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScArticulationTendonSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScBodyCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScBodySim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScBroadphase.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScCCD.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScConstraintBreakage.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScConstraintCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScConstraintInteraction.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScConstraintSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScElementInteractionMarker.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScElementSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScFEMClothCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScFEMClothShapeSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScFEMClothSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScFiltering.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScHairSystemCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScHairSystemShapeCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScHairSystemShapeSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScHairSystemSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScInteraction.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScIterators.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScKinematics.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScMetaData.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScNPhaseCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScParticleSystemCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScParticleSystemShapeCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScParticleSystemShapeSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScParticleSystemSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScPhysics.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScPipeline.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScRigidCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScRigidSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScScene.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScShapeCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScShapeInteraction.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScShapeSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScShapeSimBase.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScSimStats.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScSimulationController.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScSleep.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScSoftBodyCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScSoftBodyShapeSim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScSoftBodySim.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScSqBoundsManager.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScStaticCore.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScTriggerInteraction.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/simulationcontroller/src/ScVisualize.cpp \
PhysX/PhysX-105.1-physx-5.3.0/physx/source/task/src/TaskManager.cpp 

MUJOCOSRC = \
MuJoCo/mujoco-3.0.1/src/engine/engine_callback.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_collision_box.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_collision_convex.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_collision_driver.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_collision_primitive.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_collision_sdf.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_core_constraint.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_core_smooth.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_crossplatform.cc \
MuJoCo/mujoco-3.0.1/src/engine/engine_derivative.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_derivative_fd.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_forward.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_inverse.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_island.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_io.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_passive.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_plugin.cc \
MuJoCo/mujoco-3.0.1/src/engine/engine_print.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_ray.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_resource.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_sensor.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_setconst.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_solver.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_support.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_util_blas.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_util_errmem.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_util_misc.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_util_solve.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_util_sparse.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_util_spatial.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_vfs.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_vis_init.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_vis_interact.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_vis_state.c \
MuJoCo/mujoco-3.0.1/src/engine/engine_vis_visualize.c \
MuJoCo/mujoco-3.0.1/src/thread/thread_pool.cc \
MuJoCo/mujoco-3.0.1/src/thread/thread_task.cc \
MuJoCo/mujoco-3.0.1/src/user/user_composite.cc \
MuJoCo/mujoco-3.0.1/src/user/user_flexcomp.cc \
MuJoCo/mujoco-3.0.1/src/user/user_mesh.cc \
MuJoCo/mujoco-3.0.1/src/user/user_model.cc \
MuJoCo/mujoco-3.0.1/src/user/user_objects.cc \
MuJoCo/mujoco-3.0.1/src/user/user_util.cc \
MuJoCo/mujoco-3.0.1/src/xml/xml_api.cc \
MuJoCo/mujoco-3.0.1/src/xml/xml_base.cc \
MuJoCo/mujoco-3.0.1/src/xml/xml.cc \
MuJoCo/mujoco-3.0.1/src/xml/xml_native_reader.cc \
MuJoCo/mujoco-3.0.1/src/xml/xml_numeric_format.cc \
MuJoCo/mujoco-3.0.1/src/xml/xml_native_writer.cc \
MuJoCo/mujoco-3.0.1/src/xml/xml_urdf.cc \
MuJoCo/mujoco-3.0.1/src/xml/xml_util.cc \
MuJoCo/mujoco-3.0.1/build/_deps/ccd-src/src/ccd.c \
MuJoCo/mujoco-3.0.1/build/_deps/ccd-src/src/support.c \
MuJoCo/mujoco-3.0.1/build/_deps/ccd-src/src/mpr.c \
MuJoCo/mujoco-3.0.1/build/_deps/ccd-src/src/vec3.c \
MuJoCo/mujoco-3.0.1/build/_deps/ccd-src/src/polytope.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/accessors_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/geom2_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/geom_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/global_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/io_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/libqhull_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/mem_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/merge_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/poly2_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/poly_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/qset_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/random_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/rboxlib_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/stat_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/user_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/usermem_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/userprintf_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/qhull-src/src/libqhull_r/userprintf_rbox_r.c \
MuJoCo/mujoco-3.0.1/build/_deps/lodepng-src/lodepng.cpp \
MuJoCo/mujoco-3.0.1/build/_deps/tinyxml2-src/tinyxml2.cpp \
MuJoCo/mujoco-3.0.1/build/_deps/tinyobjloader-src/tiny_obj_loader.cc


GAITSYMOBJ = $(addsuffix .o, $(basename $(GAITSYMSRC) ) )
GAITSYMHEADER = $(addsuffix .h, $(basename $(GAITSYMSRC) ) ) PGDMath.h SimpleStrap.h SmartEnum.h MPIStuff.h TCPIPMessage.h

LIBCCDOBJ = $(addsuffix .o, $(basename $(LIBCCDSRC) ) )
ODEOBJ = $(addsuffix .o, $(basename $(ODESRC) ) )
ODEJOINTSOBJ = $(addsuffix .o, $(basename $(ODEJOINTSSRC) ) )
OPCODEICEOBJ = $(addsuffix .o, $(basename $(OPCODEICESRC) ) )
OPCODEOBJ = $(addsuffix .o, $(basename $(OPCODESRC) ) )
PYSTRINGOBJ = $(addsuffix .o, $(basename $(PYSTRINGSRC) ) )
PHYSXOBJ = $(addsuffix .o, $(basename $(PHYSXSRC) ) )
MUJOCOOBJ = $(addsuffix .o, $(basename $(MUJOCOSRC) ) )

BINARIES = bin/gaitsym_2019_asio_async bin/gaitsym_2019

all: directories binaries

directories: bin obj

binaries: $(BINARIES)

obj:
	-mkdir obj
	-mkdir obj/cl
	-mkdir obj/libccd
	-mkdir obj/ode
	-mkdir obj/odejoints
	-mkdir obj/opcodeice
	-mkdir obj/opcode
	-mkdir obj/pystring
	-mkdir obj/asio_async
	-mkdir obj/physx
	-mkdir obj/mujoco

bin:
	-mkdir bin

obj/cl/%.o : src/%.cpp
	$(CXX) -DUSE_CL $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

obj/libccd/%.o : ode-0.15/libccd/src/%.c
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

obj/ode/%.o : ode-0.15/ode/src/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

obj/ode/%.o : ode-0.15/ode/src/%.c
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

obj/odejoints/%.o : ode-0.15/ode/src/joints/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

obj/opcodeice/%.o : ode-0.15/OPCODE/Ice/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

obj/opcode/%.o : ode-0.15/OPCODE/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

obj/pystring/%.o : pystring/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

obj/asio_async/%.o : src/%.cpp
	$(CXX) -DUSE_ASIO_ASYNC $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

obj/physx/%.o : src/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

obj/mujoco/%.o : src/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

bin/gaitsym_2019: $(addprefix obj/cl/, $(GAITSYMOBJ) ) $(addprefix obj/ode/, $(ODEOBJ) ) \
$(addprefix obj/odejoints/, $(ODEJOINTSOBJ) ) $(addprefix obj/opcodeice/, $(OPCODEICEOBJ) ) $(addprefix obj/opcode/, $(OPCODEOBJ) ) \
$(addprefix obj/pystring/, $(PYSTRINGOBJ) ) $(addprefix obj/physx/, $(PHYSXOBJ) ) $(addprefix obj/mujoco/, $(MUJOCOOBJ) )
	$(CXX) -DUSE_CL $(LDFLAGS) -o $@ $^ $(LIBS)

bin/gaitsym_2019_asio_async: $(addprefix obj/asio_async/, $(GAITSYMOBJ) ) $(addprefix obj/ode/, $(ODEOBJ) ) \
$(addprefix obj/odejoints/, $(ODEJOINTSOBJ) ) $(addprefix obj/opcodeice/, $(OPCODEICEOBJ) ) $(addprefix obj/opcode/, $(OPCODEOBJ) ) \
$(addprefix obj/pystring/, $(PYSTRINGOBJ) ) $(addprefix obj/physx/, $(PHYSXOBJ) ) $(addprefix obj/mujoco/, $(MUJOCOOBJ) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(SOCKET_LIBS) $(LIBS)

clean:
	rm -rf obj bin
	rm -rf distribution*
	rm -rf build*

superclean:
	rm -rf obj bin
	rm -rf distribution*
	rm -rf build*
	rm -rf GaitSymQt/GaitSym*.pro.user*
	find . -name '.DS_Store' -exec rm -f {} \;
	find . -name '.gdb_history' -exec rm -f {} \;
	find . -name '.#*' -exec rm -f {} \;
	find . -name '*~' -exec rm -f {} \;
	find . -name '#*' -exec rm -f {} \;
	find . -name '*.bak' -exec rm -f {} \;
	find . -name '*.bck' -exec rm -f {} \;
	find . -name '*.tmp' -exec rm -f {} \;
	find . -name '*.o' -exec rm -f {} \;

distribution: distribution_dirs gaitsym_distribution gaitsym_distribution_extras

distribution_dirs:
	rm -rf distribution distribution.zip
	-mkdir distribution
	-mkdir distribution/src

gaitsym_distribution: $(addprefix distribution/src/, $(GAITSYMSRC)) $(addprefix distribution/src/, $(GAITSYMHEADER))

$(addprefix distribution/src/, $(GAITSYMSRC)):
	scripts/strip_ifdef.py EXPERIMENTAL $(addprefix src/, $(notdir $@)) $@

$(addprefix distribution/src/, $(GAITSYMHEADER)):
	scripts/strip_ifdef.py EXPERIMENTAL $(addprefix src/, $(notdir $@)) $@

gaitsym_distribution_extras:
	cp -rf asio-1.18.2 distribution/
	cp -rf exprtk distribution/
	cp -rf GaitSymQt distribution/
	cp -rf libgwavi distribution/
	cp -rf ode-0.15 distribution/
	cp -rf pystring distribution/
	cp -rf rapidxml-1.13 distribution/
	cp -rf tinyply distribution/
	cp -rf glextrusion distribution/
	cp -rf scripts distribution/
	cp -rf python distribution/
	cp -rf omniverse distribution/
	cp makefile distribution/
	find distribution -depth -type d -name CVS -print -exec rm -rf {} \;
	rm -rf distribution/GaitSymQt/GaitSym*.pro.*
	rm -rf distribution/python/GaitSym*.pro.*
	rm -rf distribution/python/obj
	rm -rf distribution/python/*.so
	rm -rf distribution/python/*.dll
	rm -rf distribution/python/*.pyd
	zip -r distribution.zip distribution


# note makedepend might need to be installed and it does not find all the system include paths
# "gcc -xc++ -E -v -" will list what these are for gcc but in fact it is not very important
# or "echo | gcc -Wp,-v -x c++ - -fsyntax-only" which produces a shorter output
# because they never change or least not very often. It is mostly the C++ includes that are
# not found and it might be interesting to know what they are but that is all
GCC_INCLUDE_PATH = \
 /usr/include/c++/9 \
 /usr/include/x86_64-linux-gnu/c++/9 \
 /usr/include/c++/9/backward \
 /usr/lib/gcc/x86_64-linux-gnu/9/include \
 /usr/local/include \
 /usr/include/x86_64-linux-gnu \
 /usr/include

depend:
	makedepend -fmakefile -- $(CFLAGS) $(INC_DIRS) -- $(addprefix -I, $(GCC_INCLUDE_PATH) ) $(addprefix src/, $(GAITSYMSRC) )

# DO NOT DELETE

