#ifndef __DR_CON_COMMAND_SAYHELLO_H__
#define __DR_CON_COMMAND_SAYHELLO_H__

#include "drConCommand.h"

namespace dragonBoat {
    class drConCommandSayHello : public drConCommand
    {
        typedef drConCommandSayHello ThisClass;
        public:
            static drConCommand *newObject() { return new drConCommandSayHello(); }
            static bool copyObject(drConCommand* dst, const drConCommand* src)
            {
                *((ThisClass*)dst) = *((const ThisClass*)src);
                return true;
            }

        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);

        public:
            drConCommandSayHello();
        private:
            std::string mStrHello;
    };

    inline drConCommandSayHello::drConCommandSayHello()
        : drConCommand(drCT_SAY_HELLO)
          , mStrHello("HELLO")
    {
    }

    inline bool drConCommandSayHello::put(drConTransmitOutStream& out)
    {
        if (drConCommand::put(out))
        {
            return out.putString(mStrHello);
        }

        return false;
    }

    inline bool drConCommandSayHello::get(drConTransmitInStream& in)
    {
        if (drConCommand::get(in))
        {
            std::string hello;

            if (in.getString(hello))
            {
                return (hello == mStrHello);
            }
        }

        return false;
    }
} // end namespace dragonBoat
#endif //__DR_CON_COMMAND_SAYHELLO_H__
