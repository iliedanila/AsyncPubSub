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
#include "subscriptionData.hpp"
#include "publisherData.hpp"

using namespace boost::asio;
using namespace ip;
using namespace std::chrono_literals;

void populateData(LogicalLayer::PublisherData& publisherData)
{
    for(auto& dataTypes : publisherData.getData ())
    {
        dataTypes.second = "Dummy value here.";
    }
}

int main()
{
    io_service ioservice;
    
    // Test
    // -------------------------------------------------------------------------
    NetworkLayer::Node loggerNode("logger", ioservice, true);
    loggerNode.accept(8888);

    NetworkLayer::Node brokerNode("broker", ioservice);
    LogicalLayer::Broker broker(brokerNode);
    brokerNode.accept(7777);
    brokerNode.connect("localhost", 8888, false);
    
    std::vector<std::shared_ptr<NetworkLayer::Node>> nodes;
    std::vector<std::shared_ptr<LogicalLayer::Subscriber>> subscribers;
    std::vector<std::shared_ptr<LogicalLayer::Publisher>> publishers;
    
    for(auto i = 0; i < 1; i++)
    {
        auto node = std::make_shared<NetworkLayer::Node>("subscriber" + std::to_string(i), ioservice);
        node->connect("localhost", 7777, false);
        auto subscriber = std::make_shared<LogicalLayer::Subscriber>(*node);
        std::set<std::string> subscription = { "MyCustomType1", "MyCustomType2" };

        subscriber->addSubscription(
                subscription,
                [node]
                (LogicalLayer::SubscriptionData &data)
                {
                    std::string receivedData;
                    for (auto& typeData : data.getData())
                    {
                        receivedData += typeData.first;
                        receivedData += ": ";
                        receivedData += typeData.second;
                        receivedData += "\t";
                    }
                    node->log("Publisher data: " + receivedData);
                }
        );
        nodes.push_back(node);
        subscribers.push_back(subscriber);
    }
    
    for(auto i = 0; i < 1; i++)
    {
        auto node = std::make_shared<NetworkLayer::Node>("publisher" + std::to_string(i), ioservice);
        node->connect("localhost", 7777, false);
        std::set<std::string> publisherIdentity = { "MyCustomType1", "MyCustomType2", "MyCustomType3" };
        auto publisher = std::make_shared<LogicalLayer::Publisher>(*node, publisherIdentity);

        publisher->startPublishing(std::bind(&populateData, std::placeholders::_1), 2000);

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
                    publisher->stopPublishing();
                }
                for(auto& node : nodes)
                {
                    node->close();
                }
                brokerNode.close();
                loggerNode.close();
            });
            t.join();
        }

        if (boost::algorithm::starts_with(command, "stop"))
        {
            ioservice.post([&publishers] {
                for(auto& publisher : publishers)
                {
                    publisher->stopPublishing();
                }
            });
        }

        if (boost::algorithm::starts_with(command, "start"))
        {
            ioservice.post([&] {
                for(auto& publisher : publishers)
                {
                    publisher->startPublishing(std::bind(&populateData, std::placeholders::_1), 2000);
                }
            });
        }

        if (boost::algorithm::starts_with(command, "remove"))
        {
            ioservice.post ([&]{
               for (auto& subscriber : subscribers)
               {
                   std::set<std::string> subscription = { "MyCustomType1", "MyCustomType2" };
                   subscriber->removeSubscription (subscription);
               }
            });
        }

        if (boost::algorithm::starts_with(command, "add"))
        {
            ioservice.post ([&]{
                for (auto& subscriber : subscribers)
                {
                    std::set<std::string> subscription = { "MyCustomType1", "MyCustomType2" };
                    subscriber->addSubscription (
                        subscription,
                        [](LogicalLayer::SubscriptionData &data){
                            std::string receivedData;
                            for (auto& typeData : data.getData())
                            {
                                receivedData += typeData.first;
                                receivedData += ": ";
                                receivedData += typeData.second;
                                receivedData += "\t";
                            }
                            std::cout << "Publisher data: " + receivedData << '\n';
                        });
                }
            });
        }
    } while (!exit);
    
    return 0;
}
