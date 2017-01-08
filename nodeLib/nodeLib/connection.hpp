#ifndef connection_hpp
#define connection_hpp

#include <boost/asio.hpp>
#include <memory>

#include "allMessages.hpp"
#include "binaryMessage.hpp"

using namespace boost::asio;
using namespace ip;

namespace NetworkLayer
{
    
class Node;
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
    void ReadHeader(std::function<void(MessageVariant, std::shared_ptr<Connection>)>);
    void ReadBody(std::function<void(MessageVariant, std::shared_ptr<Connection>)>);

private:
    boost::asio::io_service& io_service;
    tcp::socket socket;
    std::function<void(std::shared_ptr<Connection>)> closeHandler;
    
    Node& node;
    BinaryMessage readMessage;
    BinaryMessage writeMessage;
};
    
}

#endif /* connection_hpp */
