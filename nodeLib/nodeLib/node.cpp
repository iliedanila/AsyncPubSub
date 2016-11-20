#include "node.hpp"
#include "connection.hpp"
#include "allMessages.hpp"
#include "messageVisitor.hpp"

#include <string>
#include <iostream>
#include <functional>

using namespace boost::asio::ip;

NetworkNode::NetworkNode(std::string&& _name,
                         boost::asio::io_service& _io_service)
:
    name(std::move(_name)),
    io_service(_io_service),
    accept_socket(_io_service),
    connect_socket(_io_service)
{}

NetworkNode::~NetworkNode()
{}

void NetworkNode::Accept(unsigned short _port)
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
                                  
                                  connection->Read(std::bind(&NetworkNode::OnRead,
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

void NetworkNode::Connect(std::string host,
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

                                       connection->Read(std::bind(&NetworkNode::OnRead,
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

void NetworkNode::Close()
{
    for (auto connection : connections)
    {
        connection->Close();
    }
    connections.clear();
}

std::vector<std::string> NetworkNode::GetAccessibleNodes()
{
    std::vector<std::string> nodeList;
    for ( auto keyValue : nodeDistances )
    {
        nodeList.push_back(keyValue.first);
    }
    return nodeList;
}

bool NetworkNode::IsNodeAccessible( const std::string& nodeName )
{
    return nodeDistances.find(nodeName) != nodeDistances.end();
}

void NetworkNode::SendMessage(std::string destination, std::string message)
{
    
}

void NetworkNode::CloseConnection(SharedConnection connectionDown)
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
        connection->Write(nodesDown, std::bind(&NetworkNode::OnWrite, this));
    }
}

void NetworkNode::SendRoutingToNewConnection(SharedConnection connection)
{
    RoutingMessage message;
    message.AddNodeDistance(std::make_pair(name, 0));
    
    for (auto nodeDistance : nodeDistances)
    {
        message.AddNodeDistance(nodeDistance);
    }
    connection->Write(message, std::bind(&NetworkNode::OnWrite, this));
}

SharedConnection NetworkNode::AddConnection(
    boost::asio::ip::tcp::socket&& _socket)
{
    auto connection = std::make_shared<Connection>(*this,
                                                   io_service,
                                                   std::move(_socket),
                                                   std::bind(&NetworkNode::CloseConnection,
                                                             this,
                                                             std::placeholders::_1));
    
    connections.insert(connection);
    return connection;
}

void NetworkNode::OnRead(MessageVariant _message, SharedConnection _connection)
{
    boost::apply_visitor(MessageVisitor(*this, _connection), _message);
}

void NetworkNode::OnWrite()
{
}

template <>
void NetworkNode::HandleMessage(RoutingMessage& _message, SharedConnection _connection)
{
    RoutingMessage reply;
    RoutingMessage forward;
    
    ProcessAddNodePaths(_message, reply, forward, _connection);
    ProcessFailedNodes(_message, reply, forward, _connection);
    
    if (reply.NodeDistances().size() > 0 ||
        reply.FailedNodes().size() > 0)
    {
        _connection->Write(reply, std::bind(&NetworkNode::OnWrite, this));
    }
    
    if (forward.NodeDistances().size() > 0 ||
        forward.FailedNodes().size() > 0)
    {
        for (auto conn : connections)
        {
            if (conn != _connection)
            {
                conn->Write(forward, std::bind(&NetworkNode::OnWrite, this));
            }
        }
    }
}

template<>
void NetworkNode::HandleMessage(DataMessage& _message, SharedConnection _connection)
{
    
}

void NetworkNode::ProcessAddNodePaths(RoutingMessage& message,
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

void NetworkNode::ProcessFailedNodes(RoutingMessage& message,
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