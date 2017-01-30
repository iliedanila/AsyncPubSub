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

void Node::HandleIncomingMessage(NetworkLayer::DataMessage& message)
{
	std::stringstream ss(std::move(message.Buffer()));
	boost::archive::binary_iarchive iarchive(ss);

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

void Node::DefaultSendMessageCallback(
	std::string nodeName,
	NetworkLayer::SendError error) const
{
	switch (error)
	{
	case NetworkLayer::eSuccess:
		{
		std::cout << node.Name() << " --> " << nodeName << " Success.\n";
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
	std::cout	<< node.Name() 
				<< " has received broker identity: "
				<< " " << message.NodeName() 
				<< "\n";
}

template <>
void Node::HandleMessage(Subscription& message)
{
	std::cout << node.Name() << " Subscription" << "\n";
}
    
} // namespace LogicalLayer
