#include "publisher.hpp"

namespace LogicalLayer
{
	Publisher::Publisher(NetworkLayer::Node& _node)
	:
		node(_node)
	{}

	Publisher::~Publisher()
	{
	}
}
