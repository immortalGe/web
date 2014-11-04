#ifndef __DR_CON_COMMAND_H__
#define __DR_CON_COMMAND_H__

#include "drConPacket.h"
#include "drConTransmitStream.h"
#include "drConSequence.h"

namespace dragonBoat {
    typedef enum drConCommandType
    {
        drCT_NONE_MIN = 0,              // 0

        drCT_KEY = 1,                   // 1
        drCT_ACC,                       // 2
        drCT_GYRO,                      // 3

        drCT_SAY_HELLO,                 // 4
        drCT_ANS_HELLO,                 // 5

        drCT_LOGIN,                     // 6
        drCT_ANS_LOGIN,                 // 7

        drCT_MOTE_STATE,                // 8

        drCT_MOUSE,                     // 9

        drCT_LOGOUT,                    // 10

        drCT_ECHO,                      // 11

        drCT_SHUTDOWN,                  // 12
        /////////////////////////////////////////////////////////////////
        /// commands before this is settled when CoolCon 1.1.9 released.

        drCT_MOTE_STATES,               // 13

        // gesture movement (sliding, tap, double tap, etc.)
        drCT_GESTURE,                   // 14

        // coolcon events between CoolCon_NEW and AndroidGameLauncher
        drCT_COOLCON_EVENT,             // 15

        drCT_NONE_MAX,
        drCT_FORCEWORD = 0x7FFF
    } drConCommandType;

    class drConCommand : public drConPacket
    {
        public:
            drConCommand(drConCommandType type);
        public:
            drConCommandType getType() const { return mType; }
            bool hasType(drConCommandType type) const { return mType == type; }

            drConSequence getSequence() const { return mSequence; }
            void setSequence(const drConSequence& sequence) { mSequence = sequence; }

            drConSequence getRequest() const { return mRequest; }
            void setRequest(const drConSequence& sequence) { mRequest = sequence; }

            static std::string typeToString(drConCommandType type);

        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);

        protected:
            drConCommandType mType;
            drConSequence mSequence;
            drConSequence mRequest;

            static drConSequence gSequence;
    };

    inline drConCommand::drConCommand(drConCommandType type)
        : mType(type)
          , mSequence()
          , mRequest(0)
    {
    }
} // end namespace dragonBoat
#endif //__DR_CON_COMMAND_H__
