#ifndef _LOGICAL_LAYER_PUBLISHER_HPP_
#define _LOGICAL_LAYER_PUBLISHER_HPP_

#include "messageVisitor.hpp"
#include "publisherIdentity.hpp"
#include "../../nodeLib/nodeLib/sendError.hpp"
#include "publisherData.hpp"

#include <boost/date_time/posix_time/posix_time_config.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <set>
#include <functional>

namespace NetworkLayer {
    class DataMessage;
    class Node;
}

namespace LogicalLayer
{
    typedef std::function<PublisherData()> PublishFunctionT;

    class Publisher
    {
    public:
        explicit Publisher(
            NetworkLayer::Node& node,
            PublisherIdentityT& publisherIdentity);
        ~Publisher();

        void StartPublishing(PublishFunctionT _publishFunction, uint32_t millisecondsRepeat);
        void StopPublishing();
        const std::string& Name() const;

    private:
        friend struct MessageVisitor<Publisher>;
        void HandleIncomingMessage(NetworkLayer::DataMessage& message);

        void HandleNewNodeStatus(std::string nodeName, bool isAlive);

        void DefaultHandleAck(
            const std::string nodeName,
            NetworkLayer::SendError error) const;

        template<typename MessageT>
        void HandleMessage(MessageT& message);

        void OnTimerExpired(boost::system::error_code error);

        PublisherIdentityT identity;
        NetworkLayer::Node& node;
        PublishFunctionT publishFunction;
        boost::asio::deadline_timer publishTimer;
        uint32_t millisecondsRepeatPublish;
        std::map<std::string, SubscriptionT> subscribers;
    };
}

#endif
