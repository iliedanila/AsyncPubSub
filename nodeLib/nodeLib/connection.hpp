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
typedef std::function<void(MessageVariant, SharedConnection)> ReadCallback;

class Connection : public std::enable_shared_from_this<Connection>
{
public:
    Connection(
        Node& _node,
        io_service& _io_service,
        tcp::socket&& _socket,
        std::function<void(std::shared_ptr<Connection>)> _closeHandler);

    ~Connection();
    
    void Read(ReadCallback _callback);
    
    void Write(MessageVariant, std::function<void()>);
    
    void Close();
    
private:
    io_service& io_service;
    tcp::socket socket;
    std::function<void(std::shared_ptr<Connection>)> closeHandler;
    
    Node& node;
    char readMessage[Node::MaxMessageSize];
    char writeMessage[Node::MaxMessageSize];
};
    
}

#endif /* connection_hpp */
