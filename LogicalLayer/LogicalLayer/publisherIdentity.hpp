#ifndef _PUBLISHER_IDENTITY_HPP_
#define _PUBLISHER_IDENTITY_HPP_

#include <vector>
#include <string>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <map>

namespace LogicalLayer
{
    typedef std::map<std::string, std::string> PublisherIdentityT;
    
    class PublisherIdentityMessage
    {
    public:
        PublisherIdentityMessage() {}
        ~PublisherIdentityMessage() {}

        explicit PublisherIdentityMessage(
            std::string _publisher,
            PublisherIdentityT& _publisherIdentity)
        :
            publisher(_publisher),
            publisherIdentity(_publisherIdentity)
        {}

        const std::string& Publisher() const { return publisher; }
        const PublisherIdentityT& GetPublisherIdentity() const { return publisherIdentity; }
        
    private:
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & publisher;
            ar & publisherIdentity;
        }

        friend class boost::serialization::access;

        std::string publisher;
        PublisherIdentityT publisherIdentity;
    };
}

#endif
