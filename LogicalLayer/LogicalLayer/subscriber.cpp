#include "subscriber.hpp"

namespace LogicalLayer
{
	Subscriber::Subscriber(NetworkLayer::Node& _node)
	:
		node(_node)
	{}

	Subscriber::~Subscriber()
	{
	}
}
