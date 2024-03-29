#ifndef ALL_MESSAGES_HPP
#define ALL_MESSAGES_HPP

#include "addRemoveSubscription.hpp"
#include "brokerIdentity.hpp"
#include "publisherIdentity.hpp"
#include "startPublish.hpp"
#include "stopPublish.hpp"
#include "subscriptionData.hpp"

#include <boost/variant/variant.hpp>

namespace LogicalLayer {

typedef boost::variant<BrokerIdentity, AddRemoveSubscriptionMessage,
                       PublisherIdentityMessage, StartPublish, StopPublish,
                       SubscriptionData>
    MessageVariant;
}

#endif
