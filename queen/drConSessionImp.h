#ifndef __DR_CON_SESSION_IMP_H__
#define __DR_CON_SESSION_IMP_H__

#include "drConSession.h"
#include "drConPacketClosure.h"
#include "drConSequence.h"
#include "drConTransmitType.h"
#include "drConTransmitterDiagram.h"
#include "drConTransmitSocketAddress.h"

namespace dragonBoat {
    class drConTransmitter;
    class drConPacketClosure;
    class drConCommand;

    class drConSessionImpBase : public drConSession
    {
        public:
            virtual bool startup();
            virtual void shutdown();
            virtual bool workable();

        protected:
            // This function is only limited to the implementation classes ClientSessionImp
            // and ServerSessionImp.
//            static drConSessionImpBase *newOne(drConTransmitType type, bool server);

            friend class drConClientSessionImp;
            friend class drConSearchSessionImp;
            friend class drConServerSessionImp;
            friend class drConClientSessionImp2;

        public:
            drConSessionImpBase(drConTransmitType type, bool server);
            virtual ~drConSessionImpBase();

        protected:

            bool sendPacket(const drConCommand& packet
                    , bool important = false);
            bool sendPacket(const drConCommand& packet
                    , const drConTransmitDiagramAddress& addr
                    , bool important = false);

            drConPacketClosure readPacket(const drConTransmitDiagramAddress& addr
                    , bool important = false);
            drConPacketClosure readAnyPacket(drConTransmitDiagramAddress& addr);

            void discardPackets();
//            drConTransmitAddress *newAddress();

        protected:
            drConTransmitterDiagram mTransmitter;
            drConTransmitDiagramAddress mScratchAddress;

            drConPacketClosure mPacketClosure;
    };

    //inline drConSessionImpBase *drConSessionImpBase::newOne(drConTransmitType type, bool server)
    //{
    //    return new drConSessionImpBase(type, server);
    //}
} // end namespace dragonBoat
#endif //__DR_CON_SESSION_IMP_H__
