#include <iostream>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

#include "arguments.hpp"
#include "../../nodeLib/nodeLib/node.hpp"
#include "../../LogicalLayer/LogicalLayer/broker.hpp"
#include "../../LogicalLayer/LogicalLayer/subscriber.hpp"
#include "../../LogicalLayer/LogicalLayer/publisher.hpp"


int main(int argc, const char * argv[])
{
    boost::asio::io_service io_service;
    
    try
    {
        // startNode --name someName --accept port --connect ip1:port1 --connect ip2:port2 --as broker
        if(argc < 3)
        {
            std::string message;
            message += "Example:\n";
            message += "startNode --name someName ";
            message += "--accept portNo ";
            message += "--connect ip1:portNo1 --connect ip2:portNo2 ";
            message += "--as broker";
            
            throw std::runtime_error(message);
        }

        Arguments args(argc, argv);
        
        // Name.
        auto nameVector = args.GetArgument("--name");
        if(nameVector.size() != 1)
        {
            throw std::runtime_error("Name is required. Only one name.");
        }
        
        NetworkLayer::Node node(nameVector[0], io_service);
        
        // Accept.
        auto acceptPortVector = args.GetArgument("--accept");
        if(acceptPortVector.size() == 1)
        {
            node.Accept(std::stoi(acceptPortVector[0]));
        }
        
        // Connect.
        auto connectVector = args.GetArgument("--connect");
        for(auto hostPort : connectVector)
        {
            std::vector<std::string> hostPortPair;
            boost::split(hostPortPair, hostPort, boost::is_any_of(":"));
            node.Connect(hostPortPair[0], std::stoi(hostPortPair[1]));
        }
        
        // Publisher Subscriber Broker
        std::unique_ptr<LogicalLayer::Publisher> publisher;
        std::unique_ptr<LogicalLayer::Broker> broker;
        std::unique_ptr<LogicalLayer::Subscriber> subscriber;
        
        auto identityVector = args.GetArgument("--as");
        {
            if(identityVector.size() == 1)
            {
                if(identityVector[0] == "broker")
                {
                    broker.reset(new LogicalLayer::Broker(node));
                }
                
                if(identityVector[0] == "publisher")
                {
                    publisher.reset(new LogicalLayer::Publisher(node));
                }
                
                if(identityVector[0] == "subscriber")
                {
                    subscriber.reset(new LogicalLayer::Subscriber(node));
                }
            }
        }
        
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << "\n";
    }
}
