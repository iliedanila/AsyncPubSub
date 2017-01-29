#ifndef messageVisitor_h
#define messageVisitor_h

#include <boost/variant.hpp>

#include "node.hpp"

namespace NetworkLayer
{

struct MessageVisitor : public boost::static_visitor<>
{
    MessageVisitor(Node& _node, SharedConnection _connection)
    :
        connection(_connection),
        node(_node)
    {}
    
    template <typename MessageT>
    void operator()(MessageT& message) const
    {
        node.HandleMessage(message, connection);
    }
    
private:
    SharedConnection connection;
    Node& node;
};
    
}

#endif /* messageVisitor_h */
