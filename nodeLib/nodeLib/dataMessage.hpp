#ifndef dataMessage_h
#define dataMessage_h

#include <string>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>

class DataMessage
{
public:
    DataMessage() {}
    ~DataMessage() {}
    
    DataMessage(std::string source, std::string destination, std::string msg)
    :
        sourceNodeName(std::move(source)),
        destinationNodeName(std::move(destination)),
        message(std::move(msg))
    {}
    
private:
    friend class boost::serialization::access;
    friend class NetworkNode;
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & sourceNodeName;
        ar & destinationNodeName;
        ar & message;
    }

private:
    std::string sourceNodeName;
    std::string destinationNodeName;
    std::string message;
};


#endif /* dataMessage_h */
