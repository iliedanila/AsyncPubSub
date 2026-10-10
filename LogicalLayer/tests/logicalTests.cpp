#include <memory>
#include <string>

#include "broker.hpp"
#include "node.hpp"
#include "publisher.hpp"
#include "publisherData.hpp"
#include "subscriber.hpp"
#include "subscriptionData.hpp"
#include "testSupport.hpp"

using namespace MeshTest;
using boost::asio::io_context;
using LogicalLayer::Broker;
using LogicalLayer::Publisher;
using LogicalLayer::Subscriber;
using NetworkLayer::Node;

namespace {

void fillWithDummyData(LogicalLayer::PublisherData& data) {
    for (auto& [dataType, value] : data.getData()) {
        value = "value";
    }
}

void garbagePayloadIsDropped() {
    io_context io;
    Node brokerNode("broker", io), publisherNode("pub", io),
        subscriberNode("sub", io), sender("X", io);
    Broker broker(brokerNode);
    Publisher publisher(publisherNode, {"t"});
    Subscriber subscriber(subscriberNode);
    brokerNode.accept(47210);
    for (Node* node : {&publisherNode, &subscriberNode, &sender}) {
        node->connect("localhost", 47210);
    }
    MESH_CHECK(runUntil(io, [&] {
        return sender.isNodeAccessible("broker") &&
               sender.isNodeAccessible("pub") && sender.isNodeAccessible("sub");
    }));

    int acknowledged = 0;
    for (const char* target : {"broker", "pub", "sub"}) {
        sender.sndMessage(target, "garbage",
                          [&](std::string, NetworkLayer::SendError error) {
                              if (error == NetworkLayer::eSuccess) {
                                  ++acknowledged;
                              }
                          });
    }
    MESH_CHECK(runUntil(io, [&] { return acknowledged == 3; }));
}

void publisherForgetsDepartedSubscriber() {
    CoutCapture out;
    io_context io;
    Node brokerNode("broker", io, true), publisherNode("pub", io),
        subscriberNode("sub", io);
    Broker broker(brokerNode);
    Publisher publisher(publisherNode, {"t"});
    Subscriber subscriber(subscriberNode);
    publisher.startPublishing(fillWithDummyData, 50);
    int deliveries = 0;
    subscriber.addSubscription(
        {"t"}, [&](LogicalLayer::SubscriptionData&) { ++deliveries; });
    brokerNode.accept(47220);
    publisherNode.connect("localhost", 47220);
    subscriberNode.connect("localhost", 47220);
    MESH_CHECK(runUntil(io, [&] { return deliveries > 0; }));

    subscriberNode.close();
    MESH_CHECK(runUntil(
        io, [&] { return out.contains("-- pub -- Subscriber sub left"); }, 3s));
}

// A broker and a publisher, started and stopped together.
struct BrokerAndPublisher {
    std::unique_ptr<Node> brokerNode;
    std::unique_ptr<Node> publisherNode;
    std::unique_ptr<Broker> broker;
    std::unique_ptr<Publisher> publisher;

    void start(io_context& io, unsigned short port) {
        brokerNode = std::make_unique<Node>("broker", io);
        broker = std::make_unique<Broker>(*brokerNode);
        brokerNode->accept(port);
        publisherNode = std::make_unique<Node>("pub", io);
        publisher = std::make_unique<Publisher>(
            *publisherNode, LogicalLayer::PublisherIdentityT{"t"});
        publisher->startPublishing(fillWithDummyData, 50);
        publisherNode->connect("localhost", port);
    }

    void stop(io_context& io) {
        publisher->stopPublishing();
        publisherNode->close();
        brokerNode->close();
        handlersRunIn(io, 300ms);
        publisher.reset();
        broker.reset();
        publisherNode.reset();
        brokerNode.reset();
    }
};

void subscriberRecoversAfterBrokerAndPublisherRestart() {
    io_context io;
    Node subscriberNode("sub", io);
    Subscriber subscriber(subscriberNode);
    int deliveries = 0;
    subscriber.addSubscription(
        {"t"}, [&](LogicalLayer::SubscriptionData&) { ++deliveries; });

    BrokerAndPublisher others;
    others.start(io, 47230);
    subscriberNode.connect("localhost", 47230, true);
    MESH_CHECK(runUntil(io, [&] { return deliveries > 0; }));

    others.stop(io);
    MESH_CHECK(runUntil(
        io, [&] { return !subscriberNode.isNodeAccessible("broker"); }));
    const int deliveriesBeforeRestart = deliveries;

    others.start(io, 47230);
    MESH_CHECK(
        runUntil(io, [&] { return deliveries > deliveriesBeforeRestart; }, 5s));
}

}  // namespace

int main(int argc, char** argv) {
    return runTests(
        {
            {"garbagePayloadIsDropped", garbagePayloadIsDropped},
            {"publisherForgetsDepartedSubscriber",
             publisherForgetsDepartedSubscriber},
            {"subscriberRecoversAfterBrokerAndPublisherRestart",
             subscriberRecoversAfterBrokerAndPublisherRestart},
        },
        argc, argv);
}
