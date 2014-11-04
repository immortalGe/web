#ifndef __DR_CON_CLIENT_SESSION_IMP_H__
#define __DR_CON_CLIENT_SESSION_IMP_H__

#include "drConSessionImp.h"
#include "drConTargetImp.h"
#include "drConTime.h"

namespace dragonBoat {
    class drConClientSessionImp : public drConClientSession
    {
        public:
            //    virtual drConTarget* getTarget();
            virtual bool startup();
            virtual void shutdown();
            virtual bool workable();

            virtual bool doLogin(const std::string& name, const std::string& pin);
            virtual void doLogout();

            virtual bool sendKeyData(int key, int status);
            virtual bool sendAccData(float x, float y, float z);
            virtual bool sendGyroData(float x, float y, float z);

            virtual bool sendMoteData(unsigned long keys, float acc[3], float gyro[3]);
            virtual bool sendMoteData(unsigned long keys, float acc[3], float gyro[3], int joystick[2]);

            virtual bool sendMouseMoved(int x, int y);
            virtual bool sendMouseClicked(int x, int y);
            virtual bool sendMouseDoubleClicked(int x, int y);
            virtual bool sendGestureData(int gAction, float x, float y);

            virtual bool validateTargetStatus();

            virtual bool sendShutdown();

        public:
//            static drConClientSessionImp *newOne(drConTarget* target);

        public:
            void sayHello();
            bool getHelloAnswer(std::string& who);

        public:
            drConClientSessionImp(const drConTargetImp& target);
            virtual ~drConClientSessionImp();

        private:
            drConTargetImp mTarget;
            drConSessionImpBase mImp;

            drTime mCheckTime;
            bool mTargetMissing;
    };
} // end namespace dragonBoat
#endif //__DR_CON_CLIENT_SESSION_IMP_H__
