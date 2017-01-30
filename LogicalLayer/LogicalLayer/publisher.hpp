#ifndef _LOGICAL_LAYER_PUBLISHER_HPP_
#define _LOGICAL_LAYER_PUBLISHER_HPP_
#include "messageVisitor.hpp"

namespace NetworkLayer {
	class DataMessage;
	class Node;
}

namespace LogicalLayer
{
	class Publisher
	{
	public:
		explicit Publisher(NetworkLayer::Node& node);
		~Publisher();

	private:
		friend struct MessageVisitor<Publisher>;
		void HandleIncomingMessage(NetworkLayer::DataMessage& message);

		template<typename MessageT>
		void HandleMessage(MessageT& message);

		NetworkLayer::Node& node;
	};
}

#endif
