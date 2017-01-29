#ifndef HIGH_LEVEL_NODE_HPP
#define HIGH_LEVEL_NODE_HPP

#include "allMessages.hpp"
#include "../../nodeLib/nodeLib/sendError.hpp"
#include "../../nodeLib/nodeLib/allMessages.hpp"

#include <functional>
#include "messageVisitor.hpp"

namespace NetworkLayer {
	class Node;
}

namespace LogicalLayer
{

class Node
{
public:
	explicit Node(NetworkLayer::Node& _node);

	void SndMessage(
		std::string destinationNode,
		MessageVariant message);

	void SndMessage(
		std::string destinationNode,
        MessageVariant message,
        std::function< void(NetworkLayer::SendError)> callback) const;
    
private:
	friend struct MessageVisitor<Node>;

	void HandleIncomingMessage(NetworkLayer::DataMessage message);
	void DefaultSendMessageCallback(NetworkLayer::SendError error) const;

	template <typename MessageT>
	void HandleMessage(MessageT& message);

	NetworkLayer::Node& node;
};
    
}

#endif
