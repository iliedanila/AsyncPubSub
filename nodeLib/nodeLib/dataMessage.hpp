#ifndef dataMessage_h
#define dataMessage_h

#include <boost/archive/binary_oarchive.hpp>

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
