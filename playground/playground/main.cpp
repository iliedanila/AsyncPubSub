#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <thread>
#include <chrono>

#include "../../nodeLib/nodeLib/node.hpp"
#include "../../LogicalLayer/LogicalLayer/node.hpp"
#include "../../LogicalLayer/LogicalLayer/broker.hpp"
#include "../../LogicalLayer/LogicalLayer/subscriber.hpp"
#include "../../LogicalLayer/LogicalLayer/publisher.hpp"

using namespace boost::asio;
using namespace ip;
using namespace std::chrono_literals;


int main(int argc, const char * argv[])
{
    boost::asio::io_service io_service;
    
    // Test
    // -------------------------------------------------------------------------

    NetworkLayer::Node subscriber("subscriber", io_service);
    NetworkLayer::Node broker("broker", io_service);
    NetworkLayer::Node publisher("publisher", io_service);

    broker.Accept(7777);
    subscriber.Connect("localhost", 7777);
    publisher.Connect("localhost", 7777);

    LogicalLayer::Broker hl_broker(broker);
    LogicalLayer::Subscriber hl_subscriber(subscriber);

    auto attribPair = std::make_pair("attrib", "value");
    LogicalLayer::SubscriptionT subscription;
    subscription.push_back(attribPair);
    hl_subscriber.AddSubscription(subscription);

    LogicalLayer::PublisherIdentityT publisherIdentity;
    publisherIdentity.push_back(attribPair);
    LogicalLayer::Publisher hl_publisher(publisher, publisherIdentity);
    
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
            subscriber.Close();
            broker.Close();
            publisher.Close();
            t.join();
        }
    } while (!exit);
    
    return 0;
}
