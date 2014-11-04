#ifndef __DR_CON_COMMAND_LOGOUT_H__
#define __DR_CON_COMMAND_LOGOUT_H__

#include "drConCommand.h"

namespace dragonBoat {
    class drConCommandLogout : public drConCommand
    {
        typedef drConCommandLogout ThisClass;
        public:
            static drConCommand *newObject() { return new drConCommandLogout(); }
            static bool copyObject(drConCommand* dst, const drConCommand* src)
            {
                *((ThisClass*)dst) = *((const ThisClass*)src);
                return true;
            }

        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);

        public:
            drConCommandLogout();
    };

    inline drConCommandLogout::drConCommandLogout()
        : drConCommand(drCT_LOGOUT)
    {
    }

    inline bool drConCommandLogout::put(drConTransmitOutStream& out)
    {
        if (drConCommand::put(out))
        {
            return out.putInteger(0);
        }

        return false;
    }

    inline bool drConCommandLogout::get(drConTransmitInStream& in)
    {
        if (drConCommand::get(in))
        {
            unsigned int val;
            return (in.getInteger(&val));
        }

        return false;
    }
} // end namespace dragonBoat
#endif //__DR_CON_COMMAND_LOGOUT_H__
