#include <iostream>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

#include "arguments.hpp"
#include "node.hpp"
#include "broker.hpp"
#include "subscriber.hpp"
#include "publisher.hpp"


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
            message += "--publish DataAttrib ";
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
            node.accept(std::stoi(acceptParameters[0]));
        }
        
        // Connect.
        while (args.HasArgument("--connect"))
        {
            auto connectParameters = args.GetParameters("--connect", 2);
            node.connect(connectParameters[0], std::stoi(connectParameters[1]), false);
        }

        while (args.HasArgument("--connect-reconnect"))
        {
            auto connectParameters = args.GetParameters("--connect-reconnect", 2);
            node.connect(connectParameters[0], std::stoi(connectParameters[1]), true);
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
                while(args.HasArgument("--publish"))
                {
                    auto identityParameters = args.GetParameters("--publish", 1);
                    publisherIdentity.insert(identityParameters[0]);
                }
                publisher.reset(new LogicalLayer::Publisher(node, publisherIdentity));

                auto intervalParameters = args.GetParameters("--interval", 1);
                publisher->startPublishing(
                        [&]() -> LogicalLayer::SubscriptionData {
                            return LogicalLayer::SubscriptionData(
                                    node.getName(),
                                    "Message from Publisher");
                        }, std::stoi(intervalParameters[0]));
            }
            else if (asParameters[0] == "subscriber")
            {
                subscriber.reset(new LogicalLayer::Subscriber(node));

                LogicalLayer::SubscriptionT subscription;
                while(args.HasArgument("--subscription"))
                {
                    auto subscriptionParameters = args.GetParameters("--subscription", 1);
                    subscription.insert(subscriptionParameters[0]);
                }
                if (subscription.size() > 0)
                {
                    subscriber->addSubscription(
                            subscription,
                            [&node]
                                    (LogicalLayer::SubscriptionData &publisherData) {
                                node.log("Publisher data: " + publisherData.getData());
                            }
                    );
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
