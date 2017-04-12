#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <thread>
#include <chrono>

#include "../../nodeLib/nodeLib/node.hpp"
#include "../../LogicalLayer/LogicalLayer/broker.hpp"
#include "../../LogicalLayer/LogicalLayer/subscriber.hpp"
#include "../../LogicalLayer/LogicalLayer/publisher.hpp"

using namespace boost::asio;
using namespace ip;
using namespace std::chrono_literals;

LogicalLayer::PublisherData GetPublisherData(std::string publisherName, std::string message)
{
    LogicalLayer::PublisherData publisherData(publisherName, message);
    return publisherData;
}

int main()
{
    io_service io_service;
    
    // Test
    // -------------------------------------------------------------------------

    NetworkLayer::Node broker("broker", io_service);
    NetworkLayer::Node publisher("publisher", io_service);

    broker.Accept(7777);
    publisher.Connect("localhost", 7777);

    LogicalLayer::Broker LLBroker(broker);

    std::vector<std::shared_ptr<NetworkLayer::Node>> nodes;
    std::vector<std::shared_ptr<LogicalLayer::Subscriber>> subscribers;
    for(auto i = 0; i < 10; i++)
    {
        auto node = std::make_shared<NetworkLayer::Node>("subscriber" + std::to_string(i), io_service);
        node->Connect("localhost", 7777);
        auto subscriber = std::make_shared<LogicalLayer::Subscriber>(*node);
        LogicalLayer::SubscriptionT subscription{ { "attrib", "value" } };
        subscriber->AddSubscription(subscription);
        nodes.push_back(node);
        subscribers.push_back(subscriber);
    }

    LogicalLayer::PublisherIdentityT publisherIdentity{ { "attrib", "value" } };
    LogicalLayer::Publisher LLPublisher(publisher, publisherIdentity);
    LLPublisher.StartPublishing(
        std::bind(
            &GetPublisherData,
            publisher.Name(),
            "Message from main"
        ),
        2000 /*milliseconds*/
    );
    
    std::thread t(
        [&io_service]
        {
            io_service.run();
        });
    
    // -------------------------------------------------------------------------
    
    // Receive console commands here.
    // Post them to the above thread.
    enum { commandSize = 1024 };
    char consoleCommand[commandSize];
    
    std::cout << "Insert command: \n";
    auto exit = false;
    
    do
    {
        std::cin.getline(consoleCommand, commandSize);
        std::string command(consoleCommand);
        if (boost::algorithm::starts_with(command, "exit"))
        {
            exit = true;
            io_service.post([&] {
                LLPublisher.StopPublishing();
                for(auto& subscriber : nodes)
                {
                    subscriber->Close();
                }
                broker.Close();
                publisher.Close();
            });
            t.join();
        }

        if (boost::algorithm::starts_with(command, "stop"))
        {
            io_service.post([&LLPublisher] {
                LLPublisher.StopPublishing();
            });
        }

        if (boost::algorithm::starts_with(command, "start"))
        {
            io_service.post([&] {
                LLPublisher.StartPublishing(
                    std::bind(
                        &GetPublisherData,
                        publisher.Name(),
                        "Message from main"
                    ),
                    2000 /*milliseconds*/
                );
            });
        }
    } while (!exit);
    
    return 0;
}
