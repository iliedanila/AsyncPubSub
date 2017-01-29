#ifndef _LOGICAL_LAYER_PUBLISHER_HPP_
#define _LOGICAL_LAYER_PUBLISHER_HPP_

namespace NetworkLayer {
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
		NetworkLayer::Node& node;
	};
}

#endif
