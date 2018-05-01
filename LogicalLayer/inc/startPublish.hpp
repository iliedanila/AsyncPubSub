#ifndef _START_PUBLISH_HPP_
#define _START_PUBLISH_HPP_

#include <string>
#include <boost/serialization/access.hpp>
#include <boost/serialization/set.hpp>

#include "subscription.hpp"

namespace LogicalLayer
{
    class StartPublish
    {
    public:
        StartPublish() {}
        ~StartPublish() {}

        explicit StartPublish(
            const std::string _subscriber,
            SubscriptionT _subscription)
        :
            subscriberName(_subscriber),
            subscription(_subscription)
        {}

        const std::string& getSubscriberName() const { return subscriberName; }
        const SubscriptionT& getSubscription() const { return subscription; }

    private:
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & subscriberName;
            ar & subscription;
        }

        friend class boost::serialization::access;

        std::string subscriberName;
        SubscriptionT subscription;
    };
}

#endif