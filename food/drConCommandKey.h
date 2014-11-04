#ifndef __DR_CON_COMMAND_KEY_H__
#define __DR_CON_COMMAND_KEY_H__

#include "drConCommand.h"

namespace dragonBoat {
    class drConCommandKey : public drConCommand
    {
        typedef drConCommandKey ThisClass;
        public:
            static drConCommand *newObject() { return new drConCommandKey(); }
            static bool copyObject(drConCommand* dst, const drConCommand* src)
            {
                *((ThisClass*)dst) = *((const ThisClass*)src);
                return true;
            }

            int getKey() const { return mKey; }
            int getStatus() const { return mStatus; }
            void setKey(int val) { mKey = val; }
            void setStatus(int val) { mStatus = val; }

        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);

        public:
            drConCommandKey(int key = 0, int status = 0);

        private:
            int mKey;
            int mStatus;
    };

    inline drConCommandKey::drConCommandKey(int key, int status)
        : drConCommand(drCT_KEY)
          , mKey(key)
          , mStatus(status)
    {
    }

    inline bool drConCommandKey::put(drConTransmitOutStream& out)
    {
        bool ret_code = drConCommand::put(out);

        if (ret_code)
            ret_code = out.putInteger(mKey);
        if (ret_code)
            ret_code = out.putInteger(mStatus);

        return ret_code;
    }

    inline bool drConCommandKey::get(drConTransmitInStream& in)
    {
        bool ret_code = drConCommand::get(in);

        if (ret_code)
            ret_code = in.getInteger((unsigned int*)&mKey);
        if (ret_code)
            ret_code = in.getInteger((unsigned int*)&mStatus);

        return ret_code;
    }
} // end namespace dragonBoat
#endif //__DR_CON_COMMAND_KEY_H__
