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
        publishTimer(node.getIOService())
    {
        node.getIOService().post(
            [this]
            {
                node.acceptMessages(
                        std::bind(
                                &Publisher::handleIncomingMessage,
                                this,
                                std::placeholders::_1
                        )
                );
            }
        );

        node.getIOService().post(
            [this]
            {
                node.notifyNewNodeStatus(
                        std::bind(
                                &Publisher::handleNewNodeStatus,
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
        stopPublishing();
    }

    void Publisher::startPublishing(PublishFunctionT _publishFunction, uint32_t millisecondsRepeat)
    {
        publishFunction = _publishFunction;
        millisecondsRepeatPublish = millisecondsRepeat;
        publishTimer.cancel();
        publishTimer.expires_from_now(boost::posix_time::milliseconds(millisecondsRepeatPublish));
        publishTimer.async_wait(
            std::bind(&Publisher::onTimerExpired, this, std::placeholders::_1)
        );
    }

    void Publisher::stopPublishing()
    {
        node.log("Stop publishing.");
        publishTimer.cancel();
        //subscribers.clear();
        //TODO:
    }
    
    const std::string& Publisher::getName() const
    {
        return node.getName();
    }

    void Publisher::handleIncomingMessage(NetworkLayer::DataMessage &message)
    {
        std::stringstream ss(std::move(message.getBuffer()));
        boost::archive::text_iarchive iarchive(ss);

        MessageVariant messageV;
        iarchive >> messageV;

        boost::apply_visitor(MessageVisitor<Publisher>(*this), messageV);
    }

    void Publisher::handleNewNodeStatus(std::string nodeName, bool isAlive)
    {
        if(!isAlive)
        {
            subscribers.erase(nodeName);
        }
    }

    void Publisher::defaultHandleAck(
            const std::string nodeName,
            NetworkLayer::SendError error) const
    {
        // TODO: add some implementation.
    }

    void Publisher::onTimerExpired(boost::system::error_code error)
    {
        if (!publishFunction)
            return;

        if(error)
        {
            return;
        }

        auto message = publishFunction();

        auto callback = std::bind(
                &Publisher::defaultHandleAck,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        for(auto subscriberNameSubscription : subscribers)
        {
            node.getIOService().post(
                [this, subscriberNameSubscription, message, callback]
                {
                    PublisherData localMessage(message.getPublisherName(), message.getData());
                    SubscriptionT subscription = subscriberNameSubscription.second;
                    localMessage.addSubscription(subscription);
                    MessageVariant messageV(localMessage);
                    std::stringstream ss;
                    boost::archive::text_oarchive oarchive(ss);
                    oarchive << messageV;
                    auto messageContent = ss.str();

                    node.sndMessage(subscriberNameSubscription.first, messageContent, callback);
                }
            );
        }

        publishTimer.expires_from_now(boost::posix_time::milliseconds(millisecondsRepeatPublish));
        publishTimer.async_wait(std::bind(&Publisher::onTimerExpired, this, std::placeholders::_1));
    }

    template <>
    void Publisher::handleMessage(BrokerIdentity& message)
    {
        PublisherIdentityMessage pMessage(node.getName(), identity);
        MessageVariant messageV(pMessage);
        std::stringstream ss;
        boost::archive::text_oarchive oarchive(ss);
        oarchive << messageV;
        auto messageContent = ss.str();

        auto callback = std::bind(
                &Publisher::defaultHandleAck,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        node.getIOService().post(
            [this, message, messageContent, callback]
            {
                node.sndMessage(message.getBrokerName(), messageContent, callback);
            }
        );
    }

    template <>
    void Publisher::handleMessage(SubscriptionMessage& message)
    {}

    template <>
    void Publisher::handleMessage(PublisherIdentityMessage& message)
    {}

    template <>
    void Publisher::handleMessage(StartPublish& message)
    {
        node.log("Start sending data to " + message.getSubscriberName());
        subscribers.insert(std::make_pair(message.getSubscriberName(), message.getSubscription()));
    }

    template <>
    void Publisher::handleMessage(PublisherData& message)
    {}
}
