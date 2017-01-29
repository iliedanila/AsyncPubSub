#ifndef _LOGICAL_LAYER_SUBSCRIBER_HPP_
#define _LOGICAL_LAYER_SUBSCRIBER_HPP_

namespace NetworkLayer {
	class Node;
}

namespace LogicalLayer
{
	class Subscriber
	{
	public:
		explicit Subscriber(NetworkLayer::Node& node);
		~Subscriber();

	private:
		NetworkLayer::Node& node;
	};
}

#endif
