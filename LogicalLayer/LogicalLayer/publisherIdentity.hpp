#ifndef _PUBLISHER_IDENTITY_HPP_
#define _PUBLISHER_IDENTITY_HPP_

#include <vector>
#include <string>
#include <boost/serialization/access.hpp>

namespace LogicalLayer
{
    typedef std::vector<std::pair<std::string, std::string>> PublisherIdentityT;
    
    class PublisherIdentity
    {
    public:
        PublisherIdentity() {}
        ~PublisherIdentity() {}

        explicit PublisherIdentity(
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
