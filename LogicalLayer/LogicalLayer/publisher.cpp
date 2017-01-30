#include "publisher.hpp"
#include <functional>
#include "../../nodeLib/nodeLib/node.hpp"
#include "messageVisitor.hpp"
#include "allMessages.hpp"
#include <boost/serialization/variant.hpp>

namespace LogicalLayer
{
	Publisher::Publisher(NetworkLayer::Node& _node)
	:
		node(_node)
	{
		node.AcceptMessages(
			std::bind(
				&Publisher::HandleIncomingMessage,
				this,
				std::placeholders::_1));
	}

	Publisher::~Publisher()
	{
	}

	void Publisher::HandleIncomingMessage(NetworkLayer::DataMessage& message)
	{
		std::stringstream ss(std::move(message.Buffer()));
		boost::archive::binary_iarchive iarchive(ss);

		MessageVariant messageV;
		iarchive >> messageV;

		boost::apply_visitor(MessageVisitor<Publisher>(*this), messageV);
	}

	template <>
	void Publisher::HandleMessage(LogMessage& message)
	{
	}

	template <>
	void Publisher::HandleMessage(BrokerIdentity& message)
	{
		// Send publisher attributes.
	}

	template <>
	void Publisher::HandleMessage(Subscription& message)
	{
	}
}
