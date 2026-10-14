#ifndef _LOGICAL_LAYER_BROKER_HPP_
#define _LOGICAL_LAYER_BROKER_HPP_

#include "common.hpp"
#include "sendError.hpp"

#include <map>
#include <set>
#include <vector>

namespace NetworkLayer {
class DataMessage;
class Node;
}  // namespace NetworkLayer

namespace LogicalLayer {

class BrokerIdentity;
class AddRemoveSubscriptionMessage;
class PublisherIdentityMessage;
class StartPublish;
class StopPublish;
class SubscriptionData;

class Broker {
   public:
    explicit Broker(NetworkLayer::Node& node);
    ~Broker();

   private:
    void handleIncomingMessage(NetworkLayer::DataMessage& message);
    void broadcastIdentity() const;
    void sendIdentity(std::string nodeName) const;
    void defaultCallback(std::string nodeName,
                         NetworkLayer::SendError error) const;

    void handleNewNodeStatus(const std::string nodeName, bool isAlive);

    // One handler per MessageVariant alternative. handleIncomingMessage
    // dispatches with std::visit, so a missing one is a compile error.
    void handleMessage(BrokerIdentity& message);
    void handleMessage(AddRemoveSubscriptionMessage& message);
    void handleMessage(PublisherIdentityMessage& message);
    void handleMessage(StartPublish& message);
    void handleMessage(StopPublish& message);
    void handleMessage(SubscriptionData& message);

    std::vector<std::string> getPublishersForSubscription(
        const SubscriptionT& subscription);
    std::map<std::string, SubscriptionT> getSubscribersForPublisher(
        const PublisherIdentityT& publisherIdentity);
    void sendStartPublish(std::string publisher, std::string subscriber,
                          SubscriptionT subscription);
    void sendStopPublish(std::string publisher, std::string subscriber,
                         SubscriptionT subscription);
    void stopPublishingTo(const std::string& subscriberName);

    NetworkLayer::Node& node;
    std::map<std::string, std::set<SubscriptionT>> activeSubscribers;
    std::map<std::string, PublisherIdentityT> activePublishers;
};

}  // namespace LogicalLayer

#endif
