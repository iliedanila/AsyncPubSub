#ifndef _LOGICAL_LAYER_PUBLISHER_HPP_
#define _LOGICAL_LAYER_PUBLISHER_HPP_

#include "common.hpp"
#include "sendError.hpp"

#include <boost/asio/steady_timer.hpp>
#include <functional>
#include <map>
#include <set>

namespace NetworkLayer {
class DataMessage;
class Node;
}  // namespace NetworkLayer

namespace LogicalLayer {

class BrokerIdentity;
class AddRemoveSubscriptionMessage;
class PublisherIdentityMessage;
class StartPublish;
class StopPublish;
class SubscriptionData;

class Publisher {
   public:
    explicit Publisher(NetworkLayer::Node& node,
                       PublisherIdentityT publisherIdentity);
    ~Publisher();

    void startPublishing(PublishFunctionT _publishFunction,
                         uint32_t millisecondsRepeat);
    void stopPublishing();
    const std::string& getName() const;

   private:
    void handleIncomingMessage(NetworkLayer::DataMessage& message);

    void handleNewNodeStatus(std::string nodeName, bool isAlive);

    void defaultHandleAck(const std::string nodeName,
                          NetworkLayer::SendError error) const;

    // One handler per MessageVariant alternative. handleIncomingMessage
    // dispatches with std::visit, so a missing one is a compile error.
    void handleMessage(BrokerIdentity& message);
    void handleMessage(AddRemoveSubscriptionMessage& message);
    void handleMessage(PublisherIdentityMessage& message);
    void handleMessage(StartPublish& message);
    void handleMessage(StopPublish& message);
    void handleMessage(SubscriptionData& message);

    void collectPublishData(boost::system::error_code error);

    void rebuildActiveDataTypes();

    PublisherIdentityT identity;
    NetworkLayer::Node& node;
    PublishFunctionT getPublisherData;
    boost::asio::steady_timer publishTimer;
    uint32_t millisecondsRepeatPublish;
    std::map<std::string, std::set<SubscriptionT>> subscribers;
    std::map<std::string, std::string> activeDataTypes;
};

}  // namespace LogicalLayer

#endif
