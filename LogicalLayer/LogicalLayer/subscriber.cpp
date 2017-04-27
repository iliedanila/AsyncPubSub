#include "subscriber.hpp"
#include "allMessages.hpp"
#include "../../nodeLib/nodeLib/dataMessage.hpp"
#include "../../nodeLib/nodeLib/node.hpp"

#include <boost/variant/apply_visitor.hpp>
#include <boost/serialization/variant.hpp>
#include <sstream>
#include <iostream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace LogicalLayer
{
    Subscriber::Subscriber(NetworkLayer::Node& _node)
    :
        node(_node)
    {
        node.IOService().post(
            [this]
            {
                node.AcceptMessages(
                    std::bind(
                        &Subscriber::HandleIncomingMessage,
                        this,
                        std::placeholders::_1
                    )
                );
            }
        );

        node.IOService().post(
            [this]
            {
                node.NotifyNewNodeStatus(
                    std::bind(
                        &Subscriber::HandleNewNodeStatus,
                        this,
                        std::placeholders::_1,
                        std::placeholders::_2
                    )
                );
            }
        );

    }

    Subscriber::~Subscriber()
    {
    }

    void Subscriber::AddSubscription(SubscriptionT& subscription, PublisherDataHandlerT handler)
    {
        subscriptions.insert(std::make_pair(subscription, handler));
        SendNewSubscription(subscription);
    }

    void Subscriber::RemoveSubscription(SubscriptionT& subscription)
    {
        subscriptions.erase(subscription);
        SendRemoveSubscription(subscription);
    }

    void Subscriber::HandleIncomingMessage(NetworkLayer::DataMessage& message)
    {
        std::stringstream ss(std::move(message.Buffer()));
        boost::archive::text_iarchive iarchive(ss);

        MessageVariant messageV;
        iarchive >> messageV;

        boost::apply_visitor(MessageVisitor<Subscriber>(*this), messageV);
    }

    void Subscriber::SendSubscription(
        const SubscriptionT& _subscription, 
        const std::string& brokerName,
        SubscriptionMessage::Action action) const
    {
        SubscriptionMessage subscription(node.Name(), _subscription, action);
        MessageVariant messageV(subscription);
        std::stringstream ss;
        boost::archive::text_oarchive oarchive(ss);
        oarchive << messageV;
        auto messageContent = ss.str();

        auto callback = std::bind(
            &Subscriber::HandleBrokerAck,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        node.IOService().post(
            [this, brokerName, messageContent, callback]
            {
                node.SndMessage(brokerName, messageContent, callback);
            }
        );
    }

    void Subscriber::SendNewSubscription(SubscriptionT& subscription)
    {
        for(auto& brokerName : brokers)
        {
            SendSubscription(subscription, brokerName, SubscriptionMessage::eAdd);
        }
    }

    void Subscriber::SendRemoveSubscription(SubscriptionT& subscription)
    {
        for (auto& brokerName : brokers)
        {
            SendSubscription(subscription, brokerName, SubscriptionMessage::eRemove);
        }
    }

    void Subscriber::SendAllSubscriptions(const std::string& brokerName)
    {
        if (subscriptions.empty())
            return;

        for(auto& subscription : subscriptions)
        {
            SendSubscription(subscription.first, brokerName, SubscriptionMessage::eAdd);
        }
    }

    void Subscriber::HandleNewBroker(BrokerIdentity& message)
    {
        node.Log("New broker found: " + message.BrokerName());
        if (brokers.find(message.BrokerName()) != brokers.end())
            return;

        brokers.insert(message.BrokerName());
        SendAllSubscriptions(message.BrokerName());
    }

    void Subscriber::HandleNewNodeStatus(const std::string nodeName, bool isAlive)
    {
        if (isAlive)
        {
            node.Log("New node in network: " + nodeName);
        }
        else
        {
            node.Log("Removing eventual brokers named " + nodeName);
            brokers.erase(nodeName);
        }
    }

    void Subscriber::HandleBrokerAck(
        const std::string nodeName, 
        NetworkLayer::SendError error) const
    {
        // TODO: add some implementation.
    }

    template <>
    void Subscriber::HandleMessage(BrokerIdentity& message)
    {
        HandleNewBroker(message);
    }

    template <>
    void Subscriber::HandleMessage(SubscriptionMessage& message)
    {}

    template <>
    void Subscriber::HandleMessage(PublisherIdentityMessage& message)
    {}

    template <>
    void Subscriber::HandleMessage(StartPublish& message)
    {}

    template<>
    void Subscriber::HandleMessage(PublisherData& message)
    {
        auto it = subscriptions.find(message.Subscription());
        if (it != subscriptions.end())
        {
            it->second(message);
        }
    }
}
