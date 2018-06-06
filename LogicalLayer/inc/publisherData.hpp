#ifndef _PUBLISHER_DATA_HPP_
#define _PUBLISHER_DATA_HPP_

#include <string>
#include <map>

#include "publisherIdentity.hpp"

namespace LogicalLayer
{

class PublisherData
{
public:
    explicit PublisherData(const std::map<std::string, std::string>& activeSubscriptions)
    :
        typeDataMap(activeSubscriptions)
    {
    }

    void addData(const std::string& dataType, const std::string& data)
    {
        typeDataMap[dataType] = data;
    }

    std::string getData(const std::string& aDataType)
    {
        return typeDataMap[aDataType];
    }

    std::map<std::string, std::string>& getData()
    {
        return typeDataMap;
    };

private:
    std::map<std::string, std::string> typeDataMap;
};

}

#endif //_PUBLISHER_DATA_HPP_
