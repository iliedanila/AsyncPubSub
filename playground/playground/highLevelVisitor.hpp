#ifndef HIGH_LEVEL_VISITOR_HPP
#define HIGH_LEVEL_VISITOR_HPP
#include <boost/variant/static_visitor.hpp>

class HighLevelNode;

struct HighLevelVisitor : public boost::static_visitor<>
{
	HighLevelVisitor(HighLevelNode& _node)
	:
		node(_node)
	{}

	template <typename MessageT>
	void operator()(MessageT& message) const
	{
		node.HandleMessage(message);
	}

private:
	HighLevelNode& node;
};

#endif
