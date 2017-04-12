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

    NetworkLayer::Node node("broker", io_service);
    LogicalLayer::Broker broker(node);
    node.Accept(7777);
    
    std::vector<std::shared_ptr<NetworkLayer::Node>> nodes;
    std::vector<std::shared_ptr<LogicalLayer::Subscriber>> subscribers;
    std::vector<std::shared_ptr<LogicalLayer::Publisher>> publishers;
    
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
    
    for(auto i = 0; i < 10; i++)
    {
        auto node = std::make_shared<NetworkLayer::Node>("publisher" + std::to_string(i), io_service);
        node->Connect("localhost", 7777);
        LogicalLayer::PublisherIdentityT publisherIdentity{ { "attrib", "value" } };
        auto publisher = std::make_shared<LogicalLayer::Publisher>(*node, publisherIdentity);
        publisher->StartPublishing(std::bind(&GetPublisherData, node->Name(), "Message from main()"), 2000);
        nodes.push_back(node);
        publishers.push_back(publisher);
    }

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
                for(auto& publisher : publishers)
                {
                    publisher->StopPublishing();
                }
                for(auto& node : nodes)
                {
                    node->Close();
                }
                node.Close();
            });
            t.join();
        }

        if (boost::algorithm::starts_with(command, "stop"))
        {
            io_service.post([&publishers] {
                for(auto& publisher : publishers)
                {
                    publisher->StopPublishing();
                }
            });
        }

        if (boost::algorithm::starts_with(command, "start"))
        {
            io_service.post([&] {
                for(auto& publisher : publishers)
                {
                    publisher->StartPublishing(std::bind(&GetPublisherData, publisher->Name(), "Message from main"), 2000);
                }
            });
        }
    } while (!exit);
    
    return 0;
}
