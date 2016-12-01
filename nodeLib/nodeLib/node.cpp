#include "node.hpp"
#include "connection.hpp"
#include "allMessages.hpp"
#include "messageVisitor.hpp"

#include <string>
#include <functional>

using namespace boost::asio::ip;

namespace MeshNetwork
{

Node::Node(std::string _name,
                         boost::asio::io_service& _io_service)
:
    name(std::move(_name)),
    io_service(_io_service),
    accept_socket(_io_service),
    connect_socket(_io_service)
{}

Node::~Node()
{}

void Node::Accept(unsigned short _port)
{
    tcp::endpoint endpoint(tcp::v4(), _port);
    
    if (!acceptor)
    {
        acceptor = std::make_unique<tcp::acceptor>(io_service, endpoint);
    }
    
    acceptor->async_accept(accept_socket,
                          [this, _port]
                          (const boost::system::error_code& error)
                          {
                              if (!error)
                              {
                                  auto connection = AddConnection(std::move(accept_socket));
                                  
                                  connection->Read(std::bind(&Node::OnRead,
                                                             this,
                                                             std::placeholders::_1,
                                                             std::placeholders::_2));
                                  
                                  SendRoutingToNewConnection(connection);
                              }
                              else
                              {
//                                  std::cout << name << " error accepting: "
//                                    << error.message() << "\n";
                              }
                              Accept(_port);
                          });
}

void Node::Connect(std::string host,
                          unsigned short port)
{
    tcp::resolver resolver(io_service);
    auto endpoint = resolver.resolve({ host, std::to_string(port)});
    
    boost::asio::async_connect(connect_socket,
                               endpoint,
                               [this]
                               (const boost::system::error_code& error,
                                boost::asio::ip::tcp::resolver::iterator)
                               {
                                   if (!error)
                                   {
                                       auto connection = AddConnection(std::move(connect_socket));

                                       connection->Read(std::bind(&Node::OnRead,
                                                                  this,
                                                                  std::placeholders::_1,
                                                                  std::placeholders::_2));
                                       
                                       SendRoutingToNewConnection(connection);
                                   }
                                   else
                                   {
//                                       std::cout << name << " error connecting: "
//                                       << error.message() << "\n";
                                   }
                               });
}

void Node::Close()
{
    for (auto connection : connections)
    {
        connection->Close();
    }
    if (acceptor)
    {
        acceptor->cancel();
    }
    
    connections.clear();
    nodeDistances.clear();
    nodePaths.clear();
}

std::vector<std::string> Node::GetAccessibleNodes()
{
    std::vector<std::string> nodeList;
    for ( auto keyValue : nodeDistances )
    {
        nodeList.push_back(keyValue.first);
    }
    return nodeList;
}

bool Node::IsNodeAccessible( const std::string& nodeName )
{
    return nodeDistances.find(nodeName) != nodeDistances.end();
}

void Node::SndMessage(std::string destination,
                              std::string buffer,
                              std::function< void(SendError)> callback)
{
    if (IsNodeAccessible(destination))
    {
        auto namePath = nodePaths.find(destination);
        auto connection = namePath->second;
        
        DataMessage message(name, destination, buffer);
        
        messageCallback = callback;
        connection->Write(message, std::bind(&Node::OnWrite, this));
    }
    else
    {
        callback(eNoPath);
    }
}
    
void Node::AcceptMessages(std::function<void (std::string, std::string)> callback)
{
    messageAcceptor = callback;
}

void Node::CloseConnection(SharedConnection connectionDown)
{
    RoutingMessage nodesDown;
    std::vector<std::string> toDelete;
    for (auto nodePath : nodePaths)
    {
        if (nodePath.second == connectionDown)
        {
            nodesDown.AddFailedNode(nodePath.first);
            toDelete.push_back(nodePath.first);
        }
    }
    
    for (auto key : toDelete)
    {
        nodePaths.erase(key);
        nodeDistances.erase(key);
    }
    
    connectionDown->Close();
    connections.erase(connectionDown);
    
    for (auto connection : connections)
    {
        connection->Write(nodesDown, std::bind(&Node::OnWrite, this));
    }
}

void Node::SendRoutingToNewConnection(SharedConnection connection)
{
    RoutingMessage message;
    message.AddNodeDistance(std::make_pair(name, 0));
    
    for (auto nodeDistance : nodeDistances)
    {
        message.AddNodeDistance(nodeDistance);
    }
    connection->Write(message, std::bind(&Node::OnWrite, this));
}

SharedConnection Node::AddConnection(
    boost::asio::ip::tcp::socket&& _socket)
{
    auto connection = std::make_shared<Connection>(*this,
                                                   io_service,
                                                   std::move(_socket),
                                                   std::bind(&Node::CloseConnection,
                                                             this,
                                                             std::placeholders::_1));
    
    connections.insert(connection);
    return connection;
}

void Node::OnRead(MessageVariant _message, SharedConnection _connection)
{
    boost::apply_visitor(MessageVisitor(*this, _connection), _message);
}

void Node::OnWrite()
{
}

template <>
void Node::HandleMessage(RoutingMessage& _message, SharedConnection _connection)
{
    RoutingMessage reply;
    RoutingMessage forward;
    
    ProcessAddNodePaths(_message, reply, forward, _connection);
    ProcessFailedNodes(_message, reply, forward, _connection);
    
    if (reply.NodeDistances().size() > 0 ||
        reply.FailedNodes().size() > 0)
    {
        _connection->Write(reply, std::bind(&Node::OnWrite, this));
    }
    
    if (forward.NodeDistances().size() > 0 ||
        forward.FailedNodes().size() > 0)
    {
        for (auto conn : connections)
        {
            if (conn != _connection)
            {
                conn->Write(forward, std::bind(&Node::OnWrite, this));
            }
        }
    }
}

template<>
void Node::HandleMessage(DataMessage& _message, SharedConnection _connection)
{
    if (_message.destinationNodeName == name)
    {
        if (messageAcceptor)
        {
            messageAcceptor(_message.sourceNodeName, _message.buffer);
            DataMessageAck message(name, _message.sourceNodeName, eSuccess);
            _connection->Write(message, std::bind(&Node::OnWrite, this));

        }
        else
        {
            DataMessageAck message(_message.sourceNodeName, name, eNodeNotAccepting);
            _connection->Write(message, std::bind(&Node::OnWrite, this));
        }
    }
    else
    {
        if (IsNodeAccessible(_message.destinationNodeName))
        {
            auto namePath = nodePaths.find(_message.destinationNodeName);
            auto connection = namePath->second;
            connection->Write(_message, std::bind(&Node::OnWrite, this));
        }
        else
        {
            DataMessageAck message(_message.sourceNodeName, name, eNoPath);
            _connection->Write(message, std::bind(&Node::OnWrite, this));
        }
    }
}
    
template<>
void Node::HandleMessage(DataMessageAck& _message, SharedConnection _connection)
{
    if (_message.destinationNodeName == name)
    {
        messageCallback(_message.error);
    }
    else
    {
        if (IsNodeAccessible(_message.destinationNodeName))
        {
            auto namePath = nodePaths.find(_message.destinationNodeName);
            auto connection = namePath->second;
            connection->Write(_message, std::bind(&Node::OnWrite, this));
        }
    }
}

void Node::ProcessAddNodePaths(RoutingMessage& message,
                                      RoutingMessage& reply,
                                      RoutingMessage& forward,
                                      SharedConnection connection)
{
    for (auto nodeDistance : message.NodeDistances())
    {
        auto it = nodeDistances.find(nodeDistance.first);
        if (it == nodeDistances.end())
        {
            auto nodeDist = std::make_pair(nodeDistance.first, nodeDistance.second + 1);
            forward.AddNodeDistance(nodeDist);
            
            nodeDistances.insert(nodeDist);
            nodePaths.insert(std::make_pair(nodeDistance.first, connection));
        }
        else if (it->second > nodeDistance.second + 1)
        {
            auto nodeDist = std::make_pair(nodeDistance.first, nodeDistance.second + 1);
            forward.AddNodeDistance(nodeDist);
            
            it->second = nodeDistance.second + 1;
            auto itNodePath = nodePaths.find(nodeDistance.first);
            itNodePath->second = connection;
        }
        else if (it->second + 1 < nodeDistance.second)
        {
            reply.AddNodeDistance(*it);
        }
    }
}

void Node::ProcessFailedNodes(RoutingMessage& message,
                                     RoutingMessage& reply,
                                     RoutingMessage& forward,
                                     SharedConnection connection)
{
    for (auto node : message.FailedNodes())
    {
        auto it = nodePaths.find(node);
        if (it != nodePaths.end())
        {
            if (it->second == connection)
            {
                nodeDistances.erase(node);
                nodePaths.erase(node);
                forward.AddFailedNode(node);
            }
            else
            {
                auto iterator = nodeDistances.find(node);
                reply.AddNodeDistance(*iterator);
            }
        }
    }
}
    
}
