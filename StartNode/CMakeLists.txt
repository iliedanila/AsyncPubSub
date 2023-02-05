cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 17)

project(startNode)

find_package(Boost REQUIRED date_time regex system serialization)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

include_directories(inc)
include_directories(../MeshNetwork/inc)
include_directories(../LogicalLayer/inc)

# sources
file (GLOB SOURCES "src/*.cpp")

# execs
add_executable(startNode ${SOURCES})

# libraries
target_link_libraries(startNode LogicalLayer MeshNetwork)

if(UNIX)
    target_link_libraries(startNode Boost::date_time)
    target_link_libraries(startNode Boost::regex)
    target_link_libraries(startNode Boost::serialization)
    target_link_libraries(startNode Boost::system)
    target_link_libraries(startNode pthread)
endif()

