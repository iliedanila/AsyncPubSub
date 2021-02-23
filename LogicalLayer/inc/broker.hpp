#ifndef _LOGICAL_LAYER_BROKER_HPP_
#define _LOGICAL_LAYER_BROKER_HPP_

#include "sendError.hpp"
#include "messageVisitor.hpp"
#include "common.hpp"

#include <map>
#include <set>
#include <vector>

namespace NetworkLayer {
    class DataMessage;
    class Node;
}

namespace LogicalLayer
{

class Broker
{
public:
    explicit Broker(NetworkLayer::Node& node);
    ~Broker();

private:
    friend struct MessageVisitor<Broker>;

    void handleIncomingMessage(NetworkLayer::DataMessage &message);
    void broadcastIdentity() const;
    void sendIdentity(std::string nodeName) const;
    void defaultCallback(
            std::string nodeName,
            NetworkLayer::SendError error) const;

    void handleNewNodeStatus(const std::string nodeName, bool isAlive);

    template<typename MessageT>
    void handleMessage(MessageT& message);

    std::vector<std::string> getPublishersForSubscription(const SubscriptionT &subscription);
    std::map<std::string, SubscriptionT> getSubscribersForPublisher(
            const PublisherIdentityT &publisherIdentity);
    void sendStartPublish(std::string publisher, std::string subscriber, SubscriptionT subscription);
    void sendStopPublish(std::string publisher, std::string subscriber, SubscriptionT subscription);

    NetworkLayer::Node& node;
    std::map<std::string, std::set<SubscriptionT>> activeSubscribers;
    std::map<std::string, PublisherIdentityT> activePublishers;
};

}

#endif
