#include <iostream>
#include <boost/serialization/variant.hpp>

#include "broker.hpp"
#include "../../nodeLib/nodeLib/node.hpp"
#include "brokerIdentity.hpp"
#include "messageVisitor.hpp"
#include "allMessages.hpp"

//#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/text_oarchive.hpp>

namespace LogicalLayer
{
    Broker::Broker(NetworkLayer::Node& _node)
    :
        node(_node)
    {
        node.AcceptMessages(
            std::bind(
                &Broker::HandleIncomingMessage,
                this,
                std::placeholders::_1));

        node.IOService().post([this]{
            BroadcastIdentity();
        });
    }

    Broker::~Broker()
    {
    }

    void Broker::HandleIncomingMessage(NetworkLayer::DataMessage& message)
    {
        std::stringstream ss(std::move(message.Buffer()));
        boost::archive::binary_iarchive iarchive(ss);

        MessageVariant messageV;
        iarchive >> messageV;

        boost::apply_visitor(MessageVisitor<Broker>(*this), messageV);
    }

    void Broker::BroadcastIdentity() const
    {
        for(auto nodeName : node.GetAccessibleNodes())
        {
            SendIdentity(nodeName);
        }

        node.NotifyNewNodeStatus(std::bind(
            &Broker::HandleNewNode,
            this,
            std::placeholders::_1,
            std::placeholders::_2));
    }

    void Broker::SendIdentity(std::string nodeName) const
    {
        MessageVariant messageV(BrokerIdentity(node.Name()));
        std::stringstream ss;
        boost::archive::binary_oarchive oarchive(ss);
        oarchive << messageV;

        auto callback = std::bind(
            &Broker::DefaultSendIdentityCallback,
            this,
            std::placeholders::_1,
            std::placeholders::_2);

        node.SndMessage(nodeName, ss.str(), callback);
    }

    void Broker::DefaultSendIdentityCallback(
        std::string nodeName, 
        NetworkLayer::SendError error) const
    {
        if(error == NetworkLayer::eSuccess)
        {
            std::cout	<< node.Name() 
                        << " Identity sent successfully to " 
                        << nodeName 
                        << "\n";
        }
    }

    void Broker::HandleNewNode(std::string nodeName, bool isAlive) const
    {
        if(isAlive)
        {
            SendIdentity(nodeName);
        }
    }

    template <>
    void Broker::HandleMessage(LogMessage& message)
    {
        std::cout << node.Name() << " " << message.Log() << "\n";
    }

    template <>
    void Broker::HandleMessage(BrokerIdentity& message)
    {
        std::cout << node.Name() << " " << message.Name() << "\n";
    }

    template <>
    void Broker::HandleMessage(SubscriptionMessage& message)
    {
        std::cout	<< "Broker::HandleMessage - Subscription from " 
                    << message.Subscriber()
                    << "\n";
        //TODO:
        if(message.GetAction() == SubscriptionMessage::eAdd)
        {
            activeSubscriptions[message.Subscriber()].push_back(message.GetSubscription());
            std::cout << "Added subscription from " << message.Subscriber() << "\n";
            for(auto attribPair : message.GetSubscription())
            {
                std::cout << attribPair.first << " " << attribPair.second << "\n";
            }
            std::cout << "\n";
        }
        else
        {
            
        }
    }

    template <>
    void Broker::HandleMessage(PublisherIdentityMessage& message)
    {
        std::cout   << "Broker::HandleMessage - PublisherIdentity: " 
                    << message.Publisher() 
                    << "\n";
        activePublishers[message.Publisher()] = message.GetPublisherIdentity();
        std::cout << "Added publisher: " << message.Publisher() << "\n";
        for(auto attribPair : message.GetPublisherIdentity())
        {
            std::cout << attribPair.first << " " << attribPair.second << "\n";
        }
        std::cout << "\n";
    }
}
