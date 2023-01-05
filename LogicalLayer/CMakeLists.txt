cmake_minimum_required(VERSION 3.5)

project(LogicalLayer)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

set (CMAKE_CXX_STANDARD 17)

include_directories(inc)
include_directories(../MeshNetwork/inc)
# include_directories(../MeshNetwork/boost/inc)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "inc/*.hpp")

add_library(LogicalLayer STATIC ${HEADERS} ${SOURCES})
