#include "drConTransmitSocketAddress.h"
#include <cstring>
#include <memory.h>

// dont use memcmp series functions to avoid byte padding problem.
bool operator < (const struct sockaddr_in& addr1, const struct sockaddr_in& addr2)
{
    return addr1.sin_family < addr2.sin_family
        || (addr1.sin_family == addr2.sin_family && addr1.sin_addr.s_addr < addr2.sin_addr.s_addr)
        || (addr1.sin_family == addr2.sin_family && addr1.sin_addr.s_addr == addr2.sin_addr.s_addr && addr1.sin_port < addr2.sin_port);
}
namespace dragonBoat {

    drConTransmitSocketAddress::drConTransmitSocketAddress()
    {
        memset(&mSockAddr, 0, sizeof(mSockAddr));
        mSockAddr.sin_family = AF_INET;
    }
    drConTransmitSocketAddress::drConTransmitSocketAddress(const std::string& addr_port)
    {
        memset(&mSockAddr, 0, sizeof(mSockAddr));
        mSockAddr.sin_family = AF_INET;
        drConSocketAddrConverter::toSockAddr(addr_port, mSockAddr);
    }

    drConTransmitSocketAddress::drConTransmitSocketAddress(const std::string& saddr, const short port)
    {
        mSockAddr.sin_family = AF_INET;
        mSockAddr.sin_port = htons(port);
        mSockAddr.sin_addr.s_addr = inet_addr(saddr.c_str());
    }
    drConTransmitSocketAddress::drConTransmitSocketAddress(const short port)
    {
        mSockAddr.sin_family = AF_INET;
        mSockAddr.sin_port = htons(port);
        mSockAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    }
    drConTransmitSocketAddress::drConTransmitSocketAddress(const sockaddr_in& addr)
        : mSockAddr(addr)
    {
    }
    bool drConTransmitSocketAddress::equalTo(const drConTransmitSocketAddress& other) const
    {
        return (other.getType() == getType())
            && !(mSockAddr < other.mSockAddr)
            && !(other.mSockAddr < mSockAddr);
    }
} // end namesapce dragonBoat
