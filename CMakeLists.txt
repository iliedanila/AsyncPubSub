cmake_minimum_required(VERSION 2.8.9)

set(CMAKE_BUILD_TYPE Debug)

project(playground)
project(startNode)

set (CMAKE_CXX_STANDARD 14)

include_directories(startNode/inc)
include_directories(MeshNetwork/inc)
include_directories(MeshNetwork/boost/inc)
include_directories(LogicalLayer/inc)

add_subdirectory(LogicalLayer)
add_subdirectory(MeshNetwork)

link_directories(MeshNetwork/boost/lib/macos/x64_debug)

file (GLOB SOURCES_PLAYGROUND "playground/src/*.cpp")
file (GLOB SOURCES_STARTNODE "startNode/src/*.cpp")

add_executable(playground ${SOURCES_PLAYGROUND})
add_executable(startNode ${SOURCES_STARTNODE})

target_link_libraries(playground LogicalLayer MeshNetwork)
target_link_libraries(playground libboost_date_time.a)
target_link_libraries(playground libboost_regex.a)
target_link_libraries(playground libboost_serialization.a)
target_link_libraries(playground libboost_system.a)

target_link_libraries(startNode LogicalLayer MeshNetwork)
target_link_libraries(startNode libboost_date_time.a)
target_link_libraries(startNode libboost_regex.a)
target_link_libraries(startNode libboost_serialization.a)
target_link_libraries(startNode libboost_system.a)