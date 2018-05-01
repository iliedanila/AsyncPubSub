#ifndef _LOGICAL_LAYER_SUBSCRIBER_HPP_
#define _LOGICAL_LAYER_SUBSCRIBER_HPP_

#include <set>
#include <functional>
#include <map>

#include "messageVisitor.hpp"
#include "brokerIdentity.hpp"
#include "subscription.hpp"
#include "sendError.hpp"



namespace NetworkLayer {
    class DataMessage;
    class Node;
}

namespace LogicalLayer
{
    class PublisherData;

    class Subscriber
    {
    public:
        typedef std::function<void(PublisherData& publisherData)> PublisherDataHandlerT;

        explicit Subscriber(NetworkLayer::Node& node);
        ~Subscriber();

        void addSubscription(SubscriptionT &subscription, PublisherDataHandlerT handler);
        void removeSubscription(SubscriptionT &subscription);

    private:
        friend struct MessageVisitor<Subscriber>;

        void handleIncomingMessage(NetworkLayer::DataMessage &message);

        void sendSubscription(
                const SubscriptionT &subscription,
                const std::string &brokerName,
                SubscriptionMessage::Action) const;

        void sendNewSubscription(SubscriptionT &subscription);
        void sendRemoveSubscription(SubscriptionT &subscription);
        void sendAllSubscriptions(const std::string &brokerName);
        void handleNewBroker(BrokerIdentity &message);

        void handleNewNodeStatus(const std::string nodeName, bool isAlive);

        void handleBrokerAck(
                const std::string nodeName,
                NetworkLayer::SendError error) const;

        template<typename MessageT>
        void handleMessage(MessageT& message);

        NetworkLayer::Node& node;
        std::set<std::string> brokers;
        std::map<SubscriptionT, PublisherDataHandlerT> subscriptions;
    };
}

#endif
