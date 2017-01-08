#ifndef HIGH_LEVEL_VISITOR_HPP
#define HIGH_LEVEL_VISITOR_HPP
#include <boost/variant/static_visitor.hpp>

namespace LogicalLayer
{

class Node;

struct MessageVisitor : public boost::static_visitor<>
{
	MessageVisitor(Node& _node)
	:
		node(_node)
	{}

	template <typename MessageT>
	void operator()(MessageT& message) const
	{
		node.HandleMessage(message);
	}

private:
	Node& node;
};
    
}

#endif
