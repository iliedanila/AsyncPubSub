#ifndef _BROKER_IDENTITY_HPP_
#define _BROKER_IDENTITY_HPP_

#include <boost/serialization/access.hpp>
#include <string>

namespace LogicalLayer
{
	class BrokerIdentity
	{
	public:
		BrokerIdentity() {}
		~BrokerIdentity() {}

		explicit BrokerIdentity(const std::string _nodeName)
		:
			nodeName(_nodeName)
		{}

		std::string NodeName() const { return nodeName; }

	private:
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & nodeName;
		}

		friend class boost::serialization::access;
		std::string nodeName;
	};
}

#endif
