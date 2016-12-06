#ifndef HIGH_LEVEL_NODE_HPP
#define HIGH_LEVEL_NODE_HPP

#include "allMessages.hpp"
#include "../../nodeLib/nodeLib/sendError.hpp"
#include "../../nodeLib/nodeLib/allMessages.hpp"

#include <functional>

namespace MeshNetwork {
	class Node;
}

class HighLevelNode
{
public:
	HighLevelNode(MeshNetwork::Node& _node);

	void SendHLMessage(std::string destinationNode,
		MessageVariant message);

	void SendHLMessage(std::string destinationNode,
		MessageVariant message,
		std::function< void(MeshNetwork::SendError)> callback) const;
private:
	friend struct HighLevelVisitor;

	void HandleIncomingMessage(MeshNetwork::DataMessage message);
	void DefaultSendMessageCallback(MeshNetwork::SendError error);

	template <typename MessageT>
	void HandleMessage(MessageT& message);

private:
	MeshNetwork::Node& node;
};

#endif