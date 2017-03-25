#ifndef _LOGICAL_LAYER_PUBLISHER_HPP_
#define _LOGICAL_LAYER_PUBLISHER_HPP_
#include "messageVisitor.hpp"
#include "publisherIdentity.hpp"
#include "../../nodeLib/nodeLib/sendError.hpp"

namespace NetworkLayer {
    class DataMessage;
    class Node;
}

namespace LogicalLayer
{
    class Publisher
    {
    public:
        explicit Publisher(NetworkLayer::Node& node);
        ~Publisher();

    private:
        friend struct MessageVisitor<Publisher>;
        void HandleIncomingMessage(NetworkLayer::DataMessage& message);

        void HandleBrokerAck(
            const std::string nodeName,
            NetworkLayer::SendError error) const;

        template<typename MessageT>
        void HandleMessage(MessageT& message);

        PublisherIdentityT identity;
        NetworkLayer::Node& node;
    };
}

#endif
