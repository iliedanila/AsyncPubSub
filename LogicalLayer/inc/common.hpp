#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <functional>
#include <set>
#include <string>

namespace LogicalLayer {

class PublisherData;
class SubscriptionData;

typedef std::set<std::string> SubscriptionT;
typedef std::function<void(PublisherData&)> PublishFunctionT;
typedef std::set<std::string> PublisherIdentityT;
typedef std::function<void(SubscriptionData&)> SubscriptionDataHandlerT;

}  // namespace LogicalLayer

#endif  //_COMMON_HPP_
