#ifndef __DR_CON_PACKET_CLOSURE_H__
#define __DR_CON_PACKET_CLOSURE_H__

#include "drConPacket.h"

namespace dragonBoat {

    class drConCommand;
    class drConPacketClosure : public drConPacket
    {
        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);

        public:
            drConPacketClosure();
            drConPacketClosure(const drConPacketClosure&);
            virtual ~drConPacketClosure();

            drConPacketClosure& operator = (const drConPacketClosure&);

            void clear();
        public:
            drConCommand* getCommand() const { return mEntity; }
            void putCommand(const drConCommand& cmd);

        private:
            drConCommand * mEntity;
    };
} // end namespace dragonBoat
#endif //__DR_CON_PACKET_CLOSURE_H__
