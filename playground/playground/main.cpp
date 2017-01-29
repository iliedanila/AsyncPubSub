#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <thread>
#include <chrono>


#include "../../nodeLib/nodeLib/node.hpp"
#include "../../LogicalLayer/LogicalLayer/node.hpp"

using namespace boost::asio;
using namespace ip;
using namespace std::chrono_literals;


int main(int argc, const char * argv[])
{
    boost::asio::io_service io_service;
    
	// Test
	// -------------------------------------------------------------------------

	NetworkLayer::Node logger("logger", io_service);
	NetworkLayer::Node broker("broker", io_service);
	NetworkLayer::Node sender("sender", io_service);

	broker.Accept(7777);
	logger.Connect("localhost", 7777);
	sender.Connect("localhost", 7777);

	LogicalLayer::Node hl_logger(logger);
    LogicalLayer::Node hl_sender(sender);
    
    sender.RegisterNodeAccessibility([&hl_sender](std::string nodeName, bool isAccessible)
    {
        if(nodeName == "logger" && isAccessible == true)
        {
            hl_sender.SndMessage("logger", LogicalLayer::LogMessage("Log from main."));
        }
    });
    
    std::thread t([&io_service]
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
            logger.Close();
            broker.Close();
            sender.Close();
            t.join();
        }
    } while (!exit);
    
    return 0;
}
