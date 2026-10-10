#ifndef _MESSAGE_DECODER_HPP_
#define _MESSAGE_DECODER_HPP_

#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/variant.hpp>
#include <optional>
#include <sstream>
#include <string>

#include "allMessages.hpp"

namespace LogicalLayer {

// Any node can send any bytes, so a payload that doesn't decode is dropped
// rather than allowed to throw out of the event loop.
inline std::optional<MessageVariant> decodeMessage(const std::string& payload) {
    try {
        std::stringstream stream(payload);
        boost::archive::binary_iarchive archive(stream);
        MessageVariant message;
        archive >> message;
        return message;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

}  // namespace LogicalLayer

#endif  // _MESSAGE_DECODER_HPP_
