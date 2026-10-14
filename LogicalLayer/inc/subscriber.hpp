#ifndef _LOGICAL_LAYER_SUBSCRIBER_HPP_
#define _LOGICAL_LAYER_SUBSCRIBER_HPP_

#include <functional>
#include <map>
#include <set>

#include "addRemoveSubscription.hpp"
#include "common.hpp"
#include "sendError.hpp"

namespace NetworkLayer {
class DataMessage;
class Node;
}  // namespace NetworkLayer

namespace LogicalLayer {

class BrokerIdentity;
class PublisherIdentityMessage;
class StartPublish;
class StopPublish;
class SubscriptionData;

class Subscriber {
   public:
    explicit Subscriber(NetworkLayer::Node& node);
    ~Subscriber();

    void addSubscription(SubscriptionT subscription,
                         SubscriptionDataHandlerT handler);
    void removeSubscription(SubscriptionT& subscription);

   private:
    void handleIncomingMessage(NetworkLayer::DataMessage& message);

    void sendSubscription(const SubscriptionT& subscription,
                          const std::string& brokerName,
                          AddRemoveSubscriptionMessage::Action) const;

    void sendNewSubscription(SubscriptionT& subscription);
    void sendRemoveSubscription(SubscriptionT& subscription);
    void sendAllSubscriptions(const std::string& brokerName);
    void handleNewBroker(BrokerIdentity& message);

    void handleNewNodeStatus(const std::string nodeName, bool isAlive);

    void handleBrokerAck(const std::string nodeName,
                         NetworkLayer::SendError error) const;

    // One handler per MessageVariant alternative. handleIncomingMessage
    // dispatches with std::visit, so a missing one is a compile error.
    void handleMessage(BrokerIdentity& message);
    void handleMessage(AddRemoveSubscriptionMessage& message);
    void handleMessage(PublisherIdentityMessage& message);
    void handleMessage(StartPublish& message);
    void handleMessage(StopPublish& message);
    void handleMessage(SubscriptionData& message);

    NetworkLayer::Node& node;
    std::set<std::string> brokers;
    std::map<SubscriptionT, SubscriptionDataHandlerT> subscriptions;
};

}  // namespace LogicalLayer

#endif
