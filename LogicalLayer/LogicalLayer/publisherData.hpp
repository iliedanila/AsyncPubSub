#ifndef _PUBLISHER_DATA_HPP_
#define _PUBLISHER_DATA_HPP_
#include <string>
#include <boost/serialization/access.hpp>

namespace LogicalLayer
{
    class PublisherData
    {
    public:
        PublisherData() {}
        ~PublisherData() {}

        explicit PublisherData(const std::string& _publisherName, const std::string& _data)
        :
            publisherName(_publisherName),
            data(_data)
        {}

        const std::string& PublisherName() const { return publisherName; }
        const std::string& Data() const { return data; }

    private:
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & publisherName;
            ar & data;
        }

        friend class boost::serialization::access;

        std::string publisherName;
        std::string data;
    };
}

#endif
