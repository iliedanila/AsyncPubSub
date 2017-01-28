#include "connection.hpp"
#include "allMessages.hpp"
#include "node.hpp"

#include <iostream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/variant.hpp>

namespace NetworkLayer
{

Connection::Connection(Node& _node,
                       boost::asio::io_service& _io_service,
                       tcp::socket&& _socket,
                       std::function<void(std::shared_ptr<Connection>)> _closeHandler)
:
    node(_node),
    io_service(_io_service),
    socket(std::move(_socket)),
    closeHandler(_closeHandler)
{
    std::cout << "Connection() in node " << node.Name() << "\n";
}

Connection::~Connection()
{
    std::cout << "~Connection() in node " << node.Name() << "\n";
}

void Connection::Read(std::function<void(MessageVariant, std::shared_ptr<Connection> )> _callback)
{
    ReadHeader(_callback);
}

void Connection::Write(MessageVariant _message, std::function<void()> _callback)
{
    auto self(shared_from_this());
    std::stringstream ss;
    boost::archive::binary_oarchive oarchive(ss);
    oarchive << _message;

    writeMessage.body_length(ss.str().size());
    std::memcpy(writeMessage.body(), ss.str().c_str(), writeMessage.body_length());
    writeMessage.encode_header();
    
    boost::asio::async_write(socket,
                             boost::asio::buffer(writeMessage.data(), writeMessage.length()),
                             [self, _callback]
                             (boost::system::error_code error, std::size_t lenght)
                             {
                                if (!error)
                                {
                                    _callback();
                                }
                                else
                                {
                                    std::cout << "Error writing: " << error.message() << "\n";
                                }
                             });
}

void Connection::Close()
{
    socket.close();
}

void Connection::ReadHeader(std::function<void(MessageVariant, std::shared_ptr<Connection>)> _callback)
{
    auto self(shared_from_this());
    boost::asio::async_read(socket,
                            boost::asio::buffer(readMessage.data(), BinaryMessage::header_length),
                            [this, self, _callback]
                            (const boost::system::error_code& error,
                             std::size_t bytes_transferred)
                            {
                                if (!error && readMessage.decode_header())
                                {
                                    ReadBody(_callback);
                                }
                                else
                                {
                                    std::cout << "Error reading header: " << error.value() << " " << error.message()
                                    << " on node " << node.Name() << "\n";
                                    if (error.value() == ENOENT || error.value() == ECANCELED)
                                    {
                                        closeHandler(shared_from_this());
                                    }
                                }

                            });
}

void Connection::ReadBody(std::function<void(MessageVariant, std::shared_ptr<Connection>)> _callback)
{
    auto self(shared_from_this());
    boost::asio::async_read(socket,
                            boost::asio::buffer(readMessage.body(), readMessage.body_length()),
                            [this, self, _callback]
                            (const boost::system::error_code& error, std::size_t /*length*/)
                            {
                                if( !error)
                                {
                                    std::string content(readMessage.body(), readMessage.body_length());
                                    std::stringstream ss(content);
                                    boost::archive::binary_iarchive iarchive(ss);
                                    
                                    MessageVariant message;
                                    iarchive >> message;

                                    _callback(message, self);
                                    ReadHeader(_callback);

                                }
                                else
                                {
                                    std::cout << "Error reading body: " << error.value() << " " << error.message()
                                    << " on node " << node.Name() << "\n";
                                    if (error.value() == 2)
                                    {
                                        closeHandler(shared_from_this());
                                    }
                                }
                            });
}
    
}
