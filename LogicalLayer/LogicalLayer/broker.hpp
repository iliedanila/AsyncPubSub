#ifndef _LOGICAL_LAYER_BROKER_HPP_
#define _LOGICAL_LAYER_BROKER_HPP_
#include "../../nodeLib/nodeLib/sendError.hpp"
#include "messageVisitor.hpp"
#include <map>
#include "subscription.hpp"
#include "publisherIdentity.hpp"

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
        void DefaultSendIdentityCallback(
            std::string nodeName,
            NetworkLayer::SendError error) const;

        void HandleNewNode(std::string nodeName, bool isAlive) const;

        template<typename MessageT>
        void HandleMessage(MessageT& message);

        NetworkLayer::Node& node;
        std::map<std::string, std::vector<SubscriptionT>> activeSubscriptions;
        std::map<std::string, PublisherIdentityT> activePublishers;
    };
}

#endif
