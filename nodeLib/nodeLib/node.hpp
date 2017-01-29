#ifndef node_hpp
#define node_hpp

#include <boost/asio.hpp>
#include <functional>
#include <vector>
#include <set>
#include <map>

#include "allMessages.hpp"
#include "sendError.hpp"

using namespace boost::asio;
using namespace ip;

typedef std::shared_ptr<tcp::acceptor> SharedAcceptor;
typedef std::pair<std::string, std::size_t> NodeDistance;

namespace NetworkLayer
{
    
class Connection;
typedef std::shared_ptr<Connection> SharedConnection;

class Node
{
public:
	static const int MaxMessageSize = 2048;

	Node(
		std::string _name,
		io_service& _io_service);
    ~Node();
    
    void Accept(unsigned short _port);
    void Connect(std::string _host, unsigned short _port);
    
    std::string Name() const { return name; }
    void Close();
    
    std::vector<std::string> GetAccessibleNodes();
    
    bool IsNodeAccessible(const std::string& nodeName);
    
    void RegisterNodeAccessibility(std::function<void(std::string, bool)> callback);
    
    void SndMessage(
		std::string destination,
        std::string data,
        std::function< void(SendError)> callback);
    
    void AcceptMessages(std::function< void(DataMessage) > callback);
    
private:
    friend struct MessageVisitor;
    
    SharedConnection AddConnection(tcp::socket&&);
    
    void CloseConnection(SharedConnection);
    
    void SendRoutingToNewConnection(SharedConnection);
    
    void OnRead(MessageVariant, SharedConnection);
    
    void OnWrite();
    
    template <typename MessageT>
    void HandleMessage(MessageT&, SharedConnection);
    
    void ProcessAddNodePaths(
		RoutingMessage& message,
		RoutingMessage& reply,
		RoutingMessage& forward,
		SharedConnection connection);

    void ProcessFailedNodes(
		RoutingMessage& message,
		RoutingMessage& reply,
		RoutingMessage& forward,
		SharedConnection connection);

	std::string name;
    std::set<SharedConnection> connections;
    std::map<std::string, std::size_t> nodeDistances;
    std::map<std::string, SharedConnection> nodePaths;
    
    std::unique_ptr<tcp::acceptor> acceptor;
    
    bool closing;
    std::function<void(DataMessage)> messageAcceptor;
    std::function<void(SendError)> messageCallback;
    std::function<void(std::string, bool)> nodeAccessibilityCallback;
    
    tcp::socket connect_socket;
    tcp::socket accept_socket;
    boost::asio::io_service& io_service;
};
    
}

#endif /* node_hpp */
