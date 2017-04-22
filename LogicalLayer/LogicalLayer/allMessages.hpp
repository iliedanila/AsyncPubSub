#ifndef ALL_MESSAGES_HPP
#define ALL_MESSAGES_HPP

#include "brokerIdentity.hpp"
#include "subscription.hpp"
#include "publisherIdentity.hpp"
#include "startPublish.hpp"
#include "publisherData.hpp"

#include <boost/variant/variant.hpp>

namespace LogicalLayer
{

typedef boost::variant< 
    BrokerIdentity, 
    SubscriptionMessage,
    PublisherIdentityMessage,
    StartPublish,
    PublisherData> MessageVariant;
}

#endif
