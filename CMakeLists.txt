cmake_minimum_required(VERSION 3.8)

project(PathFinding)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-std=c++14")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
IF(MSVC)
    SET(CMAKE_CXX_FLAGS "/EHsc")
ENDIF(MSVC)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(include)

set(BUILD_SHARED_LIBS FALSE)
set(SFML_BUILD_NETWORK FALSE)
set(SFML_USE_STATIC_STD_LIBS TRUE)
add_subdirectory(ext/SFML)
include_directories(ext/SFML/include)
include_directories(${SFML_INCLUDE_DIR})
file(COPY src/random64_4_1485816605.dot DESTINATION ${CMAKE_BINARY_DIR})
file(COPY src/arial.ttf DESTINATION ${CMAKE_BINARY_DIR})
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