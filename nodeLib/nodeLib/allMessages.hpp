#ifndef allMessages_h
#define allMessages_h

#include <boost/variant.hpp>
#include "routingMessage.hpp"
#include "dataMessage.hpp"
#include "dataMessageAck.hpp"

namespace NetworkLayer
{

typedef boost::variant< RoutingMessage, DataMessage, DataMessageAck > MessageVariant;
    
}

#endif /* allMessages_h */
