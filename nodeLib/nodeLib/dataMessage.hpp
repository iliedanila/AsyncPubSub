#ifndef dataMessage_h
#define dataMessage_h

#include <string>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>

namespace MeshNetwork
{

class DataMessage
{
public:
    DataMessage() {}
    ~DataMessage() {}
    
    DataMessage(std::string source, std::string destination, std::string buff)
    :
        sourceNodeName(std::move(source)),
        destinationNodeName(std::move(destination)),
        buffer(std::move(buff))
    {}
    
private:
    friend class boost::serialization::access;
    friend class Node;
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & sourceNodeName;
        ar & destinationNodeName;
        ar & buffer;
    }

private:
    std::string sourceNodeName;
    std::string destinationNodeName;
    std::string buffer;
};

}

#endif /* dataMessage_h */
