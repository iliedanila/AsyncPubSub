#include <iostream>
#include <boost/asio.hpp>
#include <functional>
#include <thread>
#include <chrono>

#include "../../nodeLib/nodeLib/node.hpp"

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std::chrono_literals;


int main(int argc, const char * argv[])
{
    boost::asio::io_service io_service;
    
    NetworkNode node1("node1", io_service);
    NetworkNode node2("node2", io_service);
    NetworkNode node3("node3", io_service);

    node1.Accept(7001);
    node2.Connect("localhost", 7001);
    node3.Connect("localhost", 7001);

    std::thread t([&io_service]
                  {
                      io_service.run();
                  });
    
    std::this_thread::sleep_for(1s);
    io_service.post([&node3]{
        auto nodesAccessible = node3.GetAccessibleNodes();
        
        std::cout << "Node3 has access to: ";
        for ( auto node : nodesAccessible )
            std::cout << node << " ";
        std::cout << "\n";
        
    });
    
    // Receive console commands here.
    // Post them to the above thread.
    enum { commandSize = 1024 };
    char consoleCommand[commandSize];
    while(std::cin.getline(consoleCommand, commandSize))
    {
        std::cout << consoleCommand << "\n";
    }
    
    return 0;
}