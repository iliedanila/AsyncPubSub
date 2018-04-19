#include <iostream>
#include <boost/serialization/variant.hpp>

#include "broker.hpp"
#include "node.hpp"
#include "brokerIdentity.hpp"
#include "messageVisitor.hpp"
#include "../inc/allMessages.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

//#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/text_oarchive.hpp>

namespace LogicalLayer
{
    Broker::Broker(NetworkLayer::Node& _node)
    :
        node(_node)
    {
        node.IOService().post(
            [this]
            {
                node.AcceptMessages(
                    std::bind(
                        &Broker::HandleIncomingMessage,
                        this,
                        std::placeholders::_1
                    )
                );
            }
        );

        node.IOService().post(
            [this] 
            {
                BroadcastIdentity();
            }
        );

        node.IOService().post(
            [this]
            {
                node.NotifyNewNodeStatus(
                    std::bind(
                        &Broker::HandleNewNodeStatus,
                        this,
                        std::placeholders::_1,
                        std::placeholders::_2
                    )
                );
            }
        );

    }

    Broker::~Broker()
    {
    }

    void Broker::HandleIncomingMessage(NetworkLayer::DataMessage& message)
    {
        std::stringstream ss(std::move(message.Buffer()));
        boost::archive::text_iarchive iarchive(ss);

        LogicalLayer::MessageVariant messageV;
        iarchive >> messageV;

        boost::apply_visitor(MessageVisitor<Broker>(*this), messageV);
    }

    void Broker::BroadcastIdentity() const
    {
        for(auto nodeName : node.GetAccessibleNodes())
        {
            SendIdentity(nodeName);
        }
    }

    void Broker::SendIdentity(std::string nodeName) const
    {
        LogicalLayer::MessageVariant messageV(BrokerIdentity(node.Name()));
        std::stringstream ss;
        boost::archive::text_oarchive oarchive(ss);
        oarchive << messageV;
        auto messageContent = ss.str();

        auto callback = std::bind(
            &Broker::DefaultCallback,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        node.IOService().post(
            [this, nodeName, callback, messageContent]
            {
                node.SndMessage(nodeName, messageContent, callback);
            }
        );
    }

    void Broker::DefaultCallback(
        std::string nodeName, 
        NetworkLayer::SendError error) const
    {
        // TODO: add some implementation.
        // maybe log...
    }

    void Broker::HandleNewNodeStatus(const std::string nodeName, bool isAlive)
    {
        if(isAlive)
        {
            node.Log("New node in network: " + nodeName);
            SendIdentity(nodeName);
        }
        else
        {
            node.Log("Removing node " + nodeName + " from publishers and subscribers.");
            activeSubscribers.erase(nodeName);
            activePublishers.erase(nodeName);
        }
    }

    std::vector<std::string> Broker::GetPublishersForSubscription(const SubscriptionT& subscription)
    {
        std::vector<std::string> publisherList;
        for(auto& publisher : activePublishers)
        {
            if(std::includes(
                publisher.second.begin(), 
                publisher.second.end(),
                subscription.begin(),
                subscription.end()))
            {
                publisherList.push_back(publisher.first);
            }
        }

        return publisherList;
    }

    std::map<std::string, SubscriptionT> Broker::GetSubscribersForPublisher(
        const PublisherIdentityT& publisherIdentity)
    {
        std::map<std::string, SubscriptionT> subscriberList;
        for(auto& subscriber : activeSubscribers)
        {
            for(auto& subscription : subscriber.second)
            {
                if(std::includes(
                    publisherIdentity.begin(),
                    publisherIdentity.end(),
                    subscription.begin(),
                    subscription.end()))
                {
                    subscriberList.insert(std::make_pair(subscriber.first, subscription));
                }
            }
        }

        return subscriberList;
    }

    void Broker::SendStartPublish(
        std::string publisher,
        std::string subscriberName,
        SubscriptionT subscription)
    {
        StartPublish startPublishMessage(subscriberName, subscription);
        LogicalLayer::MessageVariant messageV(startPublishMessage);
        std::stringstream ss;
        boost::archive::text_oarchive oarchive(ss);
        oarchive << messageV;
        auto messageContent = ss.str();

        auto callback = std::bind(
            &Broker::DefaultCallback,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        );

        node.IOService().post(
            [this, publisher, messageContent, callback]
            {
                node.SndMessage(publisher, messageContent, callback);
            }
        );
    }

    template <>
    void Broker::HandleMessage(BrokerIdentity& message)
    {
        node.Log("Received BrokerIdentity: " + message.BrokerName());
    }

    template <>
    void Broker::HandleMessage(SubscriptionMessage& message)
    {
        node.Log("Received subscription from " + message.SubscriberName());

        if(message.GetAction() == SubscriptionMessage::eAdd)
        {
            activeSubscribers[message.SubscriberName()].insert(message.Subscription());
            auto publishers = GetPublishersForSubscription(message.Subscription());
            for(auto publisher : publishers)
            {
                SendStartPublish(publisher, message.SubscriberName(), message.Subscription());
            }
        }
        else
        {
            auto iterator = activeSubscribers.find(message.SubscriberName());
            if(iterator != activeSubscribers.end())
            {
                iterator->second.erase(message.Subscription());
            }

            // TODO: send stop publish.
        }
    }

    template <>
    void Broker::HandleMessage(PublisherIdentityMessage& message)
    {
        node.Log("Received PublisherIdentity: " + message.Publisher());

        activePublishers[message.Publisher()] = message.GetPublisherIdentity();
        auto subscribers = GetSubscribersForPublisher(message.GetPublisherIdentity());
        for (auto subscriberSubscriptions : subscribers)
        {
            SendStartPublish(
                message.Publisher(),
                subscriberSubscriptions.first,
                subscriberSubscriptions.second
            );
        }
    }

    template<>
    void Broker::HandleMessage(StartPublish& message)
    {}

    template<>
    void Broker::HandleMessage(PublisherData& message)
    {}
}