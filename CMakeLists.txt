cmake_minimum_required(VERSION 3.25...4.4)

project(asyncPubSub LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# No C++20 modules here, so skip the dependency scan CMake 3.28+ runs on
# every C++20 source.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# 1.84 is the first Boost whose serialization handles std::variant in
# boost/serialization/variant.hpp, the header that keeps the wire format.
find_package(Boost 1.84 CONFIG REQUIRED COMPONENTS serialization)
find_package(Threads REQUIRED)

# Warnings for this project's own targets (MeshNetwork sets its own); Boost's
# headers come in as system headers and stay quiet. The debug preset turns on
# CMAKE_COMPILE_WARNING_AS_ERROR so they fail the build.
add_library(AsyncPubSubWarnings INTERFACE)
target_compile_options(AsyncPubSubWarnings INTERFACE
    "$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall;-Wextra;-Wpedantic>")

include(CTest)

add_subdirectory(MeshNetwork)
add_subdirectory(LogicalLayer)
add_subdirectory(StartNode)
add_subdirectory(Playground)
