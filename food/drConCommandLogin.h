#ifndef __DR_CON_COMMAND_LOGIN_H__
#define __DR_CON_COMMAND_LOGIN_H__

#include "drConCommand.h"
#include <iostream>

namespace dragonBoat {
    class drConCommandLogin : public drConCommand
    {
        typedef drConCommandLogin ThisClass;
        public:
            static drConCommand *newObject() { return new drConCommandLogin("", ""); }
            static bool copyObject(drConCommand* dst, const drConCommand* src)
            {
                *((ThisClass*)dst) = *((const ThisClass*)src);
                return true;
            }

            std::string getUserName() const { return mUserName; }
            std::string getPassword() const { return mPassword; }

        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);

        public:
            drConCommandLogin(const std::string& name, const std::string& pass);

        private:
            std::string mUserName;
            std::string mPassword;
    };

    inline drConCommandLogin::drConCommandLogin(const std::string& name, const std::string& pass)
        : drConCommand(drCT_LOGIN)
          , mUserName(name)
          , mPassword(pass)
    {
    }

    inline bool drConCommandLogin::put(drConTransmitOutStream& out)
    {
        bool ret_code = drConCommand::put(out);

        if (ret_code)
            ret_code = out.putString(mUserName);
        if (ret_code)
            ret_code = out.putString(mPassword);

        return ret_code;
    }

    inline bool drConCommandLogin::get(drConTransmitInStream& in)
    {
        bool ret_code = drConCommand::get(in);

        if (ret_code)
            ret_code = in.getString(mUserName);
        if (ret_code)
            ret_code = in.getString(mPassword);

        return ret_code;
    }
} // end namespace dragonBoat
#endif //__DR_CON_COMMAND_LOGIN_H__
