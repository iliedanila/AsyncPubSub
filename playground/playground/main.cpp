#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <thread>
#include <chrono>


#include "../../nodeLib/nodeLib/node.hpp"
#include "../../LogicalLayer/node.hpp"

using namespace boost::asio;
using namespace ip;
using namespace std::chrono_literals;


int main(int argc, const char * argv[])
{
    boost::asio::io_service io_service;
    
    NetworkLayer::Node node1("node1", io_service);
    NetworkLayer::Node node2("node2", io_service);
    NetworkLayer::Node node3("node3", io_service);

    node1.Accept(7001);
    node2.Connect("localhost", 7001);
    node3.Connect("localhost", 7001);


    std::thread t([&io_service]
                  {
                      io_service.run();
                  });


	// Test 1
	// -------------------------------------------------------------------------
    
    node3.AcceptMessages([](NetworkLayer::DataMessage message) {
		std::cout << "Message from " << message.Source() << ": " << message.Buffer() << "\n";
	});

	while( !node2.IsNodeAccessible("node3") )
	{
		std::this_thread::sleep_for(20ms);
	}
    
    node2.SndMessage("node3", "hello buddy", [](NetworkLayer::SendError error){
        if (error == NetworkLayer::eSuccess)
        {
            std::cout << "Message successfully sent.\n";
        }
    });

	// Test 2
	// -------------------------------------------------------------------------

	NetworkLayer::Node logger("logger", io_service);
	NetworkLayer::Node broker("broker", io_service);
	NetworkLayer::Node sender("sender", io_service);

	broker.Accept(7777);
	logger.Connect("localhost", 7777);
	sender.Connect("localhost", 7777);

	LogicalLayer::Node hl_logger(logger);
    LogicalLayer::Node hl_sender(sender);

	while (!sender.IsNodeAccessible("logger"))
	{
		std::this_thread::sleep_for(20ms);
	}

    hl_sender.SendMessage("logger", LogicalLayer::LogMessage("Log from main."));
    
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
