#ifndef _LOGICAL_LAYER_BROKER_HPP_
#define _LOGICAL_LAYER_BROKER_HPP_
#include "../../MeshNetwork/src/sendError.hpp"
#include "messageVisitor.hpp"
#include <map>
#include "subscription.hpp"
#include "publisherIdentity.hpp"
#include <set>

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

        void HandleIncomingMessage(NetworkLayer::DataMessage& message);
        void BroadcastIdentity() const;
        void SendIdentity(std::string nodeName) const;
        void DefaultCallback(
            std::string nodeName,
            NetworkLayer::SendError error) const;

        void HandleNewNodeStatus(const std::string nodeName, bool isAlive);

        template<typename MessageT>
        void HandleMessage(MessageT& message);

        std::vector<std::string> GetPublishersForSubscription(const SubscriptionT& subscription);
        std::map<std::string, SubscriptionT> GetSubscribersForPublisher(
            const PublisherIdentityT& publisherIdentity);
        void SendStartPublish(std::string publisher, std::string subscriber, SubscriptionT subscription);

        NetworkLayer::Node& node;
        std::map<std::string, std::set<SubscriptionT>> activeSubscribers;
        std::map<std::string, PublisherIdentityT> activePublishers;
    };
}

#endif
