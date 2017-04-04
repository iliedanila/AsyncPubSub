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
        node(_node),
        identity(_publisherIdentity)
    {
        node.AcceptMessages(
            std::bind(
                &Publisher::HandleIncomingMessage,
                this,
                std::placeholders::_1));
    }

    Publisher::~Publisher()
    {
    }

    void Publisher::HandleIncomingMessage(NetworkLayer::DataMessage& message)
    {
        std::stringstream ss(std::move(message.Buffer()));
        boost::archive::binary_iarchive iarchive(ss);

        MessageVariant messageV;
        iarchive >> messageV;

        boost::apply_visitor(MessageVisitor<Publisher>(*this), messageV);
    }

    void Publisher::HandleBrokerAck(
        const std::string nodeName, 
        NetworkLayer::SendError error) const
    {
        // TODO: add some implementation.
    }

    template <>
    void Publisher::HandleMessage(LogMessage& message)
    {
    }

    template <>
    void Publisher::HandleMessage(BrokerIdentity& message)
    {
        PublisherIdentityMessage pMessage(node.Name(), identity);
        MessageVariant messageV(pMessage);
        std::stringstream ss;
        boost::archive::binary_oarchive oarchive(ss);
        oarchive << messageV;

        auto callback = std::bind(
            &Publisher::HandleBrokerAck,
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
    }
}
