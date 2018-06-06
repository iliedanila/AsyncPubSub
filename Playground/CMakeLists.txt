cmake_minimum_required(VERSION 2.8.9)

set (CMAKE_CXX_STANDARD 14) # C++ 14

project(playground)

include_directories(../MeshNetwork/inc)
include_directories(../MeshNetwork/boost/inc)
include_directories(../LogicalLayer/inc)

# link path
if(APPLE)
    if (CMAKE_BUILD_TYPE MATCHES Release)
        link_directories(../MeshNetwork/boost/lib/macos/x64_release)
    elseif (CMAKE_BUILD_TYPE MATCHES Debug)
        link_directories(../MeshNetwork/boost/lib/macos/x64_debug)
    endif()
elseif(UNIX)
    if (CMAKE_SYSTEM_PROCESSOR MATCHES armv7l)
        if (CMAKE_BUILD_TYPE MATCHES Release)
            link_directories(../MeshNetwork/boost/lib/arm/release)
        elseif (CMAKE_BUILD_TYPE MATCHES Debug)
            link_directories(../MeshNetwork/boost/lib/arm/debug)
        endif()
    else()
        if (CMAKE_BUILD_TYPE MATCHES Release)
            link_directories(../MeshNetwork/boost/lib/linux/x64_release)
        elseif (CMAKE_BUILD_TYPE MATCHES Debug)
            link_directories(../MeshNetwork/boost/lib/linux/x64_debug)
        endif()
    endif()
elseif(WIN32)
    link_directories(../MeshNetwork/boost/lib/win/x32/debug) # for some reason the default is debug
endif()

# sources
file (GLOB SOURCES "src/*.cpp")

# execs
add_executable(playground ${SOURCES})

# libraries
target_link_libraries(playground LogicalLayer MeshNetwork)

if(UNIX)
    target_link_libraries(playground boost_date_time)
    target_link_libraries(playground boost_regex)
    target_link_libraries(playground boost_serialization)
    target_link_libraries(playground boost_system)
    target_link_libraries(playground pthread)
endif()
