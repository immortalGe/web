#ifndef __H_DR_CON_COMMAND3D_H__
#define __H_DR_CON_COMMAND3D_H__

#include "drConCommand.h"

namespace dragonBoat {
    class drConCommand3D : public drConCommand
    {
        public:
            float getX() const {return mX;}
            float getY() const {return mY;}
            float getZ() const {return mZ;}

            void setX(float val) {mX = val;}
            void setY(float val) {mY = val;}
            void setZ(float val) {mZ = val;}

        public:
            virtual bool put(drConTransmitOutStream& out);
            virtual bool get(drConTransmitInStream& in);

        public:
            drConCommand3D(drConCommandType type, float x = 0, float y = 0, float z = 0)
                : drConCommand(type)
                  , mX(x) , mY(y), mZ(z)
        {}

        private:
            float mX;
            float mY;
            float mZ;
    };

    inline bool drConCommand3D::put(drConTransmitOutStream& out)
    {
        bool ret_code = drConCommand::put(out);

        if (ret_code)
            ret_code = out.putFloat(mX);
        if (ret_code)
            ret_code = out.putFloat(mY);
        if (ret_code)
            ret_code = out.putFloat(mZ);

        return ret_code;
    }

    inline bool drConCommand3D::get(drConTransmitInStream& in)
    {
        bool ret_code = drConCommand::get(in);

        if (ret_code)
            ret_code = in.getFloat(&mX);
        if (ret_code)
            ret_code = in.getFloat(&mY);
        if (ret_code)
            ret_code = in.getFloat(&mZ);

        return ret_code;
    }

    /*
     *******************************************************************************
     class drConCommandAcc
     *******************************************************************************
     */

    class drConCommandAcc : public drConCommand3D
    {
        typedef drConCommandAcc ThisClass;
        public:
            drConCommandAcc(float x = 0, float y = 0, float z = 0)
                : drConCommand3D(drCT_ACC, x, y, z)
            {}

            static drConCommand* newObject() { return new drConCommandAcc(); }
            static bool copyObject(drConCommand* dst, const drConCommand* src)
            {
               *((ThisClass*)dst) = *((const ThisClass*)src);
               return true;
            }
    };

    /*
     *******************************************************************************
     class drConCommandGyro
     *******************************************************************************
     */

    class drConCommandGyro : public drConCommand3D
    {
        typedef drConCommandGyro ThisClass;
        public:
            drConCommandGyro(float x = 0, float y = 0, float z = 0)
                : drConCommand3D(drCT_GYRO, x, y, z)
            {}

            static drConCommand* newObject() { return new drConCommandGyro(); }
            static bool copyObject(drConCommand* dst, const drConCommand* src)
            {
               *((ThisClass*)dst) = *((const ThisClass*)src);
               return true;
            }
    };
} // end namespace dragonBoat
#endif //__H_DR_CON_COMMAND3D_H__
