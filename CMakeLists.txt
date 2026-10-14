cmake_minimum_required(VERSION 3.25...4.4)

project(asyncPubSub LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# No C++20 modules here, so skip the dependency scan CMake 3.28+ runs on
# every C++20 source.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

find_package(Boost CONFIG REQUIRED COMPONENTS serialization)
find_package(Threads REQUIRED)

include(CTest)

add_subdirectory(MeshNetwork)
add_subdirectory(LogicalLayer)
add_subdirectory(StartNode)
add_subdirectory(Playground)
