#ifndef _SUBSCRIPTION_HPP_
#define _SUBSCRIPTION_HPP_

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <vector>

namespace LogicalLayer
{
	typedef std::vector<std::pair<std::string, std::string>> SubscriptionT;

	class Subscription
	{
	public:
		Subscription() {}
		~Subscription() {}

		explicit Subscription(SubscriptionT& _subscription)
		:
			subscription(_subscription)
		{}

		const SubscriptionT& GetSubscription() const { return subscription; }

	private:
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & subscription;
		}

		friend class boost::serialization::access;
		SubscriptionT subscription;
	};
}

#endif
