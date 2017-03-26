#ifndef _SUBSCRIPTION_HPP_
#define _SUBSCRIPTION_HPP_

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <vector>

namespace LogicalLayer
{
    typedef std::vector<std::pair<std::string, std::string>> SubscriptionT;

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
            subscriber(_subscriber),
            subscription(_subscription),
            action(_action)
        {}

        const std::string& Subscriber() const { return subscriber; }
        const SubscriptionT& GetSubscription() const { return subscription; }
        const Action& GetAction() const { return action; }

    private:
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & subscriber;
            ar & subscription;
            ar & action;
        }

        friend class boost::serialization::access;

        std::string subscriber;
        SubscriptionT subscription;
        Action action;
    };
}

#endif
