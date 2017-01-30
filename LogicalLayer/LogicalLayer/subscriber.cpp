#include "subscriber.hpp"
#include "subscription.hpp"
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
		subscriptions.push_back(subscription);
		SendNewSubscription(subscription);
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
		SubscriptionT& _subscription, 
		const std::string& brokerName) const
	{
		Subscription subscription(_subscription);
		MessageVariant messageV(subscription);
		std::stringstream ss;
		boost::archive::binary_oarchive oarchive(ss);
		oarchive << messageV;

		auto callback = std::bind(
			&Subscriber::HandleBrokerSubscriptionAck,
			this,
			std::placeholders::_1,
			std::placeholders::_2);

		node.SndMessage(brokerName, ss.str(), callback);
	}

	void Subscriber::SendNewSubscription(SubscriptionT& subscription)
	{
		for(auto& brokerName : brokers)
		{
			SendSubscription(subscription, brokerName);
		}
	}

	void Subscriber::SendAllSubscriptions(const std::string& brokerName)
	{
		for(auto& subscription : subscriptions)
		{
			SendSubscription(subscription, brokerName);
		}
	}

	void Subscriber::HandleNewBroker(BrokerIdentity& message)
	{
		brokers.push_back(message.NodeName());
		SendAllSubscriptions(message.NodeName());
	}

	void Subscriber::HandleBrokerSubscriptionAck(
		const std::string nodeName, 
		NetworkLayer::SendError error) const
	{
		std::cout << "Subscriber::HandleBrokerSubscriptionAck in node "
			<< node.Name()
			<< "\n";
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
	void Subscriber::HandleMessage(Subscription& message)
	{
	}
}
