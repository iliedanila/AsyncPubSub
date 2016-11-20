//
//  allMessages.h
//  node_lib
//
//  Created by Ilie Danila on 22/10/2016.
//  Copyright © 2016 Ilie Danila. All rights reserved.
//

#ifndef allMessages_h
#define allMessages_h

#include <boost/variant.hpp>
#include "routingMessage.hpp"
#include "dataMessage.hpp"

typedef boost::variant< RoutingMessage, DataMessage > MessageVariant;

#endif /* allMessages_h */
