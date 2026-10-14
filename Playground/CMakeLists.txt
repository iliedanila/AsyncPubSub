add_executable(playground src/main.cpp)
# It runs the io_context on a std::thread.
target_link_libraries(playground
    PRIVATE LogicalLayer Boost::headers Threads::Threads AsyncPubSubWarnings)

if(BUILD_TESTING)
    # Uses the playground's fixed ports 7777 and 8888.
    add_test(NAME cli.playgroundExitsOnEof
             COMMAND sh -c "$<TARGET_FILE:playground> < /dev/null")
    set_tests_properties(cli.playgroundExitsOnEof PROPERTIES
        TIMEOUT 15 RUN_SERIAL TRUE)
endif()
