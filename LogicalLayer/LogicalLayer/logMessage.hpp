#ifndef LOG_MESSAGE_HPP
#define LOG_MESSAGE_HPP

#include <boost/serialization/access.hpp>

namespace LogicalLayer
{

class LogMessage
{
public:
	LogMessage() {}
	~LogMessage() {}

	explicit LogMessage(std::string _log)
	:
		log(std::move(_log))
	{}

	std::string& Log() { return log; }

private:
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & log;
	}

private:
	friend class boost::serialization::access;
	std::string log;
};
    
}

#endif