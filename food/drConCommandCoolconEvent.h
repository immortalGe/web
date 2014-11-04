/*
 *  drConCommandCoolconEvent.h
 *  CoolCon
 *
 *  Created by weixiong hu on 11-9-19.
 *  Copyright 2011 HU WEIXIONG. All rights reserved.
 *
 */

#ifndef __DR_CON_COMMAND_COOLCONEVENT_H__
#define __DR_CON_COMMAND_COOLCONEVENT_H__

#include "drConCommand.h"

namespace dragonBoat {

    class drConCommandCoolconEvent: public drConCommand
    {
        typedef drConCommandCoolconEvent ThisClass;
        public:
            static drConCommand *newObject() { return new drConCommandCoolconEvent(); }
            static bool copyObject(drConCommand* dst, const drConCommand* src)
            {
                *((ThisClass*)dst) = *((const ThisClass*)src);
                return true;
            }

        public:
            drConCommandCoolconEvent();

            void setCoolconEvent(const std::string& evt) { mEventBody = evt; }
            std::string getCoolconEvent() const { return mEventBody; }

        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);

        private:
            // coolcon event body
            std::string mEventBody;
    };

    inline drConCommandCoolconEvent::drConCommandCoolconEvent()
        : drConCommand(drCT_COOLCON_EVENT)
    {}

    inline bool drConCommandCoolconEvent::put(drConTransmitOutStream& out)
    {
        bool ret_code = drConCommand::put(out);

        if (ret_code)
            ret_code = out.putString(mEventBody);

        return ret_code;
    }

    inline bool drConCommandCoolconEvent::get(drConTransmitInStream& in)
    {
        bool ret_code = drConCommand::get(in);

        if (ret_code)
            ret_code = in.getString(mEventBody);

        return ret_code;
    }
} // end namespace dragonBoat
#endif //__DR_CON_COMMAND_COOLCONEVENT_H__
