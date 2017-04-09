#include "publisher.hpp"
#include <functional>
#include "../../nodeLib/nodeLib/node.hpp"
#include "messageVisitor.hpp"
#include "allMessages.hpp"
#include <boost/serialization/variant.hpp>
#include <iostream>

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
        node.AcceptMessages(
            std::bind(
                &Publisher::HandleIncomingMessage,
                this,
                std::placeholders::_1));

        node.NotifyNewNodeStatus(
            std::bind(
                &Publisher::HandleNewNodeStatus,
                this,
                std::placeholders::_1,
                std::placeholders::_2
            )
        );
    }

    Publisher::~Publisher()
    {
        StopPublishing();
    }

    void Publisher::StartPublishing(PublishFunctionT _publishFunction, std::size_t millisecondsRepeat)
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
        std::cout << "Publisher::StopPublishing()\n";
        publishTimer.cancel();
        subscribers.clear();
    }

    void Publisher::HandleIncomingMessage(NetworkLayer::DataMessage& message)
    {
        std::stringstream ss(std::move(message.Buffer()));
        boost::archive::binary_iarchive iarchive(ss);

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
            std::cout << "Timer error: " << error.message() << "\n";
        }

        auto message = publishFunction();
        MessageVariant messageV(message);
        std::stringstream ss;
        boost::archive::binary_oarchive oarchive(ss);
        oarchive << messageV;

        auto callback = std::bind(
            &Publisher::DefaultHandleAck,
            this,
            std::placeholders::_1,
            std::placeholders::_2);
        for(auto subscriber : subscribers)
        {
            node.SndMessage(subscriber, ss.str(), callback);
        }

        publishTimer.expires_from_now(boost::posix_time::milliseconds(millisecondsRepeatPublish));
        publishTimer.async_wait(std::bind(&Publisher::OnTimerExpired, this, std::placeholders::_1));
    }

    template <>
    void Publisher::HandleMessage(LogMessage& message)
    {}

    template <>
    void Publisher::HandleMessage(BrokerIdentity& message)
    {
        PublisherIdentityMessage pMessage(node.Name(), identity);
        MessageVariant messageV(pMessage);
        std::stringstream ss;
        boost::archive::binary_oarchive oarchive(ss);
        oarchive << messageV;

        auto callback = std::bind(
            &Publisher::DefaultHandleAck,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        node.SndMessage(message.Name(), ss.str(), callback);
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
        std::cout << "Publisher: Start sending data to " << message.Subscriber() << "\n";
        subscribers.insert(message.Subscriber());
    }

    template <>
    void Publisher::HandleMessage(PublisherData& message)
    {}
}
