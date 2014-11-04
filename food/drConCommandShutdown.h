#ifndef __DR_CON_COMMAND_SHUTDOWN_H__
#define __DR_CON_COMMAND_SHUTDOWN_H__

#include "drConCommand.h"

namespace dragonBoat {
    class drConCommandShutdown : public drConCommand
    {
        typedef drConCommandShutdown ThisClass;
        public:
            static drConCommand *newObject() { return new drConCommandShutdown(); }
            static bool copyObject(drConCommand* dst, const drConCommand* src)
            {
                *((ThisClass*)dst) = *((const ThisClass*)src);
                return true;
            }

        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);

        public:
            drConCommandShutdown();
        private:
            std::string mString;
    };

    inline drConCommandShutdown::drConCommandShutdown()
        : drConCommand(drCT_SHUTDOWN)
          , mString("SHUTDOWN")
    {
    }

    inline bool drConCommandShutdown::put(drConTransmitOutStream& out)
    {
        if (drConCommand::put(out))
        {
            return out.putString(mString);
        }

        return false;
    }

    inline bool drConCommandShutdown::get(drConTransmitInStream& in)
    {
        if (drConCommand::get(in))
        {
            std::string str_shut;
            if (in.getString(str_shut))
            {
                return (mString == str_shut);
            }
        }

        return false;
    }
}
#endif //__DR_CON_COMMAND_SHUTDOWN_H__
