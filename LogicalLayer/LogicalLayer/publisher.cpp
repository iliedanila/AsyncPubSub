#include "publisher.hpp"
#include <functional>
#include "../../nodeLib/nodeLib/node.hpp"
#include "messageVisitor.hpp"
#include "allMessages.hpp"
#include <boost/serialization/variant.hpp>
#include <iostream>

namespace LogicalLayer
{
	Publisher::Publisher(NetworkLayer::Node& _node)
	:
		node(_node)
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
		std::cout << "Publisher::HandleBrokerAck in node "
			<< node.Name()
			<< "\n";
	}

	template <>
	void Publisher::HandleMessage(LogMessage& message)
	{
	}

	template <>
	void Publisher::HandleMessage(BrokerIdentity& message)
	{
		PublisherIdentity pMessage(node.Name(), identity);
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
	void Publisher::HandleMessage(Subscription& message)
	{}

	template <>
	void Publisher::HandleMessage(PublisherIdentity& message)
	{}
}
