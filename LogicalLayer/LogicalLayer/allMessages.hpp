#ifndef ALL_MESSAGES_HPP
#define ALL_MESSAGES_HPP

#include "logMessage.hpp"
#include "brokerIdentity.hpp"
#include "subscription.hpp"
#include "publisherIdentity.hpp"
#include <boost/variant/variant.hpp>
#include "StartPublish.hpp"

namespace LogicalLayer
{

typedef boost::variant<
    LogMessage, 
    BrokerIdentity, 
    SubscriptionMessage,
    PublisherIdentityMessage,
    StartPublish> MessageVariant;
    
}

#endif
