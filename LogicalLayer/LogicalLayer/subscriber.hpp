#ifndef _LOGICAL_LAYER_SUBSCRIBER_HPP_
#define _LOGICAL_LAYER_SUBSCRIBER_HPP_
#include "messageVisitor.hpp"
#include <vector>
#include "brokerIdentity.hpp"
#include "subscription.hpp"
#include "../../nodeLib/nodeLib/sendError.hpp"

namespace NetworkLayer {
	class DataMessage;
	class Node;
}

namespace LogicalLayer
{
	class Subscriber
	{
	public:
		explicit Subscriber(NetworkLayer::Node& node);
		~Subscriber();

		void AddSubscription(SubscriptionT& subscription);

	private:
		friend struct MessageVisitor<Subscriber>;

		void HandleIncomingMessage(NetworkLayer::DataMessage& message);

		void SendSubscription(
			SubscriptionT& subscription, 
			const std::string& brokerName) const;
		void SendNewSubscription(SubscriptionT& subscription);
		void SendAllSubscriptions(const std::string& brokerName);
		void HandleNewBroker(BrokerIdentity& message);

		void HandleBrokerSubscriptionAck(
			const std::string nodeName,
			NetworkLayer::SendError error) const;

		template<typename MessageT>
		void HandleMessage(MessageT& message);

		NetworkLayer::Node& node;
		std::vector<std::string> brokers;
		std::vector<SubscriptionT> subscriptions;
	};
}

#endif
