#ifndef _SUBSCRIPTION_HPP_
#define _SUBSCRIPTION_HPP_

#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/utility.hpp>
#include <set>
#include <vector>

#include "common.hpp"

namespace LogicalLayer {

class AddRemoveSubscriptionMessage {
   public:
    enum Action { eAdd = 1, eRemove };

    AddRemoveSubscriptionMessage() {}

    ~AddRemoveSubscriptionMessage() {}

    explicit AddRemoveSubscriptionMessage(const std::string aSubscriberName,
                                          const SubscriptionT& aSubscription,
                                          Action anAction)
        : subscriberName(aSubscriberName),
          subscription(aSubscription),
          action(anAction) {}

    const std::string& getSubscriberName() const { return subscriberName; }

    const SubscriptionT& getSubscription() const { return subscription; }

    const Action& getAction() const { return action; }

   private:
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& subscriberName;
        ar& subscription;
        ar& action;
    }

    friend class boost::serialization::access;

    std::string subscriberName;
    SubscriptionT subscription;
    Action action;
};

}  // namespace LogicalLayer

#endif
