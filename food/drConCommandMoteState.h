#ifndef __DR_CON_COMMAND_MOTE_STATE_H__
#define __DR_CON_COMMAND_MOTE_STATE_H__

#include "drConCommand.h"

namespace dragonBoat {
    class drConVector
    {
        public:
            float x;
            float y;
            float z;

        public:
            drConVector(float _x = 0, float _y = 0, float _z = 0)
                : x(_x), y(_y), z(_z)
            {}

            void set(float _x, float _y, float _z)
            {
                x = _x; y = _y; z = _z;
            }

            void get(float& _x, float& _y, float& _z)
            {
                _x = x; _y = y; _z = z;
            }
    };

    class drConKeyState
    {
        public:
            drConKeyState(unsigned int keys = 0)
                : mKeys(keys)
            {}

            unsigned int getKeys() const {return mKeys;}
            void setKeys(unsigned int val) {mKeys = val;}

        private:
            unsigned int mKeys;
    };

    class drConCommandMoteState : public drConCommand
    {
        typedef drConCommandMoteState ThisClass;
        public:
            static drConCommand *newObject() { return new drConCommandMoteState(); }
            static bool copyObject(drConCommand* dst, const drConCommand* src)
            {
                *((ThisClass*)dst) = *((const ThisClass*)src);
                return true;
            }

        public:
            drConCommandMoteState()
                : drConCommand(drCT_MOTE_STATE)
            {}

            const drConKeyState getKey() const {return mKeys;}
            const drConVector getAcc() const {return mAcc;}
            const drConVector getGyro() const {return mGyro;}

            void setKey(const drConKeyState& val) {mKeys = val;};
            void setAcc(const drConVector& val) {mAcc = val;}
            void setGyro(const drConVector& val) {mGyro = val;}

        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);

        private:
            drConKeyState mKeys;
            drConVector mAcc;
            drConVector mGyro;
    };

    inline bool drConCommandMoteState::put(drConTransmitOutStream& out)
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

        return ret_code;
    }

    inline bool drConCommandMoteState::get(drConTransmitInStream& in)
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
            }
        }

        return ret_code;
    }
} // end namespace dragonBoat
#endif //__DR_CON_COMMAND_MOTE_STATE_H__
