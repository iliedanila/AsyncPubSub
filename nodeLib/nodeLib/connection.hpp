#ifndef connection_hpp
#define connection_hpp

#include <boost/asio.hpp>
#include <memory>

#include "allMessages.hpp"
#include "node.hpp"

using namespace boost::asio;
using namespace ip;

namespace NetworkLayer
{
    
class Connection;
typedef std::shared_ptr<Connection> SharedConnection;


class Connection : public std::enable_shared_from_this<Connection>
{
public:
    Connection(Node& _node,
               io_service&,
               tcp::socket&&,
               std::function<void(std::shared_ptr<Connection>)>);
    ~Connection();
    
    void Read(std::function<void(MessageVariant, std::shared_ptr<Connection>)>);
    
    void Write(MessageVariant, std::function<void()>);
    
    void Close();
    
private:
    boost::asio::io_service& io_service;
    tcp::socket socket;
    std::function<void(std::shared_ptr<Connection>)> closeHandler;
    
    Node& node;
    char readMessage[Node::MaxMessageSize];
    char writeMessage[Node::MaxMessageSize];
};
    
}

#endif /* connection_hpp */
