cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 17)

project(startNode)

find_package(Boost REQUIRED serialization)
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
    target_link_libraries(startNode Boost::serialization)
    target_link_libraries(startNode pthread)
endif()


if(BUILD_TESTING)
    add_test(NAME cli.missingFlagValue COMMAND startNode --accept 47300 --name)
    set_tests_properties(cli.missingFlagValue PROPERTIES
        PASS_REGULAR_EXPRESSION "Missing value for --name" TIMEOUT 10)
    add_test(NAME cli.publisherNeedsInterval
             COMMAND startNode --name p --as publisher --publish x)
    set_tests_properties(cli.publisherNeedsInterval PROPERTIES
        PASS_REGULAR_EXPRESSION "--interval MS is required" TIMEOUT 10)
endif()
