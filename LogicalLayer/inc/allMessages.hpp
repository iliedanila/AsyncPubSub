#ifndef ALL_MESSAGES_HPP
#define ALL_MESSAGES_HPP

#include "addRemoveSubscription.hpp"
#include "brokerIdentity.hpp"
#include "publisherIdentity.hpp"
#include "startPublish.hpp"
#include "stopPublish.hpp"
#include "subscriptionData.hpp"

// Writes a std::variant as an int index and the value, as boost::variant was
// written. Not boost/serialization/std_variant.hpp: its index is a size_t,
// which would change the wire format.
#include <boost/serialization/variant.hpp>
#include <variant>

namespace LogicalLayer {

// A payload carries the index of its alternative, so new alternatives go at
// the end; reordering or inserting breaks running nodes.
using MessageVariant =
    std::variant<BrokerIdentity, AddRemoveSubscriptionMessage,
                 PublisherIdentityMessage, StartPublish, StopPublish,
                 SubscriptionData>;
}  // namespace LogicalLayer

#endif
