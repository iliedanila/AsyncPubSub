cmake_minimum_required(VERSION 2.8.9)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message("debug mode")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES arm)
    message("arm mode")
endif()

# the complete system name, e.g. "Linux-2.4.22", "FreeBSD-5.4-RELEASE" or "Windows 5.1"
MESSAGE( STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM} )
# the short system name, e.g. "Linux", "FreeBSD" or "Windows"
MESSAGE( STATUS "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME} )
# only the version part of CMAKE_SYSTEM
MESSAGE( STATUS "CMAKE_SYSTEM_VERSION: " ${CMAKE_SYSTEM_VERSION} )
# the processor name (e.g. "Intel(R) Pentium(R) M processor 2.00GHz")
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR} )

project(playground)
project(startNode)

set (CMAKE_CXX_STANDARD 14) # C++ 14

include_directories(startNode/inc)
include_directories(MeshNetwork/inc)
# include_directories(MeshNetwork/boost/inc)
#include_directories(c:/boost/include/boost-1_66)
include_directories(/Users/ilie/workspace/boost/include)
include_directories(LogicalLayer/inc)

# project dependencies
add_subdirectory(LogicalLayer)
add_subdirectory(MeshNetwork)

# link path
if(APPLE)
#    link_directories(MeshNetwork/boost/lib/macos/x64_release)
    link_directories(/Users/ilie/workspace/boost/lib)
elseif(UNIX)
    link_directories(MeshNetwork/boost/lib/linux/x64_release)
elseif(WIN32)
    # link_directories(MeshNetwork/boost/lib/win/x32/debug) # for some reason the default is debug
    link_directories(c:/boost/lib)
endif()

# sources
file (GLOB SOURCES_PLAYGROUND "playground/src/*.cpp")
file (GLOB SOURCES_STARTNODE "startNode/src/*.cpp")

# execs
add_executable(playground ${SOURCES_PLAYGROUND})
add_executable(startNode ${SOURCES_STARTNODE})

# libraries
target_link_libraries(playground LogicalLayer MeshNetwork)
target_link_libraries(startNode LogicalLayer MeshNetwork)

if(UNIX)
    target_link_libraries(playground boost_date_time)
    target_link_libraries(playground boost_regex)
    target_link_libraries(playground boost_serialization)
    target_link_libraries(playground boost_system)

    target_link_libraries(startNode boost_date_time)
    target_link_libraries(startNode boost_regex)
    target_link_libraries(startNode boost_serialization)
    target_link_libraries(startNode boost_system)

    target_link_libraries(playground pthread)
    target_link_libraries(startNode pthread)
endif()