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
    
    
    MeshNetwork::Node node1("node1", io_service);
    MeshNetwork::Node node2("node2", io_service);
    MeshNetwork::Node node3("node3", io_service);

    node1.Accept(7001);
    node2.Connect("localhost", 7001);
    node3.Connect("localhost", 7001);


    std::thread t([&io_service]
                  {
                      io_service.run();
                  });
    
    std::this_thread::sleep_for(1s);
    
    node3.AcceptMessages([](std::string sourceNode, std::string buffer){
        std::cout << "Message from " << sourceNode << ": " << buffer << "\n";
    });
    
    node2.SendMessage("node3", "hello buddy", [](MeshNetwork::SendError error){
        if (error == MeshNetwork::eSuccess)
        {
            std::cout << "Message successfully sent.\n";
        }
    });
    
    
    // Receive console commands here.
    // Post them to the above thread.
    enum { commandSize = 1024 };
    char consoleCommand[commandSize];
    
    std::cout << "Insert command: \n";
    
    while(std::cin.getline(consoleCommand, commandSize))
    {
        std::cout << consoleCommand << "\n";
    }
    
    return 0;
}
