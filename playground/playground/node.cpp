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
                        std::bind(&Node::HandleIncomingMessage,
                            this,
                            std::placeholders::_1));
}

void Node::SendMessage(
	std::string destinationNode, 
	MessageVariant message)
{

	auto defaultSendMessageCallback = 
		std::bind(&Node::DefaultSendMessageCallback, this, std::placeholders::_1);

	SendMessage(destinationNode, message, defaultSendMessageCallback);
}

void Node::SendMessage(
	std::string destinationNode,
	MessageVariant message,
	std::function< void(NetworkLayer::SendError)> callback) const
{
	std::stringstream ss;
	boost::archive::binary_oarchive oarchive(ss);
	oarchive << message;

	node.SndMessage(destinationNode, ss.str(), callback);
}

void Node::HandleIncomingMessage(NetworkLayer::DataMessage message)
{
	std::cout << "HandleIncomingMessage distance: " << message.Distance() << "\n";
	std::stringstream ss(std::move(message.Buffer()));
	boost::archive::binary_iarchive iarchive(ss);

	MessageVariant messageV;
	iarchive >> messageV;

	boost::apply_visitor(MessageVisitor(*this), messageV);
}

void Node::DefaultSendMessageCallback(NetworkLayer::SendError error)
{
	switch (error)
	{
	case NetworkLayer::eSuccess:
		{
		std::cout << "Success.\n";
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
	std::cout << message.Log() << "\n";
}
    
} // namespace LogicalLayer
