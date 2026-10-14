add_executable(logicalTests logicalTests.cpp)
target_link_libraries(logicalTests
    PRIVATE LogicalLayer MeshNetworkTestSupport AsyncPubSubWarnings)

set(LOGICAL_TESTS
    garbagePayloadIsDropped
    publisherForgetsDepartedSubscriber
    subscriberRecoversAfterBrokerAndPublisherRestart
    publisherKeepsSubscriberAcrossReroute
    deliveryResumesAfterBrokerReroute
)
foreach(test IN LISTS LOGICAL_TESTS)
    add_test(NAME logical.${test} COMMAND logicalTests ${test})
    set_tests_properties(logical.${test} PROPERTIES TIMEOUT 30)
endforeach()
