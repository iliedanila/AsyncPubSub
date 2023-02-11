#ifndef _SUBSCRIPTION_DATA_HPP_
#define _SUBSCRIPTION_DATA_HPP_

#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>
#include <string>

#include "common.hpp"

namespace LogicalLayer {

class SubscriptionData {
   public:
    SubscriptionData() {}

    ~SubscriptionData() {}

    explicit SubscriptionData(const std::string& aPublisherName,
                              const SubscriptionT& aSubscription)
        : publisherName(aPublisherName), subscription(aSubscription) {}

    void addData(const std::string& aDataType, const std::string& aData) {
        data.insert(std::make_pair(aDataType, aData));
    }

    const std::string& getPublisherName() const { return publisherName; }

    const std::map<std::string, std::string>& getData() const { return data; }

    const SubscriptionT getSubscription() const { return subscription; }

   private:
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& publisherName;
        ar& subscription;
        ar& data;
    }

    friend class boost::serialization::access;

    std::string publisherName;
    SubscriptionT subscription;
    std::map<std::string, std::string> data;
};

}  // namespace LogicalLayer

#endif
