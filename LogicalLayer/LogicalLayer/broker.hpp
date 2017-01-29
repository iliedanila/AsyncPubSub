#ifndef _LOGICAL_LAYER_BROKER_HPP_
#define _LOGICAL_LAYER_BROKER_HPP_

namespace NetworkLayer {
	class Node;
}

namespace LogicalLayer
{
	class Broker
	{
	public:
		explicit Broker(NetworkLayer::Node& node);
		~Broker();

	private:
		NetworkLayer::Node& node;
	};
}

#endif