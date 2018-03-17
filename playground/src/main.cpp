#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <thread>
#include <chrono>

#include "node.hpp"
#include "broker.hpp"
#include "subscriber.hpp"
#include "publisher.hpp"

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
    io_service ioservice;
    
    // Test
    // -------------------------------------------------------------------------
    NetworkLayer::Node loggerNode("logger", ioservice, true);
    loggerNode.Accept(8888);

    NetworkLayer::Node brokerNode("broker", ioservice);
    LogicalLayer::Broker broker(brokerNode);
    brokerNode.Accept(7777);
    brokerNode.Connect("localhost", 8888, false);
    
    std::vector<std::shared_ptr<NetworkLayer::Node>> nodes;
    std::vector<std::shared_ptr<LogicalLayer::Subscriber>> subscribers;
    std::vector<std::shared_ptr<LogicalLayer::Publisher>> publishers;
    
    for(auto i = 0; i < 1; i++)
    {
        auto node = std::make_shared<NetworkLayer::Node>("subscriber" + std::to_string(i), ioservice);
        node->Connect("localhost", 7777, false);
        auto subscriber = std::make_shared<LogicalLayer::Subscriber>(*node);
        LogicalLayer::SubscriptionT subscription{ { "data" } };
        subscriber->AddSubscription(
            subscription,
            [node]
            (LogicalLayer::PublisherData& data)
            {
                node->Log("Publisher data: " + data.Data());
            }
        );
        nodes.push_back(node);
        subscribers.push_back(subscriber);
    }
    
    for(auto i = 0; i < 1; i++)
    {
        auto node = std::make_shared<NetworkLayer::Node>("publisher" + std::to_string(i), ioservice);
        node->Connect("localhost", 7777, false);
        LogicalLayer::PublisherIdentityT publisherIdentity{ { "data" } };
        auto publisher = std::make_shared<LogicalLayer::Publisher>(*node, publisherIdentity);
        publisher->StartPublishing(std::bind(&GetPublisherData, node->Name(), "Message from main()"), 2000);
        nodes.push_back(node);
        publishers.push_back(publisher);
    }

    std::thread t(
        [&ioservice]
        {
            ioservice.run();
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
            ioservice.post([&] {
                for(auto& publisher : publishers)
                {
                    publisher->StopPublishing();
                }
                for(auto& node : nodes)
                {
                    node->Close();
                }
                brokerNode.Close();
                loggerNode.Close();
            });
            t.join();
        }

        if (boost::algorithm::starts_with(command, "stop"))
        {
            ioservice.post([&publishers] {
                for(auto& publisher : publishers)
                {
                    publisher->StopPublishing();
                }
            });
        }

        if (boost::algorithm::starts_with(command, "start"))
        {
            ioservice.post([&] {
                for(auto& publisher : publishers)
                {
                    publisher->StartPublishing(std::bind(&GetPublisherData, publisher->Name(), "Message from main"), 2000);
                }
            });
        }
    } while (!exit);
    
    return 0;
}
