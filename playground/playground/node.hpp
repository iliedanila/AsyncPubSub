#ifndef HIGH_LEVEL_NODE_HPP
#define HIGH_LEVEL_NODE_HPP

#include "allMessages.hpp"
#include "../../nodeLib/nodeLib/sendError.hpp"
#include "../../nodeLib/nodeLib/allMessages.hpp"

#include <functional>

namespace NetworkLayer {
	class Node;
}

namespace LogicalLayer
{

class Node
{
public:
	Node(NetworkLayer::Node& _node);

	void SendMessage(
                     std::string destinationNode,
                     MessageVariant message);

	void SendMessage(
                     std::string destinationNode,
                     MessageVariant message,
                     std::function< void(NetworkLayer::SendError)> callback) const;
    
private:
	friend struct MessageVisitor;

	void HandleIncomingMessage(NetworkLayer::DataMessage message);
	void DefaultSendMessageCallback(NetworkLayer::SendError error);

	template <typename MessageT>
	void HandleMessage(MessageT& message);

private:
	NetworkLayer::Node& node;
};
    
}

#endif
