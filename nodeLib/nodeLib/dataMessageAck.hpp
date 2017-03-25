#ifndef dataMessageAck_hpp
#define dataMessageAck_hpp

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "sendError.hpp"

namespace NetworkLayer
{
    
class DataMessageAck
{
public:
    DataMessageAck()
    :
        error()
    {}

    ~DataMessageAck() {}
    
    DataMessageAck(std::string source, std::string destination, SendError _error)
    :
        sourceNodeName(std::move(source)),
        destinationNodeName(std::move(destination)),
        error(_error)
    {}
    
private:
    friend class boost::serialization::access;
    friend class Node;
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & sourceNodeName;
        ar & destinationNodeName;
        ar & error;
    }
    
private:
    std::string sourceNodeName;
    std::string destinationNodeName;
    SendError error;
};
    
}

#endif /* dataMessageAck_hpp */
