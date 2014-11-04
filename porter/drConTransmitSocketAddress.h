#ifndef __DR_CON_TRANSMIT_SOCKET_ADDRESS_H__
#define __DR_CON_TRANSMIT_SOCKET_ADDRESS_H__

#include "drConTransmitAddress.h"
#include "drConSocket.h"

bool operator < (const struct sockaddr_in&, const struct sockaddr_in&);

namespace dragonBoat {
    class drConSocketAnyAddress
    {
        public:
            sockaddr_in addr_in;

            drConSocketAnyAddress(int port)
            {
                addr_in.sin_family = AF_INET;
                addr_in.sin_port = htons(port);
                addr_in.sin_addr.s_addr = inet_addr("255.255.255.255");
            }
    };


    class drConTransmitSocketAddress : public drConTransmitAddress
    {
        public:
            drConTransmitSocketAddress();
            drConTransmitSocketAddress(const std::string& addr_port);
            drConTransmitSocketAddress(const std::string& saddr, const short port);
            drConTransmitSocketAddress(const short port);
            drConTransmitSocketAddress(const sockaddr_in& addr);

            virtual ~drConTransmitSocketAddress() {};

        public:
            virtual drConTransmitType getType() const = 0;
            virtual std::string toString() const { return drConSocketAddrConverter::toString(mSockAddr); }

            bool equalTo(const drConTransmitSocketAddress& other) const;
            //    virtual drConTransmitAddress *copy() const = 0;
            bool lessThan(const drConTransmitSocketAddress& other) const
            {
                return mSockAddr < other.mSockAddr;
            };
        public:
            void setSockAddr(const sockaddr_in& addr) { mSockAddr = addr; }
            sockaddr_in getSockAddr() const { return mSockAddr; }
            unsigned short getPort() const { return mSockAddr.sin_port; }
        private:
            struct sockaddr_in mSockAddr;
    };
} // end namespace dragonBoat
#endif //__DR_CON_TRANSMITTER_SOCKET_ADDRESS_H__
