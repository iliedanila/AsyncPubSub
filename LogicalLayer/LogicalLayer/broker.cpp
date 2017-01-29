#include "broker.hpp"

namespace LogicalLayer
{
	Broker::Broker(NetworkLayer::Node& _node)
	:
		node(_node)
	{}

	Broker::~Broker()
	{
	}
}
