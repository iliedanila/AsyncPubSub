#include "connection.hpp"
#include "allMessages.hpp"
#include "node.hpp"

#include <iostream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/variant.hpp>

namespace NetworkLayer
{

Connection::Connection(
    Node& _node,
    boost::asio::io_service& _io_service,
    tcp::socket&& _socket,
    std::function<void(std::shared_ptr<Connection>)> _closeHandler)
:
    io_service(_io_service),
    socket(std::move(_socket)),
    closeHandler(_closeHandler),
    node(_node)
{
    std::cout << "Connection() in node " << node.Name() << "\n";
}

Connection::~Connection()
{
    std::cout << "~Connection() in node " << node.Name() << "\n";
}

void Connection::Write()
{
    auto self(shared_from_this());
    boost::asio::async_write(
        socket,
        buffer(
            writeMessages.front().first.GetOutputBuffer(),
            writeMessages.front().first.GetOutputBuffer().size()
        ),
        [this, self]
        (boost::system::error_code error, std::size_t /*lenght*/)
        {
            if(!error)
            {
                // Callback.
                writeMessages.front().second(error);

                writeMessages.pop_front();
                if(!writeMessages.empty())
                {
                    Write();
                }
            }
        }
    );
}

void Connection::Read(ReadCallback _callback)
{
    ReadHeader(_callback);
}

void Connection::Send(MessageVariant _message, WriteCallback _callback)
{
    std::stringstream ss;
    boost::archive::binary_oarchive oarchive(ss);
    oarchive << _message;

    Message message(ss.str());
    message.CreateOutputBuffer();

    auto writeInProgress = !writeMessages.empty();
    writeMessages.push_back(std::make_pair(message, _callback));

    if (!writeInProgress)
    {
        Write();
    }
}

void Connection::Close()
{
    socket.close();
}

void Connection::ReadHeader(ReadCallback _callback)
{
    auto self(shared_from_this());

    boost::asio::async_read(
        socket,
        buffer(readMessage.GetHeader(), Message::eHeaderLength),
        [this, self, _callback]
        (boost::system::error_code error_code, std::size_t length)
        {
            if(!error_code)
            {
                readMessage.DecodeHeader();
                ReadBody(_callback);
            }
        }
    );
}

void Connection::ReadBody(ReadCallback _callback)
{
    auto self(shared_from_this());

    boost::asio::async_read(
        socket,
        buffer(readMessage.GetBody(), readMessage.GetBodySize()),
        [this, self, _callback]
        (boost::system::error_code error_code, std::size_t /*length*/)
        {
            if (!error_code)
            {
                std::string content(readMessage.GetBody().begin(), readMessage.GetBody().end());
                std::stringstream ss(content);
                boost::archive::binary_iarchive iarchive(ss);

                MessageVariant message;
                iarchive >> message;

                _callback(message, self);
                ReadHeader(_callback);
            }
        }
    );
}

}
