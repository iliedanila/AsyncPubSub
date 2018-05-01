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
        node.getIOService().post(
            [this]
            {
                node.acceptMessages(
                        std::bind(
                                &Broker::handleIncomingMessage,
                                this,
                                std::placeholders::_1
                        )
                );
            }
        );

        node.getIOService().post(
            [this] 
            {
                broadcastIdentity();
            }
        );

        node.getIOService().post(
            [this]
            {
                node.notifyNewNodeStatus(
                        std::bind(
                                &Broker::handleNewNodeStatus,
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

    void Broker::handleIncomingMessage(NetworkLayer::DataMessage &message)
    {
        std::stringstream ss(std::move(message.getBuffer()));
        boost::archive::text_iarchive iarchive(ss);

        LogicalLayer::MessageVariant messageV;
        iarchive >> messageV;

        boost::apply_visitor(MessageVisitor<Broker>(*this), messageV);
    }

    void Broker::broadcastIdentity() const
    {
        for(auto nodeName : node.getAccessibleNodes())
        {
            sendIdentity(nodeName);
        }
    }

    void Broker::sendIdentity(std::string nodeName) const
    {
        LogicalLayer::MessageVariant messageV(BrokerIdentity(node.getName()));
        std::stringstream ss;
        boost::archive::text_oarchive oarchive(ss);
        oarchive << messageV;
        auto messageContent = ss.str();

        auto callback = std::bind(
                &Broker::defaultCallback,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        node.getIOService().post(
            [this, nodeName, callback, messageContent]
            {
                node.sndMessage(nodeName, messageContent, callback);
            }
        );
    }

    void Broker::defaultCallback(
            std::string nodeName,
            NetworkLayer::SendError error) const
    {
        // TODO: add some implementation.
        // maybe log...
    }

    void Broker::handleNewNodeStatus(const std::string nodeName, bool isAlive)
    {
        if(isAlive)
        {
            node.log("New node in network: " + nodeName);
            sendIdentity(nodeName);
        }
        else
        {
            node.log("Removing node " + nodeName + " from publishers and subscribers.");
            activeSubscribers.erase(nodeName);
            activePublishers.erase(nodeName);
        }
    }

    std::vector<std::string> Broker::getPublishersForSubscription(const SubscriptionT &subscription)
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

    std::map<std::string, SubscriptionT> Broker::getSubscribersForPublisher(
            const PublisherIdentityT &publisherIdentity)
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

    void Broker::sendStartPublish(
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
                &Broker::defaultCallback,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        );

        node.getIOService().post(
            [this, publisher, messageContent, callback]
            {
                node.sndMessage(publisher, messageContent, callback);
            }
        );
    }

    template <>
    void Broker::handleMessage(BrokerIdentity& message)
    {
        node.log("Received BrokerIdentity: " + message.getBrokerName());
    }

    template <>
    void Broker::handleMessage(SubscriptionMessage& message)
    {
        node.log("Received subscription from " + message.getSubscriberName());

        if(message.getAction() == SubscriptionMessage::eAdd)
        {
            activeSubscribers[message.getSubscriberName()].insert(message.getSubscription());
            auto publishers = getPublishersForSubscription(message.getSubscription());
            for(auto publisher : publishers)
            {
                sendStartPublish(publisher, message.getSubscriberName(), message.getSubscription());
            }
        }
        else
        {
            auto iterator = activeSubscribers.find(message.getSubscriberName());
            if(iterator != activeSubscribers.end())
            {
                iterator->second.erase(message.getSubscription());
            }

            // TODO: send stop publish.
        }
    }

    template <>
    void Broker::handleMessage(PublisherIdentityMessage& message)
    {
        node.log("Received PublisherIdentity: " + message.getPublisherName());

        activePublishers[message.getPublisherName()] = message.getPublisherIdentity();
        auto subscribers = getSubscribersForPublisher(message.getPublisherIdentity());
        for (auto subscriberSubscriptions : subscribers)
        {
            sendStartPublish(
                    message.getPublisherName(),
                    subscriberSubscriptions.first,
                    subscriberSubscriptions.second
            );
        }
    }

    template<>
    void Broker::handleMessage(StartPublish& message)
    {}

    template<>
    void Broker::handleMessage(PublisherData& message)
    {}
}
