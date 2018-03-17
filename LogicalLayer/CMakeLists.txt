cmake_minimum_required(VERSION 2.8.9)

project(LogicalLayer)

set (CMAKE_CXX_STANDARD 14)

include_directories(inc)
include_directories(../MeshNetwork/inc)
include_directories(../MeshNetwork/boost/inc)

file(GLOB SOURCES "src/*.cpp")

add_library(LogicalLayer STATIC ${SOURCES})
