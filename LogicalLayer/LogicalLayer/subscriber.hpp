#ifndef _LOGICAL_LAYER_SUBSCRIBER_HPP_
#define _LOGICAL_LAYER_SUBSCRIBER_HPP_
#include "messageVisitor.hpp"
#include <vector>
#include "brokerIdentity.hpp"
#include "subscription.hpp"
#include "../../nodeLib/nodeLib/sendError.hpp"
#include <set>

namespace NetworkLayer {
    class DataMessage;
    class Node;
}

namespace LogicalLayer
{
    class Subscriber
    {
    public:
        explicit Subscriber(NetworkLayer::Node& node);
        ~Subscriber();

        void AddSubscription(SubscriptionT& subscription);
        void RemoveSubscription(SubscriptionT& subscription);

    private:
        friend struct MessageVisitor<Subscriber>;

        void HandleIncomingMessage(NetworkLayer::DataMessage& message);

        void SendSubscription(
            const SubscriptionT& subscription, 
            const std::string& brokerName,
            SubscriptionMessage::Action) const;

        void SendNewSubscription(SubscriptionT& subscription);
        void SendRemoveSubscription(SubscriptionT& subscription);
        void SendAllSubscriptions(const std::string& brokerName);
        void HandleNewBroker(BrokerIdentity& message);

        void HandleBrokerAck(
            const std::string nodeName,
            NetworkLayer::SendError error) const;

        template<typename MessageT>
        void HandleMessage(MessageT& message);

        NetworkLayer::Node& node;
        std::set<std::string> brokers;
        std::set<SubscriptionT> subscriptions;
    };
}

#endif
