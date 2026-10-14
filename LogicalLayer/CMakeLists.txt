add_library(LogicalLayer STATIC
    src/broker.cpp
    src/publisher.cpp
    src/subscriber.cpp
    inc/addRemoveSubscription.hpp
    inc/allMessages.hpp
    inc/broker.hpp
    inc/brokerIdentity.hpp
    inc/common.hpp
    inc/messageDecoder.hpp
    inc/publisher.hpp
    inc/publisherData.hpp
    inc/publisherIdentity.hpp
    inc/startPublish.hpp
    inc/stopPublish.hpp
    inc/subscriber.hpp
    inc/subscriptionData.hpp
)
# The headers use MeshNetwork's types and Boost.Serialization.
target_include_directories(LogicalLayer PUBLIC inc)
target_compile_features(LogicalLayer PUBLIC cxx_std_20)
target_link_libraries(LogicalLayer PUBLIC MeshNetwork Boost::serialization)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
