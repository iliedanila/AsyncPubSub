#ifndef _SUBSCRIPTION_HPP_
#define _SUBSCRIPTION_HPP_

#include <boost/serialization/access.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/set.hpp>
#include <vector>
#include <set>

namespace LogicalLayer
{
    typedef std::set<std::string> SubscriptionT;

    class SubscriptionMessage
    {
    public:
        enum Action
        {
            eAdd = 1,
            eRemove
        };

        SubscriptionMessage() {}
        ~SubscriptionMessage() {}

        explicit SubscriptionMessage(
            const std::string _subscriber,
            const SubscriptionT& _subscription,
            Action _action)
        :
            subscriberName(_subscriber),
            subscription(_subscription),
            action(_action)
        {}

        const std::string& SubscriberName() const { return subscriberName; }
        const SubscriptionT& Subscription() const { return subscription; }
        const Action& GetAction() const { return action; }

    private:
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & subscriberName;
            ar & subscription;
            ar & action;
        }

        friend class boost::serialization::access;

        std::string subscriberName;
        SubscriptionT subscription;
        Action action;
    };
}

#endif
