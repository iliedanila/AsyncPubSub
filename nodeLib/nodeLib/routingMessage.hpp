#ifndef routingMessage_hpp
#define routingMessage_hpp

#include <vector>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>

namespace NetworkLayer
{

class RoutingMessage
{
public:
    RoutingMessage() {}
    ~RoutingMessage() {}
    
    auto NodeDistances() { return nodeDistances; }
    auto FailedNodes() { return failedNodes; }
    
    void AddNodeDistance(std::pair<std::string, std::size_t> _nodeDistance)
    {
        nodeDistances.push_back(_nodeDistance);
    }
    
    void AddFailedNode(std::string node)
    {
        failedNodes.push_back(node);
    }
    
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & nodeDistances;
        ar & failedNodes;
    }
    
private:
    std::vector<std::pair<std::string, std::size_t>> nodeDistances;
    std::vector<std::string> failedNodes;
};
    
}

#endif /* routingMessage_hpp */
