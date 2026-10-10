find_package(Boost REQUIRED COMPONENTS serialization)
find_package(Threads REQUIRED)

add_executable(logicalTests logicalTests.cpp)
target_include_directories(logicalTests PRIVATE ../../MeshNetwork/tests)
target_link_libraries(logicalTests LogicalLayer MeshNetwork
                      Boost::serialization Threads::Threads)

set(LOGICAL_TESTS
    garbagePayloadIsDropped
    publisherForgetsDepartedSubscriber
    subscriberRecoversAfterBrokerAndPublisherRestart
)
foreach(test IN LISTS LOGICAL_TESTS)
    add_test(NAME logical.${test} COMMAND logicalTests ${test})
    set_tests_properties(logical.${test} PROPERTIES TIMEOUT 30)
endforeach()
