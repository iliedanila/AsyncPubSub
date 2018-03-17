#include "publisher.hpp"
#include <functional>
#include "node.hpp"
#include "messageVisitor.hpp"
#include "../inc/allMessages.hpp"
#include <boost/serialization/variant.hpp>
#include <iostream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace LogicalLayer
{
    Publisher::Publisher(
        NetworkLayer::Node& _node,
        PublisherIdentityT& _publisherIdentity)
    :
        identity(_publisherIdentity),
        node(_node),
        publishTimer(node.IOService())
    {
        node.IOService().post(
            [this]
            {
                node.AcceptMessages(
                    std::bind(
                        &Publisher::HandleIncomingMessage,
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
                        &Publisher::HandleNewNodeStatus,
                        this,
                        std::placeholders::_1,
                        std::placeholders::_2
                    )
                );
            }
        );
    }

    Publisher::~Publisher()
    {
        StopPublishing();
    }

    void Publisher::StartPublishing(PublishFunctionT _publishFunction, uint32_t millisecondsRepeat)
    {
        publishFunction = _publishFunction;
        millisecondsRepeatPublish = millisecondsRepeat;
        publishTimer.cancel();
        publishTimer.expires_from_now(boost::posix_time::milliseconds(millisecondsRepeatPublish));
        publishTimer.async_wait(
            std::bind(&Publisher::OnTimerExpired, this, std::placeholders::_1)
        );
    }

    void Publisher::StopPublishing()
    {
        node.Log("Stop publishing.");
        publishTimer.cancel();
        //subscribers.clear();
        //TODO:
    }
    
    const std::string& Publisher::Name() const
    {
        return node.Name();
    }

    void Publisher::HandleIncomingMessage(NetworkLayer::DataMessage& message)
    {
        std::stringstream ss(std::move(message.Buffer()));
        boost::archive::text_iarchive iarchive(ss);

        MessageVariant messageV;
        iarchive >> messageV;

        boost::apply_visitor(MessageVisitor<Publisher>(*this), messageV);
    }

    void Publisher::HandleNewNodeStatus(std::string nodeName, bool isAlive)
    {
        if(!isAlive)
        {
            subscribers.erase(nodeName);
        }
    }

    void Publisher::DefaultHandleAck(
        const std::string nodeName, 
        NetworkLayer::SendError error) const
    {
        // TODO: add some implementation.
    }

    void Publisher::OnTimerExpired(boost::system::error_code error)
    {
        if (!publishFunction)
            return;

        if(error)
        {
            return;
        }

        auto message = publishFunction();

        auto callback = std::bind(
            &Publisher::DefaultHandleAck,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        for(auto subscriberNameSubscription : subscribers)
        {
            node.IOService().post(
                [this, subscriberNameSubscription, message, callback]
                {
                    PublisherData localMessage(message.PublisherName(), message.Data());
                    SubscriptionT subscription = subscriberNameSubscription.second;
                    localMessage.AddSubscription(subscription);
                    MessageVariant messageV(localMessage);
                    std::stringstream ss;
                    boost::archive::text_oarchive oarchive(ss);
                    oarchive << messageV;
                    auto messageContent = ss.str();

                    node.SndMessage(subscriberNameSubscription.first, messageContent, callback);
                }
            );
        }

        publishTimer.expires_from_now(boost::posix_time::milliseconds(millisecondsRepeatPublish));
        publishTimer.async_wait(std::bind(&Publisher::OnTimerExpired, this, std::placeholders::_1));
    }

    template <>
    void Publisher::HandleMessage(BrokerIdentity& message)
    {
        PublisherIdentityMessage pMessage(node.Name(), identity);
        MessageVariant messageV(pMessage);
        std::stringstream ss;
        boost::archive::text_oarchive oarchive(ss);
        oarchive << messageV;
        auto messageContent = ss.str();

        auto callback = std::bind(
            &Publisher::DefaultHandleAck,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        node.IOService().post(
            [this, message, messageContent, callback]
            {
                node.SndMessage(message.BrokerName(), messageContent, callback);
            }
        );
    }

    template <>
    void Publisher::HandleMessage(SubscriptionMessage& message)
    {}

    template <>
    void Publisher::HandleMessage(PublisherIdentityMessage& message)
    {}

    template <>
    void Publisher::HandleMessage(StartPublish& message)
    {
        node.Log("Start sending data to " + message.SubscriberName());
        subscribers.insert(std::make_pair(message.SubscriberName(), message.Subscription()));
    }

    template <>
    void Publisher::HandleMessage(PublisherData& message)
    {}
}
