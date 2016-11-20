#ifndef messageVisitor_h
#define messageVisitor_h

#include <boost/variant.hpp>

#include "allMessages.hpp"
#include "connection.hpp"
#include "node.hpp"

struct MessageVisitor : public boost::static_visitor<>
{
    MessageVisitor(NetworkNode& _node, SharedConnection _connection)
    :
        node(_node),
        connection(_connection)
    {}
    
    template <typename MessageT>
    void operator()(MessageT& message) const
    {
        node.HandleMessage(message, connection);
    }
    
private:
    SharedConnection connection;
    NetworkNode& node;
};

#endif /* messageVisitor_h */
