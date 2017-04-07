#include "subscriber.hpp"
#include "allMessages.hpp"
#include "../../nodeLib/nodeLib/dataMessage.hpp"
#include "../../nodeLib/nodeLib/node.hpp"

#include <boost/variant/apply_visitor.hpp>
#include <boost/serialization/variant.hpp>
#include <sstream>
#include <iostream>

namespace LogicalLayer
{
    Subscriber::Subscriber(NetworkLayer::Node& _node)
    :
        node(_node)
    {
        node.AcceptMessages(
            std::bind(
                &Subscriber::HandleIncomingMessage,
                this,
                std::placeholders::_1));
    }

    Subscriber::~Subscriber()
    {
    }

    void Subscriber::AddSubscription(SubscriptionT& subscription)
    {
        subscriptions.insert(subscription);
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
        boost::archive::binary_iarchive iarchive(ss);

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
        boost::archive::binary_oarchive oarchive(ss);
        oarchive << messageV;

        auto callback = std::bind(
            &Subscriber::HandleBrokerAck,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        node.SndMessage(brokerName, ss.str(), callback);
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
        for(auto& subscription : subscriptions)
        {
            SendSubscription(subscription, brokerName, SubscriptionMessage::eAdd);
        }
    }

    void Subscriber::HandleNewBroker(BrokerIdentity& message)
    {
        brokers.push_back(message.Name());
        SendAllSubscriptions(message.Name());
    }

    void Subscriber::HandleBrokerAck(
        const std::string nodeName, 
        NetworkLayer::SendError error) const
    {
        // TODO: add some implementation.
    }

    template <>
    void Subscriber::HandleMessage(LogMessage& message)
    {
        std::cout << node.Name() << " " << message.Log() << "\n";
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
        std::cout 
            << node.Name()
            << " handle message from Publisher: "
            << message.PublisherName()
            << "----->"
            << message.Data()
            << "\n";
    }
}
