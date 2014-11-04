#ifndef __DR_CON_TRANSMITER_DIAGRAM_H__
#define __DR_CON_TRANSMITER_DIAGRAM_H__

#include "drConTransmitter.h"
#include "drConSocket.h"
#include "drConTransmitSocketStream.h"
#include "drConTransmitSocketAddress.h"

namespace dragonBoat {
    class drConTransmitDiagramOutStream;
    class drConTransmitDiagramInStream;

    class drConTransmitterDiagram : public drConTransmitter
    {
        public:
        enum {kServerPort = 10101};
        virtual drConTransmitType getType() const { return drTT_DIAGRAM; }

        virtual bool startup();
        virtual void shutdown();
        virtual bool workable();

//        virtual drConTransmitAddress* newAddress(const char *addr = NULL);

        virtual bool sendPacket(drConPacket& packet);
        virtual bool sendPacket( drConPacket& packet, const drConTransmitAddress& addr);
        virtual bool readPacket(drConPacket& packet, drConTransmitAddress& addr);

        public:
        //    static drConTransmitterDiagram *newOne(bool asServer);

//        static drConTransmitAddress* createAddress(const std::string& address);
//        static drConTransmitAddress* createAnyAddress();

        public:
        drConTransmitterDiagram(bool asServer);
        virtual ~drConTransmitterDiagram();

        private:
        drConTransmitterDiagram(const drConTransmitterDiagram&);
        drConTransmitterDiagram& operator = (const drConTransmitterDiagram&);

        private:
        drConSocket * mSocket;
        bool mInitialized;
        bool mAsServer;

        drConTransmitDiagramInStream mInStream;
        drConTransmitDiagramOutStream mOutStream;
    };

    class drConTransmitDiagramAddress : public drConTransmitSocketAddress
    {
        public:
            drConTransmitDiagramAddress()
            {
            }
            drConTransmitDiagramAddress(const std::string& addr_port)
                : drConTransmitSocketAddress(addr_port)
            {
            }
            drConTransmitDiagramAddress(const sockaddr_in& addr)
                : drConTransmitSocketAddress(addr)
            {
            }
            drConTransmitDiagramAddress(const short port)
                : drConTransmitSocketAddress(port)
            {
            }
            virtual ~drConTransmitDiagramAddress() {}

        public:
            virtual drConTransmitType getType() const { return drTT_DIAGRAM; }
            bool equalTo(const drConTransmitDiagramAddress& other) const { return drConTransmitSocketAddress::equalTo(other); }
    };
    // used for STL containers
    class drConAddrComp
    {
        public:
            bool operator () (const drConTransmitDiagramAddress& left
                    , const drConTransmitDiagramAddress& right) const
            {
                return left.lessThan((const drConTransmitSocketAddress&)right);
            }
    };

    //inline drConTransmitterDiagram *drConTransmitterDiagram::newOne(bool asServer)
    //{
    //    return new drConTransmitterDiagram(asServer);
    //}
} // end namespace dragonBoat
#endif //__DR_CON_TRANSMITER_DIAGRAM_H__
