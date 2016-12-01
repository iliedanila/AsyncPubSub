#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <thread>
#include <chrono>


#include "../../nodeLib/nodeLib/node.hpp"

using namespace boost::asio;
using namespace ip;
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


	// Test 1
	// -------------------------------------------------------------------------
    
	node3.AcceptMessages([](std::string sourceNode, std::string buffer) {
		std::cout << "Message from " << sourceNode << ": " << buffer << "\n";
	});

	while( !node2.IsNodeAccessible("node3") )
	{
		std::this_thread::sleep_for(20ms);
	}
    
    node2.SndMessage("node3", "hello buddy", [](MeshNetwork::SendError error){
        if (error == MeshNetwork::eSuccess)
        {
            std::cout << "Message successfully sent.\n";
        }
    });

	// Test 2
	// -------------------------------------------------------------------------

	node1.Close();
    
    
    // Receive console commands here.
    // Post them to the above thread.
    enum { commandSize = 1024 };
    char consoleCommand[commandSize];
    
    std::cout << "Insert command: \n";
    bool exit = false;
    
    do
    {
        std::cin.getline(consoleCommand, commandSize);
        std::string command(consoleCommand);
        if (boost::algorithm::starts_with(command, "exit"))
        {
            exit = true;
            node1.Close();
            node2.Close();
            node3.Close();

            io_service.stop();
            t.join();
        }
    } while (!exit);
    
    return 0;
}
