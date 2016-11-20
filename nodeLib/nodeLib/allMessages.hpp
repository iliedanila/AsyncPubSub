#ifndef allMessages_h
#define allMessages_h

#include <boost/variant.hpp>
#include "routingMessage.hpp"
#include "dataMessage.hpp"

typedef boost::variant< RoutingMessage, DataMessage > MessageVariant;

#endif /* allMessages_h */
