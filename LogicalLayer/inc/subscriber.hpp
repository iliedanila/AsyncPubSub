#ifndef _LOGICAL_LAYER_SUBSCRIBER_HPP_
#define _LOGICAL_LAYER_SUBSCRIBER_HPP_

#include <set>
#include <functional>
#include <map>

#include "addRemoveSubscription.hpp"
#include "messageVisitor.hpp"
#include "sendError.hpp"
#include "common.hpp"

namespace NetworkLayer {
    class DataMessage;
    class Node;
}

namespace LogicalLayer
{

class SubscriptionData;
class BrokerIdentity;

class Subscriber
{
public:
    explicit Subscriber(NetworkLayer::Node& node);
    ~Subscriber();

    void addSubscription(SubscriptionT subscription, SubscriptionDataHandlerT handler);
    void removeSubscription(SubscriptionT &subscription);

private:
    friend struct MessageVisitor<Subscriber>;

    void handleIncomingMessage(NetworkLayer::DataMessage &message);

    void sendSubscription(
            const SubscriptionT &subscription,
            const std::string &brokerName,
            AddRemoveSubscriptionMessage::Action) const;

    void sendNewSubscription(SubscriptionT &subscription);
    void sendRemoveSubscription(SubscriptionT &subscription);
    void sendAllSubscriptions(const std::string &brokerName);
    void handleNewBroker(BrokerIdentity& message);

    void handleNewNodeStatus(const std::string nodeName, bool isAlive);

    void handleBrokerAck(
            const std::string nodeName,
            NetworkLayer::SendError error) const;

    template<typename MessageT>
    void handleMessage(MessageT& message);

    NetworkLayer::Node& node;
    std::set<std::string> brokers;
    std::map<SubscriptionT, SubscriptionDataHandlerT> subscriptions;
};

}

#endif
