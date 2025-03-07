
add_library(kine INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/kine/Kine.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/kine/KineComponent.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/kine/KineLimit.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/kine/KineLink.hpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/kine/ik/CCDSolver.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/kine/ik/IKSolver.hpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/kine/joints/KineJoint.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/kine/joints/PrismaticJoint.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/kine/joints/RevoluteJoint.hpp"
)
target_include_directories(kine INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(pathfinding INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/pathfinding/Coordinate.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/pathfinding/Heuristic.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/pathfinding/Path.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/pathfinding/PathFinder.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/pathfinding/TileBasedMap.hpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/pathfinding/algorithm/AStar.hpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/pathfinding/heuristics/ClosestHeuristic.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/pathfinding/heuristics/ClosestSquaredHeuristic.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/pathfinding/heuristics/ManhattanHeuristic.hpp"
)
target_include_directories(pathfinding INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
