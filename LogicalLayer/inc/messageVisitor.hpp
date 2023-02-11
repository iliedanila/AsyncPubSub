#ifndef HIGH_LEVEL_VISITOR_HPP
#define HIGH_LEVEL_VISITOR_HPP

#include <boost/variant/static_visitor.hpp>

namespace LogicalLayer {

template <class NodeT>
struct MessageVisitor : public boost::static_visitor<> {
    explicit MessageVisitor(NodeT& _node) : node(_node) {}

    template <typename MessageT>
    void operator()(MessageT& message) const {
        node.handleMessage(message);
    }

   private:
    NodeT& node;
};

}  // namespace LogicalLayer

#endif
