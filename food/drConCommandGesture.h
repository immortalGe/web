/*
 *  drConCommandGesture.h
 *  CoolCon
 *
 *  Created by weixiong hu on 11-9-19.
 *  Copyright 2011 HU WEIXIONG. All rights reserved.
 *
 */

#ifndef __DR_CON_COMMAND_GESTURE_H__
#define __DR_CON_COMMAND_GESTURE_H__

#include "drConCommand.h"

namespace dragonBoat {
    typedef enum drGestureAction
    {
        drGA_NONE = 0,
        drGA_SINGLETAP,         // 1
        drGA_DOUBLETAP,         // 2
        drGA_FLING_LEFT,        // 3
        drGA_FLING_RIGHT,       // 4
        drGA_FLING_UP,          // 5
        drGA_FLING_DOWN,        // 6
        drGA_TOUCH_DOWN,        // 7
        drGA_TOUCH_MOVE,        // 8
        drGA_TOUCH_UP,          // 9

        drGA_FOREDWORD = 0x7FFFFFFF
    } drGestureAction;

    class drConCommandGesture : public drConCommand
    {
        typedef drConCommandGesture ThisClass;
        public:
            static drConCommand *newObject() { return new drConCommandGesture(); }
            static bool copyObject(drConCommand* dst, const drConCommand* src)
            {
                *((ThisClass*)dst) = *((const ThisClass*)src);
                return true;
            }

        public:
            drConCommandGesture();

            void setGestureAction(int action) { mGestureAction = action; }
            int getGestureAction() const { return mGestureAction; }

            // gesture event relative position x, y
            void setGestureXY(float x, float y) { mTapX = x; mTapY = y; }
            float getGestureX() const { return mTapX; }
            float  getGestureY() const { return mTapY; }

            void setScreenSize(int width, int height) { mWidth = width; mHeight = height; }
            int getWidth() const { return mWidth; }
            int getHeight() const { return mHeight; }

        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);

        private:
            // drGestureAction
            int mGestureAction;

            // relative position of point
            // in range of [0.0f, 1.0f]
            // which is pos to width/height ratio
            float mTapX, mTapY;

            // size of display screen, mixture with screencap
            int mWidth, mHeight;
    };

    inline drConCommandGesture::drConCommandGesture()
        : drConCommand(drCT_GESTURE)
          , mGestureAction(drGA_NONE)
          , mTapX(0)
          , mTapY(0)
          , mWidth(0)
          , mHeight(0)
    {}

    inline bool drConCommandGesture::put(drConTransmitOutStream& out)
    {
        bool ret_code = drConCommand::put(out);

        if (ret_code)
            ret_code = out.putInteger(mGestureAction);
        if (ret_code)
            ret_code = out.putFloat(mTapX);
        if (ret_code)
            ret_code = out.putFloat(mTapY);
        if (ret_code)
            ret_code = out.putInteger(mWidth);
        if (ret_code)
            ret_code = out.putInteger(mHeight);

        return ret_code;
    }

    inline bool drConCommandGesture::get(drConTransmitInStream& in)
    {
        bool ret_code = drConCommand::get(in);

        if (ret_code)
            ret_code = in.getInteger((unsigned int *)&mGestureAction);
        if (ret_code)
            ret_code = in.getFloat(&mTapX);
        if (ret_code)
            ret_code = in.getFloat(&mTapY);
        if (ret_code)
            ret_code = in.getInteger((unsigned int*)&mWidth);
        if (ret_code)
            ret_code = in.getInteger((unsigned int*)&mHeight);

        return ret_code;
    }
} // end namespace dragonBoat
#endif //__DR_CON_COMMAND_MOUSE_H__
