#ifndef _PUBLISHER_IDENTITY_HPP_
#define _PUBLISHER_IDENTITY_HPP_

#include <vector>
#include <string>
#include <boost/serialization/access.hpp>
#include <boost/serialization/set.hpp>
#include <set>

#include "common.hpp"

namespace LogicalLayer
{

class PublisherIdentityMessage
{
public:
    PublisherIdentityMessage() {}
    ~PublisherIdentityMessage() {}

    explicit PublisherIdentityMessage(
        std::string aPublisherName,
        PublisherIdentityT& aPublisherIdentity)
    :
        publisherName(aPublisherName),
        publisherIdentity(aPublisherIdentity)
    {}

    const std::string& getPublisherName() const { return publisherName; }
    const PublisherIdentityT& getPublisherIdentity() const { return publisherIdentity; }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & publisherName;
        ar & publisherIdentity;
    }

    friend class boost::serialization::access;

    std::string publisherName;
    PublisherIdentityT publisherIdentity;
};

}

#endif
