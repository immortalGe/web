#include "drConCommand.h"

namespace dragonBoat {
    bool drConCommand::put(drConTransmitOutStream& out)
    {
        bool ret_code = true;

        if (ret_code)
            ret_code = mSequence.put(out);
        if (ret_code)
            ret_code = mRequest.put(out);

        return ret_code;
    }

    bool drConCommand::get(drConTransmitInStream& in)
    {
        bool ret_code = true;

        if (ret_code)
            ret_code = mSequence.get(in);
        if (ret_code)
            ret_code = mRequest.get(in);

        return ret_code;
    }

    std::string drConCommand::typeToString(drConCommandType type)
    {
        std::string str("invalid");

#define CMD_CASE(T) case T: str = #T; break;

        switch(type)
        {
            case drCT_KEY:         str = "drCT_KEY";         break;
            case drCT_ACC:         str = "drCT_ACC";         break;
            case drCT_GYRO:        str = "drCT_GYRO";        break;
            case drCT_MOTE_STATE:  str = "drCT_MOTE_STATE";  break;
            case drCT_MOTE_STATES: str = "drCT_MOTE_STATES"; break;
            case drCT_MOUSE:       str = "drCT_MOUSE";       break;

            case drCT_LOGIN:       str = "drCT_LOGIN";       break;
            case drCT_ANS_LOGIN:   str = "drCT_ANS_LOGIN";   break;
            case drCT_SAY_HELLO:   str = "drCT_SAY_HELLO";   break;
            case drCT_ANS_HELLO:   str = "drCT_ANS_HELLO";   break;
            case drCT_SHUTDOWN:    str = "drCT_SHUTDOWN";    break;

            case drCT_ECHO:        str = "drCT_ECHO";        break;

            default: break;
        }

        return str;
    }
} // end namespace dragonBoat
