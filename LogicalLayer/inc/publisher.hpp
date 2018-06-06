#ifndef _LOGICAL_LAYER_PUBLISHER_HPP_
#define _LOGICAL_LAYER_PUBLISHER_HPP_

#include "messageVisitor.hpp"
#include "sendError.hpp"
#include "common.hpp"

//#include <boost/date_time/posix_time/posix_time_config.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <set>
#include <functional>

namespace NetworkLayer {
    class DataMessage;
    class Node;
}

namespace LogicalLayer
{

class Publisher
{
public:
    explicit Publisher(
        NetworkLayer::Node& node,
        PublisherIdentityT& publisherIdentity);
    ~Publisher();

    void startPublishing(PublishFunctionT _publishFunction, uint32_t millisecondsRepeat);
    void stopPublishing();
    const std::string& getName() const;

private:
    friend struct MessageVisitor<Publisher>;
    void handleIncomingMessage(NetworkLayer::DataMessage &message);

    void handleNewNodeStatus(std::string nodeName, bool isAlive);

    void defaultHandleAck(
            const std::string nodeName,
            NetworkLayer::SendError error) const;

    template<typename MessageT>
    void handleMessage(MessageT& message);

    void collectPublishData (boost::system::error_code error);

    PublisherIdentityT identity;
    NetworkLayer::Node& node;
    PublishFunctionT getPublisherData;
    boost::asio::deadline_timer publishTimer;
    uint32_t millisecondsRepeatPublish;
    std::map<std::string, std::set<SubscriptionT>> subscribers;
    std::map<std::string, std::string> activeDataTypes;
};

}

#endif
