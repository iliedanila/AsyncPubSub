cmake_minimum_required(VERSION 2.8.9)

project(playground)
project(startNode)

set (CMAKE_CXX_STANDARD 14)

include_directories(startNode/inc)
include_directories(MeshNetwork/inc)
include_directories(MeshNetwork/boost/inc)
include_directories(LogicalLayer/inc)

add_subdirectory(LogicalLayer)
add_subdirectory(MeshNetwork)

link_directories(MeshNetwork/boost/lib/linux/x64_release)

file (GLOB SOURCES_PLAYGROUND "playground/src/*.cpp")
file (GLOB SOURCES_STARTNODE "startNode/src/*.cpp")

add_executable(playground ${SOURCES_PLAYGROUND})
add_executable(startNode ${SOURCES_STARTNODE})

target_link_libraries(playground LogicalLayer MeshNetwork)
target_link_libraries(startNode LogicalLayer MeshNetwork)