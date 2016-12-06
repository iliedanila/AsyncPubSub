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
		std::placeholders::_1));
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
	std::function< void(MeshNetwork::SendError)> callback) const
{
	std::stringstream ss;
	boost::archive::binary_oarchive oarchive(ss);
	oarchive << message;

	node.SndMessage(destinationNode, ss.str(), callback);
}

void HighLevelNode::HandleIncomingMessage(MeshNetwork::DataMessage message)
{
	std::cout << "HandleIncomingMessage distance: " << message.Distance() << "\n";
	std::stringstream ss(std::move(message.Buffer()));
	boost::archive::binary_iarchive iarchive(ss);

	MessageVariant messageV;
	iarchive >> messageV;

	boost::apply_visitor(HighLevelVisitor(*this), messageV);
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
