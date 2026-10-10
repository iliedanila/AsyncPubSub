cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 17) # C++ 17

project(playground)

find_package(Boost REQUIRED system serialization)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

include_directories(../MeshNetwork/inc)
include_directories(../LogicalLayer/inc)

# sources
file (GLOB SOURCES "src/*.cpp")

# execs
add_executable(playground ${SOURCES})

# libraries
target_link_libraries(playground LogicalLayer MeshNetwork)

if(UNIX)
    target_link_libraries(playground Boost::serialization)
    target_link_libraries(playground Boost::system)
    target_link_libraries(playground pthread)
endif()

if(BUILD_TESTING)
    # Uses the playground's fixed ports 7777 and 8888.
    add_test(NAME cli.playgroundExitsOnEof
             COMMAND sh -c "$<TARGET_FILE:playground> < /dev/null")
    set_tests_properties(cli.playgroundExitsOnEof PROPERTIES
        TIMEOUT 15 RUN_SERIAL TRUE)
endif()
