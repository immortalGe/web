#include "drConTransmitterDiagram.h"
#include "drConTransmitSocketAddress.h"
#include "drConTransmitSocketStream.h"
#include "drConPacket.h"
#include "drConLogger.h"

namespace dragonBoat {

    //drConTransmitAddress* drConTransmitterDiagram::createAddress(const std::string& address)
    //{
    //    sockaddr_in addr = {0};
    //
    //    if (drConSocketAddrConverter::toSockAddr(address, addr))
    //    {
    //        addr.sin_port = htons(kServerPort);
    //        return drConTransmitDiagramAddress::newOne(&addr);
    //    }
    //
    //    return NULL;
    //}

    //drConTransmitAddress* drConTransmitterDiagram::createAnyAddress()
    //{
    //    drConSocketAnyAddress any_addr(kServerPort);
    //    return drConTransmitDiagramAddress::newOne(&any_addr.addr_in);
    //}

    void drConTransmitterDiagram::shutdown()
    {
        if ( mSocket )
        {
            delete mSocket;
            mSocket = 0;
        }
    }

    bool drConTransmitterDiagram::workable()
    {
        return ( mSocket && mSocket->workable() );
    }

//    drConTransmitAddress* drConTransmitterDiagram::newAddress(const char *addr)
//    {
//        if (addr == NULL)
//        {
//            return createAnyAddress();
//        }
//        else
//        {
//            std::string addr_str(addr);
//            return createAddress(addr_str);
//        }
//    }

    bool drConTransmitterDiagram::sendPacket(drConPacket& packet)
    {
        if ( workable() )
        {
            mOutStream.position(0);
            if (packet.put(mOutStream))
            {
                int data_size = mOutStream.getSize();
                const void *buffer = mOutStream.getBuffer();
                drConSocketAnyAddress any_addr(kServerPort);

                int send_bytes = mSocket->sendTo(buffer, data_size, any_addr.addr_in);
                return (data_size == send_bytes);
            }
        }

        return false;
    }

    bool drConTransmitterDiagram::sendPacket(drConPacket& packet, const drConTransmitAddress& addr)
    {
        if ( addr.getType() != drTT_DIAGRAM )
            return false;

        if ( workable() )
        {
            mOutStream.position(0);
            if (packet.put(mOutStream))
            {
                const drConTransmitSocketAddress* sock_addr = (const drConTransmitSocketAddress *)&addr;
                const sockaddr_in& addr_in = sock_addr->getSockAddr();

                int data_size = mOutStream.getSize();
                int send_bytes = mSocket->sendTo(mOutStream.getBuffer(), data_size, addr_in);

                return (data_size == send_bytes);
            }
        }

        return false;
    }

    bool drConTransmitterDiagram::readPacket(drConPacket& packet, drConTransmitAddress& addr)
    {
        if ( addr.getType() != drTT_DIAGRAM )
            return false;

        if (workable())
        {
            sockaddr_in addr_in = {0};
            drConTransmitSocketAddress *sock_addr = (drConTransmitSocketAddress *)&addr;
            int buf_size = mInStream.getCapacity();

            mInStream.position(0);
            if (mSocket->recvFrom(mInStream.getWriteBuffer(), buf_size, addr_in) > 0)
            {
                //            mInStream->dump();
                sock_addr->setSockAddr(addr_in);
                return packet.get(mInStream);
            }
        }

        return false;
    }

    drConTransmitterDiagram::drConTransmitterDiagram(bool asServer)
        : mSocket(0)
          , mInitialized(false)
          , mAsServer(asServer)
          , mInStream(3 * (1 << 10))
          , mOutStream(3 * (1 << 10))
    {
    }

    drConTransmitterDiagram::~drConTransmitterDiagram()
    {
        shutdown();
    }

    bool drConTransmitterDiagram::startup()
    {
        if ( mSocket )
        {
            drCon::log(__FUNCTION__, "it has started!");
            return false;
        }

        if (mAsServer)
        {
            mSocket = drConSocket::createServerSocket(kServerPort);
        }
        else
        {
            mSocket = drConSocket::createClientSocket();
        }

        if ( !mSocket->workable() )
        {
            delete mSocket;
            mSocket = 0;
            drCon::log(__FUNCTION__, "failed to create socket.");
            return false;
        }

        return true;
    }
} // end namespace dragonBoat
