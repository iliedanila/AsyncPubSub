#ifndef _START_PUBLISH_HPP_
#define _START_PUBLISH_HPP_

#include <string>
#include <boost/serialization/access.hpp>

namespace LogicalLayer
{
    class StartPublish
    {
    public:
        StartPublish() {}
        ~StartPublish() {}

        explicit StartPublish(const std::string _subscriber)
        :
            subscriber(_subscriber)
        {}

        const std::string& Subscriber() const { return subscriber; }

    private:
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & subscriber;
        }

        friend class boost::serialization::access;

        std::string subscriber;
    };
}

#endif