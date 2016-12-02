#include <boost/serialization/variant.hpp>

#include "HighLevelNode.hpp"
#include "../../nodeLib/nodeLib/node.hpp"
#include <iostream>
#include "highLevelVisitor.hpp"

HighLevelNode::HighLevelNode(MeshNetwork::Node& _node)
:
	node(_node)
{
	node.AcceptMessages(std::bind(&HighLevelNode::HandleIncomingMessage,
		this,
		std::placeholders::_1,
		std::placeholders::_2));
}

void HighLevelNode::SendHLMessage(
	std::string destinationNode, 
	MessageVariant message)
{

	auto defaultSendMessageCallback = 
		std::bind(&HighLevelNode::DefaultSendMessageCallback, this, std::placeholders::_1);

	SendHLMessage(destinationNode, message, defaultSendMessageCallback);	
}

void HighLevelNode::SendHLMessage(
	std::string destinationNode,
	MessageVariant message,
	std::function< void(MeshNetwork::SendError)> callback)
{
	std::stringstream ss;
	boost::archive::binary_oarchive oarchive(ss);
	oarchive << message;

	node.SndMessage(destinationNode, ss.str(), callback);
}

void HighLevelNode::HandleIncomingMessage(std::string sourceNode, std::string buffer)
{
	std::stringstream ss(std::move(buffer));
	boost::archive::binary_iarchive iarchive(ss);

	MessageVariant message;
	iarchive >> message;

	boost::apply_visitor(HighLevelVisitor(*this), message);
}

void HighLevelNode::DefaultSendMessageCallback(MeshNetwork::SendError error)
{
	switch (error)
	{
	case MeshNetwork::eSuccess:
		{
		std::cout << "Success.\n";
		}
		break;
	case MeshNetwork::eNoPath:
		{
		std::cout << "No path.\n";
		}
		break;
	case MeshNetwork::eNodeNotAccepting:
		{
		std::cout << "Not accepting.\n";
		}
		break;
	default:
		break;
	}
}

template <>
void HighLevelNode::HandleMessage(LogMessage& message)
{
	std::cout << message.Log() << "\n";
}
