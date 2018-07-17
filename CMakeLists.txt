cmake_minimum_required(VERSION 3.8)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.22.32.tar.gz"
    SHA1 "37cc115ebce13ee279cc1f086d7203394e2e959a"
)
project(PathFinding)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-std=c++14")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(include)

add_subdirectory(ext/SFML)
include_directories(ext/SFML/include)
include_directories(${SFML_INCLUDE_DIR})

set(PATHFINDING_H
	./include/GraphSolver.h
)

set (PATHFINDING_CPP
	./src/GraphSolver.cpp
)

source_group("Header Files" FILES ${PATHFINDING_H})
source_group("Source Files" FILES ${PATHFINDING_CPP})
include_directories(
	${PATHFINDING_H}
	${PATHFINDING_CPP}
)
add_executable(PathFinding
	src/Main.cpp
	${PATHFINDING_H}
	${PATHFINDING_CPP}
)

target_link_libraries(PathFinding PUBLIC sfml-graphics sfml-window sfml-system sfml-audio sfml-main)

if(WIN32)
	set_target_properties(PathFinding PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif()