#include "subscriber.hpp"
#include "../inc/allMessages.hpp"
#include "brokerIdentity.hpp"
#include "dataMessage.hpp"
#include "node.hpp"

// #include <boost/archive/text_iarchive.hpp>
// #include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <iostream>
#include <sstream>

namespace LogicalLayer {
Subscriber::Subscriber(NetworkLayer::Node& _node) : node(_node) {
    boost::asio::post(node.getIOService(), [this] {
        node.acceptMessages(std::bind(&Subscriber::handleIncomingMessage, this,
                                      std::placeholders::_1));
    });
}

Subscriber::~Subscriber() {}

void Subscriber::addSubscription(SubscriptionT subscription,
                                 SubscriptionDataHandlerT handler) {
    subscriptions.insert(std::make_pair(subscription, handler));
    sendNewSubscription(subscription);
}

void Subscriber::removeSubscription(SubscriptionT& subscription) {
    subscriptions.erase(subscription);
    sendRemoveSubscription(subscription);
}

void Subscriber::handleIncomingMessage(NetworkLayer::DataMessage& message) {
    std::stringstream ss(std::move(message.getBuffer()));
    boost::archive::binary_iarchive iarchive(ss);

    MessageVariant messageV;
    iarchive >> messageV;

    boost::apply_visitor(MessageVisitor<Subscriber>(*this), messageV);
}

void Subscriber::sendSubscription(
    const SubscriptionT& _subscription, const std::string& brokerName,
    AddRemoveSubscriptionMessage::Action action) const {
    AddRemoveSubscriptionMessage subscription(node.getName(), _subscription,
                                              action);
    MessageVariant messageV(subscription);
    std::stringstream ss;
    boost::archive::binary_oarchive oarchive(ss);
    oarchive << messageV;
    auto messageContent = ss.str();

    auto callback = std::bind(&Subscriber::handleBrokerAck, this,
                              std::placeholders::_1, std::placeholders::_2);

    boost::asio::post(node.getIOService(),
                      [this, brokerName, messageContent, callback] {
                          node.sndMessage(brokerName, messageContent, callback);
                      });
}

void Subscriber::sendNewSubscription(SubscriptionT& subscription) {
    for (auto& brokerName : brokers) {
        sendSubscription(subscription, brokerName,
                         AddRemoveSubscriptionMessage::eAdd);
    }
}

void Subscriber::sendRemoveSubscription(SubscriptionT& subscription) {
    for (auto& brokerName : brokers) {
        sendSubscription(subscription, brokerName,
                         AddRemoveSubscriptionMessage::eRemove);
    }
}

void Subscriber::sendAllSubscriptions(const std::string& brokerName) {
    if (subscriptions.empty())
        return;

    for (auto& subscription : subscriptions) {
        sendSubscription(subscription.first, brokerName,
                         AddRemoveSubscriptionMessage::eAdd);
    }
}

void Subscriber::handleNewBroker(BrokerIdentity& message) {
    node.log("New broker found: " + message.getBrokerName());
    if (brokers.find(message.getBrokerName()) != brokers.end())
        return;

    brokers.insert(message.getBrokerName());
    sendAllSubscriptions(message.getBrokerName());
}

void Subscriber::handleNewNodeStatus(const std::string nodeName, bool isAlive) {
    if (isAlive) {
        node.log("New node in network: " + nodeName);
    } else {
        node.log("Removing eventual brokers named " + nodeName);
        brokers.erase(nodeName);
    }
}

void Subscriber::handleBrokerAck(const std::string nodeName,
                                 NetworkLayer::SendError error) const {
    // TODO: add some implementation.
}

template <>
void Subscriber::handleMessage(StopPublish& message) {}

template <>
void Subscriber::handleMessage(BrokerIdentity& message) {
    handleNewBroker(message);
}

template <>
void Subscriber::handleMessage(AddRemoveSubscriptionMessage& message) {}

template <>
void Subscriber::handleMessage(PublisherIdentityMessage& message) {}

template <>
void Subscriber::handleMessage(StartPublish& message) {}

template <>
void Subscriber::handleMessage(SubscriptionData& message) {
    auto it = subscriptions.find(message.getSubscription());
    if (it != subscriptions.end()) {
        boost::asio::post(node.getIOService(), std::bind(it->second, message));
    }
}
}  // namespace LogicalLayer
