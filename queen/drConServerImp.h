#ifndef _DR_CON_SERVER_IMP_H__
#define _DR_CON_SERVER_IMP_H__

#include "pthread.h"
#include "drConServer.h"
#include "drConTransmitType.h"
#include "drConServerSessionImp.h"

namespace dragonBoat {
    class drConSessionHandlerBridge;
    class drConServerImp : public drConServer
    {
        friend class drConSessionHandlerBridge;

        public:
        virtual void setName(const std::string& name) { mName = name; }
        virtual std::string getName() const { return mName; }

        virtual bool start();
        virtual void shutdown();
        virtual void wait();

        virtual void update();

        public:
        //    static drConServerImp *newOne(drConEngine *engine, drConTransmitType type, drConServerHandler *handler);

        drConServerImp(drConTransmitType type, drConServerHandler *handler);
        virtual ~drConServerImp();

        void setNiName(const std::string& name) { mNiName = name; }
        std::string getNiName() const { return mNiName; }

        void setScreenSize(int width, int height) { mSession.setScreenSize(width, height); }

        private:
//        drAutoRef<drConEngine> mEngine;
        drConTransmitType mType;

        //drAutoRef<drConSessionHandler> mDelegate;

        // host name to display
        std::string mName;
        // Network Interface name used by zeromq
        std::string mNiName;

        std::string mCurSession;

        drConServerSessionImp mSession;
        drConServerHandler * mHandler;
        drConSessionHandlerBridge * mHandlerBridge;

//        pthread_t mThread;
//        bool mThreadValid;
//        bool mNeedStop;

        static void *thread_proc(void *arg);

        bool mNeedKickClient;
        void kickClient();
    };
} // end namespace dragonBoat
#endif //_DR_CON_SERVER_IMP_H__
