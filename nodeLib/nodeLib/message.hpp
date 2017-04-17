#ifndef _MESSAGE_HPP_
#define _MESSAGE_HPP_

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <boost/asio/detail/buffer_sequence_adapter.hpp>

namespace NetworkLayer
{
    class Message
    {
    public:
        enum { eHeaderLength = 4 };

        Message()
            :
            buffer(nullptr),
            bodyLength(0)
        {}

        ~Message()
        {
            delete[] buffer;
        }

        Message(const Message& other)
        {
            memcpy(header, other.header, eHeaderLength);
            bodyLength = other.bodyLength;
            buffer = new char[bodyLength + eHeaderLength];
            memcpy(buffer, other.buffer, eHeaderLength + bodyLength);
        }

        const char* Header() const { return header; }
        char* Header() { return header; }

        const char* Buffer() const { return buffer; }
        char* Buffer() { return buffer; }

        uint32_t Length() const { return eHeaderLength + bodyLength; }

        const char* Body() const { return buffer + eHeaderLength; }

        char* Body() { return buffer + eHeaderLength; }

        uint32_t BodyLength() const { return bodyLength; }

        void SetBodyLength(uint32_t newLength)
        {
            bodyLength = newLength;
            buffer = new char[eHeaderLength + bodyLength];
        }

        void DecodeHeader()
        {
            uint32_t networkBodyLength;
            memcpy(&networkBodyLength, header, eHeaderLength);
            bodyLength = ntohl(networkBodyLength);

            if (buffer == nullptr)
            {
                buffer = new char[eHeaderLength + bodyLength];
            }
        }

        void EncodeHeader()
        {
            uint32_t networkBodyLength = htonl(bodyLength);
            memcpy(buffer, &networkBodyLength, eHeaderLength);
        }

    private:
        char header[eHeaderLength + 1];
        char* buffer;
        uint32_t bodyLength;
    };

}

#endif // _MESSAGE_HPP_
