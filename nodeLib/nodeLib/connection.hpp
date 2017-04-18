#ifndef connection_hpp
#define connection_hpp

#include <boost/asio.hpp>
#include <memory>
#include <deque>

#include "allMessages.hpp"
#include "message.hpp"
#include "node.hpp"

using namespace boost::asio;
using namespace ip;

namespace NetworkLayer
{

class Connection;
typedef std::shared_ptr<Connection> SharedConnection;
typedef std::function<void(boost::system::error_code)> WriteCallback;
typedef std::function<void(MessageVariant, SharedConnection)> ReadCallback;
typedef std::deque<std::pair<Message, WriteCallback>> MessageQueue;

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
    
    void Send(MessageVariant, WriteCallback);

    void Write();
    
    void Close();
    
private:
    void ReadHeader(ReadCallback _callback);
    void ReadBody(ReadCallback _callback);

    io_service& io_service;
    tcp::socket socket;
    std::function<void(std::shared_ptr<Connection>)> closeHandler;
    
    Node& node;
    Message readMessage;
    MessageQueue writeMessages;
};
    
}

#endif /* connection_hpp */
