#ifndef ALL_MESSAGES_HPP
#define ALL_MESSAGES_HPP
#include <boost/variant/variant.hpp>
#include "logMessage.hpp"

typedef boost::variant<LogMessage> MessageVariant;

#endif
