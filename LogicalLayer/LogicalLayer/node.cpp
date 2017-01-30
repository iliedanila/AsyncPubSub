#include <boost/serialization/variant.hpp>

#include "node.hpp"
#include "../../nodeLib/nodeLib/node.hpp"
#include <iostream>
#include "messageVisitor.hpp"

namespace LogicalLayer
{

Node::Node(NetworkLayer::Node& _node)
:
	node(_node)
{
	node.AcceptMessages(
		std::bind(
			&Node::HandleIncomingMessage,
			this,
			std::placeholders::_1));
}

void Node::SndMessage(
	std::string destinationNode, 
	MessageVariant message)
{

	auto defaultSendMessageCallback = 
		std::bind(&Node::DefaultSendMessageCallback, this, std::placeholders::_1);

	SndMessage(destinationNode, message, defaultSendMessageCallback);
}

void Node::SndMessage(
	std::string destinationNode,
	MessageVariant message,
	std::function< void(NetworkLayer::SendError)> callback) const
{
	std::stringstream ss;
	boost::archive::binary_oarchive oarchive(ss);
//	boost::archive::text_oarchive oarchive(ss);
	oarchive << message;

	node.SndMessage(destinationNode, ss.str(), callback);
}

void Node::HandleIncomingMessage(NetworkLayer::DataMessage message)
{
	std::cout << node.Name() << " HandleIncomingMessage distance: " << message.Distance() << "\n";
	std::stringstream ss(std::move(message.Buffer()));
	boost::archive::binary_iarchive iarchive(ss);
//	boost::archive::text_iarchive iarchive(ss);

	MessageVariant messageV;
	try
	{
		iarchive >> messageV;
		boost::apply_visitor(MessageVisitor<Node>(*this), messageV);
	}
	catch (std::exception& e)
	{
		std::cout << node.Name() << " " << e.what() << "\n";
	}
}

void Node::DefaultSendMessageCallback(NetworkLayer::SendError error) const
{
	switch (error)
	{
	case NetworkLayer::eSuccess:
		{
		std::cout << node.Name() << " Success.\n";
		}
		break;
	case NetworkLayer::eNoPath:
		{
		std::cout << "No path.\n";
		}
		break;
	case NetworkLayer::eNodeNotAccepting:
		{
		std::cout << "Not accepting.\n";
		}
		break;
	default:
		break;
	}
}

template <>
void Node::HandleMessage(LogMessage& message)
{
	std::cout << node.Name() << " " << message.Log() << "\n";
}

template <>
void Node::HandleMessage(BrokerIdentity& message)
{
	std::cout << node.Name() << " " << message.NodeName() << "\n";
}
    
} // namespace LogicalLayer
