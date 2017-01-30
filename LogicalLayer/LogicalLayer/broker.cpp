#include <iostream>
#include <boost/serialization/variant.hpp>

#include "broker.hpp"
#include "../../nodeLib/nodeLib/node.hpp"
#include "brokerIdentity.hpp"
#include "messageVisitor.hpp"
#include "allMessages.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

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

	void Broker::HandleIncomingMessage(NetworkLayer::DataMessage message)
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
		BrokerIdentity message(node.Name());
		MessageVariant messageV(message);
		std::stringstream ss;
		boost::archive::binary_oarchive oarchive(ss);
		oarchive << messageV;

		auto callback = std::bind(
			&Broker::DefaultSendIdentityCallback,
			this,
			nodeName,
			std::placeholders::_1);

		node.SndMessage(nodeName, ss.str(), callback);
	}

	void Broker::DefaultSendIdentityCallback(
		std::string nodeName, 
		NetworkLayer::SendError error) const
	{
		if(error == NetworkLayer::eSuccess)
		{
			std::cout	<< node.Name() 
						<< "Identity sent successfully to " 
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
		std::cout << node.Name() << " " << message.NodeName() << "\n";
	}
}
