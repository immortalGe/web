#ifndef __DR_CON_COMMAND_MOTE_STATES_H__
#define __DR_CON_COMMAND_MOTE_STATES_H__

#include "drConCommand.h"
#include "drConCommandMoteState.h"

namespace dragonBoat {
    class drConJoystick
    {
        public:
            int joy_x;
            int joy_y;
        public:
            drConJoystick(int x = 0, int y = 0)
                : joy_x(x)
                  , joy_y(y)
        {}
            void set(int _x, int _y)
            {
                joy_x = _x;
                joy_y = _y;
            }
            void get(int& _x, int& _y)
            {
                _x = joy_x;
                _y = joy_y;
            }
    };

    class drConCommandMoteStates : public drConCommand
    {
        typedef drConCommandMoteStates ThisClass;
        public:
            static drConCommand *newObject() { return new drConCommandMoteStates(); }
            static bool copyObject(drConCommand* dst, const drConCommand* src)
            {
                *((ThisClass*)dst) = *((const ThisClass*)src);
                return true;
            }

        public:
            drConCommandMoteStates()
                : drConCommand(drCT_MOTE_STATES)
            {}

            const drConKeyState getKey() const {return mKeys;}
            const drConVector getAcc() const {return mAcc;}
            const drConVector getGyro() const {return mGyro;}
            const drConJoystick getJoystick() const {return mJoystick;}

            void setKey(const drConKeyState& val) {mKeys = val;}
            void setAcc(const drConVector& val) {mAcc = val;}
            void setGyro(const drConVector& val) {mGyro = val;}
            void setJoystick(const drConJoystick& val) {mJoystick = val;}
        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);

        private:
            drConKeyState mKeys;
            drConVector mAcc;
            drConVector mGyro;
            drConJoystick mJoystick;
    };

    inline bool drConCommandMoteStates::put(drConTransmitOutStream& out)
    {
        bool ret_code = drConCommand::put(out);

        if (ret_code)
            ret_code = out.putInteger(mKeys.getKeys());

        if (ret_code)
            ret_code = out.putFloat(mAcc.x);
        if (ret_code)
            ret_code = out.putFloat(mAcc.y);
        if (ret_code)
            ret_code = out.putFloat(mAcc.z);

        if (ret_code)
            ret_code = out.putFloat(mGyro.x);
        if (ret_code)
            ret_code = out.putFloat(mGyro.y);
        if (ret_code)
            ret_code = out.putFloat(mGyro.z);

        if (ret_code)
            ret_code = out.putInteger(mJoystick.joy_x);
        if (ret_code)
            ret_code = out.putInteger(mJoystick.joy_y);

        return ret_code;
    }

    inline bool drConCommandMoteStates::get(drConTransmitInStream& in)
    {
        bool ret_code = drConCommand::get(in);

        unsigned int keys;
        if (ret_code)
            ret_code = in.getInteger(&keys);
        if (ret_code)
        {
            mKeys.setKeys(keys);

            float x, y, z;

            if (ret_code)
                ret_code = in.getFloat(&x);
            if (ret_code)
                ret_code = in.getFloat(&y);
            if (ret_code)
                ret_code = in.getFloat(&z);

            if (ret_code)
            {
                mAcc.set(x, y, z);

                if (ret_code)
                    ret_code = in.getFloat(&x);
                if (ret_code)
                    ret_code = in.getFloat(&y);
                if (ret_code)
                    ret_code = in.getFloat(&z);

                mGyro.set(x, y, z);

                unsigned int joyx, joyy;
                if (ret_code)
                    ret_code = (int)in.getInteger(&joyx);
                if (ret_code)
                    ret_code = (int)in.getInteger(&joyy);

                mJoystick.set(joyx, joyy);
            }
        }

        return ret_code;
    }
} // end namespace dragonBoat
#endif //__DR_CON_COMMAND_MOTE_STATES_H__
