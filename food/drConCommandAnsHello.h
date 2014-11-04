#ifndef __DR_CON_COMMAND_ANSHELLO_H__
#define __DR_CON_COMMAND_ANSHELLO_H__

#include "drConCommand.h"

namespace dragonBoat {
    class drConCommandAnsHello : public drConCommand
    {
        typedef drConCommandAnsHello ThisClass;
        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);

        public:
            drConCommandAnsHello(const std::string& name);
            static drConCommand* newObject() { return new drConCommandAnsHello(""); }
            static bool copyObject(drConCommand* dst, const drConCommand* src)
            {
                *((ThisClass*)dst) = *((const ThisClass*)src);
                return true;
            }

            std::string getName() { return mName; }
        private:
            std::string mName;
    };

    inline drConCommandAnsHello::drConCommandAnsHello(const std::string& name)
        : drConCommand(drCT_ANS_HELLO)
          , mName(name)
    {
    }

    inline bool drConCommandAnsHello::put(drConTransmitOutStream& out)
    {
        if (drConCommand::put(out))
        {
            return out.putString(mName);
        }

        return false;
    }

    inline bool drConCommandAnsHello::get(drConTransmitInStream& in)
    {
        if (drConCommand::get(in))
        {
            return in.getString(mName);
        }

        return false;
    }
} // end namespace dragonBoat
#endif //__DR_CON_COMMAND_ANSHELLO_H__
