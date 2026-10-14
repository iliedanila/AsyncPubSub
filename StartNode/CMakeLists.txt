add_executable(startNode
    src/arguments.cpp
    src/main.cpp
    inc/arguments.hpp
)
target_include_directories(startNode PRIVATE inc)
target_link_libraries(startNode
    PRIVATE LogicalLayer Boost::headers AsyncPubSubWarnings)

if(BUILD_TESTING)
    add_test(NAME cli.missingFlagValue COMMAND startNode --accept 47300 --name)
    set_tests_properties(cli.missingFlagValue PROPERTIES
        PASS_REGULAR_EXPRESSION "Missing value for --name" TIMEOUT 10)
    add_test(NAME cli.publisherNeedsInterval
             COMMAND startNode --name p --as publisher --publish x)
    set_tests_properties(cli.publisherNeedsInterval PROPERTIES
        PASS_REGULAR_EXPRESSION "--interval MS is required" TIMEOUT 10)
endif()
