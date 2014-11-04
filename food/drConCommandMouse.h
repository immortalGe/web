/*
 *  drConCommandMouse.h
 *  CoolCon
 *
 *  Created by weixiong hu on 11-9-19.
 *  Copyright 2011 HU WEIXIONG. All rights reserved.
 *
 */

#ifndef __DR_CON_COMMAND_MOUSE_H__
#define __DR_CON_COMMAND_MOUSE_H__

#include "drConCommand.h"

namespace dragonBoat {
    typedef enum drMouseAction
    {
        drMA_MOVED = 0,
        drMA_CLICKED,
        drMA_DOUBLECLICKED,

        drMA_FOREDWORD = 0x7FFFFFFF
    } drMouseAction;

    class drConCommandMouse : public drConCommand
    {
        typedef drConCommandMouse ThisClass;
        public:
            static drConCommand *newObject() { return new drConCommandMouse(); }
            static bool copyObject(drConCommand* dst, const drConCommand* src)
            {
                *((ThisClass*)dst) = *((const ThisClass*)src);
                return true;
            }

        public:
            drConCommandMouse();

            void setMouseAction(drMouseAction action) { mMouseAction = action; }
            void setMouseXY(int x, int y) { mMouseX = x; mMouseY = y; }
            drMouseAction getMouseAction() const { return mMouseAction; }
            int getMouseX() const { return mMouseX; }
            int getMouseY() const { return mMouseY; }

        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);

        private:
            drMouseAction mMouseAction;
            int mMouseX;
            int mMouseY;
    };

    inline drConCommandMouse::drConCommandMouse()
        : drConCommand(drCT_MOUSE)
          , mMouseAction(drMA_MOVED)
          , mMouseX(0)
          , mMouseY(0)
    {}

    inline bool drConCommandMouse::put(drConTransmitOutStream& out)
    {
        bool ret_code = drConCommand::put(out);

        if (ret_code)
            ret_code = out.putInteger(mMouseAction);
        if (ret_code)
            ret_code = out.putInteger(mMouseX);
        if (ret_code)
            ret_code = out.putInteger(mMouseY);

        return ret_code;
    }

    inline bool drConCommandMouse::get(drConTransmitInStream& in)
    {
        bool ret_code = drConCommand::get(in);

        if (ret_code)
            ret_code = in.getInteger((unsigned int *)&mMouseAction);
        if (ret_code)
            ret_code = in.getInteger((unsigned int *)&mMouseX);
        if (ret_code)
            ret_code = in.getInteger((unsigned int *)&mMouseY);

        return ret_code;
    }
} // end namespace dragonBoat
#endif //__DR_CON_COMMAND_MOUSE_H__
