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
        if(argc < 3)
        {
            std::string message;
            message += "Example:\n";
            message += "startNode --name someName ";
            message += "--accept portNo ";
            message += "--connect ip1 portNo1 --connect ip2 portNo2 ";
            message += "--as publisher ";
            message += "--identity attrib value ";
            message += "--logger";
            
            throw std::runtime_error(message);
        }

        Arguments args(argc, argv);
        
        // Name.
        if (!args.HasArgument("--name"))
        {
            throw std::runtime_error("Name is required. Only one name.");
        }
        auto nameParameter = args.GetParameters("--name", 1);
        
        NetworkLayer::Node node(nameParameter[0], io_service, args.HasArgument("--logger"));
        
        // Accept.
        if (args.HasArgument("--accept"))
        {
            auto acceptParameters = args.GetParameters("--accept", 1);
            node.Accept(std::stoi(acceptParameters[0]));
        }
        
        // Connect.
        while (args.HasArgument("--connect"))
        {
            auto connectParameters = args.GetParameters("--connect", 2);
            node.Connect(connectParameters[0], std::stoi(connectParameters[1]), false);
        }

        while (args.HasArgument("--connect-reconnect"))
        {
            auto connectParameters = args.GetParameters("--connect-reconnect", 2);
            node.Connect(connectParameters[0], std::stoi(connectParameters[1]), true);
        }
        
        // Publisher Subscriber Broker
        std::unique_ptr<LogicalLayer::Publisher> publisher;
        std::unique_ptr<LogicalLayer::Broker> broker;
        std::unique_ptr<LogicalLayer::Subscriber> subscriber;

        if (args.HasArgument("--as"))
        {
            auto asParameters = args.GetParameters("--as", 1);
            if (asParameters[0] == "broker")
            {
                broker.reset(new LogicalLayer::Broker(node));
            }
            else if (asParameters[0] == "publisher")
            {
                LogicalLayer::PublisherIdentityT publisherIdentity;
                while(args.HasArgument("--identity"))
                {
                    auto identityParameters = args.GetParameters("--identity", 2);
                    publisherIdentity.insert(
                        std::make_pair(
                            identityParameters[0],
                            identityParameters[1]));
                }
                publisher.reset(new LogicalLayer::Publisher(node, publisherIdentity));

                auto intervalParameters = args.GetParameters("--interval", 1);
                publisher->StartPublishing(
                    [&]() -> LogicalLayer::PublisherData
                    {
                        return LogicalLayer::PublisherData(
                            node.Name(),
                            "Message from Publisher");
                    }, std::stoi(intervalParameters[0]));
            }
            else if (asParameters[0] == "subscriber")
            {
                subscriber.reset(new LogicalLayer::Subscriber(node));

                LogicalLayer::SubscriptionT subscription;
                while(args.HasArgument("--subscription"))
                {
                    auto subscriptionParameters = args.GetParameters("--subscription", 2);
                    subscription.insert(
                        std::make_pair(
                            subscriptionParameters[0],
                            subscriptionParameters[1]));
                }
                if (subscription.size() > 0)
                {
                    subscriber->AddSubscription(subscription);
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
