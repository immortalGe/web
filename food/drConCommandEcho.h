#ifndef __DR_CON_COMMAND_ECHO_H__
#define __DR_CON_COMMAND_ECHO_H__

#include "drConCommand.h"

namespace dragonBoat {
    class drConCommandEcho : public drConCommand
    {
        typedef drConCommandEcho ThisClass;
        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);
        public:
            drConCommandEcho();

            static drConCommand* newObject() { return new drConCommandEcho(); }
            static bool copyObject(drConCommand* dst, const drConCommand* src)
            {
                *((ThisClass*)dst) = *((const ThisClass*)src);
                return true;
            }

        private:
            std::string mString;
    };

    inline drConCommandEcho::drConCommandEcho()
        : drConCommand(drCT_ECHO)
          , mString("ECHO")
    {
    }

    inline bool drConCommandEcho::put(drConTransmitOutStream& out)
    {
        if (drConCommand::put(out))
        {
            return out.putString(mString);
        }

        return false;
    }

    inline bool drConCommandEcho::get(drConTransmitInStream& in)
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
} // end namespace dragonBoat
#endif //__DR_CON_COMMAND_ECHO_H__
