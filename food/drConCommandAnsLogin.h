#ifndef __DR_CON_COMMAND_ANSLOGIN_H__
#define __DR_CON_COMMAND_ANSLOGIN_H__

#include "drConCommand.h"

namespace dragonBoat {
    class drConCommandAnsLogin : public drConCommand
    {
        typedef drConCommandAnsLogin ThisClass;
        public:
            drConCommandAnsLogin(bool auth);

            static drConCommand* newObject() { return new drConCommandAnsLogin(false); }
            static bool copyObject(drConCommand* dst, const drConCommand* src)
            {
                *((ThisClass*)dst) = *((const ThisClass*)src);
                return true;
            }
            bool getAuth() const { return mAuth; }
        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);

        protected:
            bool mAuth;
    };

    inline drConCommandAnsLogin::drConCommandAnsLogin(bool auth)
        : drConCommand(drCT_ANS_LOGIN)
          , mAuth(auth)
    {
    }

    inline bool drConCommandAnsLogin::put(drConTransmitOutStream& out)
    {
        if (drConCommand::put(out))
        {
            return out.putInteger(mAuth ? 1 : 0);
        }

        return false;
    }

    inline bool drConCommandAnsLogin::get(drConTransmitInStream& in)
    {
        if (drConCommand::get(in))
        {
            //        unsigned long auth = 0;
            unsigned int auth = 0;
            if (in.getInteger(&auth))
            {
                mAuth = (auth == 1);
                return true;
            }
        }

        return false;
    }
} // end namespace dragonBoat
#endif //__DR_CON_COMMAND_ANSLOGIN_H__
