#include "drConServerImp.h"

#include "drConServerSessionImp.h"
#include "drConLogger.h"
#include "drConTime.h"

namespace dragonBoat {
    ////////////////////////////////////////////////////////////////////////////////
    /// declaration and implementation of drConSessionHandlerBridge
    class drConSessionHandlerBridge : public drConSessionHandler
    {
        public:
            virtual std::string getName() const;

            virtual bool onAuth(const std::string& name, const std::string& pin);

            virtual void onKeyData(unsigned int id, int key, int status);
            virtual void onAccData(unsigned int id, float x, float y, float z);
            virtual void onGyroData(unsigned int id, float x, float y, float z);

            virtual void onMoteData(unsigned int id, unsigned long keys, float acc[3], float gyro[3]);
            virtual void onMoteData(unsigned int id, unsigned long keys, float acc[3], float gyro[3], int joystick[2]);

            virtual void onMouseMoved(unsigned int id, int x, int y);
            virtual void onMouseClicked(unsigned int id, int x, int y);
            virtual void onMouseDoubleClicked(unsigned int id, int x, int y);
            virtual void onGestureData(unsigned int id, const drConGestureEvent& evt, drConGestureResult& result);
            virtual void onCoolconEvent(unsigned int id, const std::string& evt);

            virtual void onShutdown();

        private:
            friend class drConServerImp;
            drConSessionHandlerBridge(drConServerImp *server);

        private:
            drConServerHandler * mHandler;
            drConServerImp * mServer;

            drTime mTimeLastData;
    };

    std::string drConSessionHandlerBridge::getName() const
    {
        if (mServer)
            return mServer->mName;
        return "";
    }

    bool drConSessionHandlerBridge::onAuth(const std::string& name, const std::string& pin)
    {
        if (mHandler && mHandler->onAuth(name, pin))
        {
            mHandler->onEndSession(mServer->mCurSession);
            mHandler->onNewSession(name);
            mServer->mCurSession = name;

            return true;
        }
        return false;
    }

    void drConSessionHandlerBridge::onKeyData(unsigned int id, int key, int status)
    {
        if (mHandler)
        {
            mHandler->onKeyData(id, key, status);
        }
    }

    void drConSessionHandlerBridge::onAccData(unsigned int id, float x, float y, float z)
    {
        if (mHandler)
        {
            mHandler->onAccData(id, x, y, z);
        }
    }

    void drConSessionHandlerBridge::onGyroData(unsigned int id, float x, float y, float z)
    {
        if (mHandler)
        {
            mHandler->onGyroData(id, x, y, z);
        }
    }

    void drConSessionHandlerBridge::onMoteData(unsigned int id, unsigned long keys, float acc[3], float gyro[3])
    {
        //drCon::log("drConSessionHandlerBridge::onMoteData()");
        if (mHandler)
        {
            //drCon::log("mHandler->onMoteData()...");
            mHandler->onMoteData(id, keys, acc, gyro);
        }

        drTime timeNow;
        if (timeNow.laterThan(mTimeLastData, 2500))
        {
            mTimeLastData = timeNow;
//            if (mServer)
//                mServer->kickClient();
        }
    }

    void drConSessionHandlerBridge::onMoteData(unsigned int id, unsigned long keys, float acc[3], float gyro[3], int joystick[2])
    {
        if (mHandler)
        {
            mHandler->onMoteData(id, keys, acc, gyro, joystick);
        }

        drTime timeNow;
        if (timeNow.laterThan(mTimeLastData, 2500))
        {
            mTimeLastData = timeNow;
//            if (mServer)
//                mServer->kickClient();
        }
    }

    void drConSessionHandlerBridge::onMouseMoved(unsigned int id, int x, int y)
    {
        if (mHandler)
        {
            mHandler->onMouseMoved(id, x, y);
        }
    }

    void drConSessionHandlerBridge::onMouseClicked(unsigned int id, int x, int y)
    {
        if (mHandler)
        {
            mHandler->onMouseClicked(id, x, y);
        }
    }

    void drConSessionHandlerBridge::onMouseDoubleClicked(unsigned int id, int x, int y)
    {
        if (mHandler)
        {
            mHandler->onMouseDoubleClicked(id, x, y);
        }
    }
    void drConSessionHandlerBridge::onGestureData(unsigned int id, const drConGestureEvent& evt, drConGestureResult& result)
    {
        if (mHandler)
        {
            mHandler->onGestureData(id, evt, result);
        }
    }
    void drConSessionHandlerBridge::onCoolconEvent(unsigned int id, const std::string& evt)
    {
        if (mHandler)
        {
            mHandler->onCoolconEvent(id, evt);
        }
    }

    void drConSessionHandlerBridge::onShutdown()
    {
        if (mServer && mHandler)
        {
            mHandler->onEndSession(mServer->mCurSession);

            if (mHandler->onShutdownServer())
            {
                mServer->shutdown();
            }
        }
    }

    drConSessionHandlerBridge::drConSessionHandlerBridge(drConServerImp *server)
        : mServer(server)
    {
        if (mServer)
        {
            mHandler = mServer->mHandler;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////
    /// implementation of drConServerImp

    bool drConServerImp::start()
    {
        if ( !mHandler )
            return false;

//        if ( pthread_create(&mThread, NULL, thread_proc, (void *)this) != 0 )
//            return false;
//
//        mThreadValid = true;

        mSession.setHandler(mHandlerBridge);
        mSession.startup();
        return true;
    }

    void drConServerImp::shutdown()
    {
        mSession.setHandler(0);
        mSession.shutdown();
//        mNeedStop = true;
    }

    void drConServerImp::wait()
    {
//        if (mThreadValid)
//        {
//            pthread_join(mThread, NULL);
//            mThreadValid = false;
//        }
    }

    void drConServerImp::update()
    {
        if( mSession.workable() )
        {
            mSession.process();

            if ( mNeedKickClient )
            {
//                mSession.kickClient();
                mNeedKickClient = false;
            }
        }
    }

    //drConServerImp *drConServerImp::newOne(drConEngine *engine, drConTransmitType type, drConServerHandler *handler)
    //{
    //    return new drConServerImp(engine, type, handler);
    //}

    drConServerImp::drConServerImp(drConTransmitType type, drConServerHandler *handler)
        : mType(type)
          , mName("Unnamed")
          , mSession(drConTargetImp(mType))
          , mHandler(handler)
//          , mThreadValid(false)
//          , mNeedStop(false)
          , mNeedKickClient(false)
    {
        drCon::log(__FUNCTION__, "OK - drConServerImp created.");
        mHandlerBridge = new drConSessionHandlerBridge(this);
    }

    drConServerImp::~drConServerImp()
    {
        shutdown();
        delete mHandlerBridge;
        drCon::log(__FUNCTION__, "OK - drConServerImp destoryed.");
    }

    void drConServerImp::kickClient()
    {
        mNeedKickClient = true;
    }

//    void *drConServerImp::thread_proc(void *arg)
//    {
//        drCon::log(__FUNCTION__, "drConServerImp thread entering...");
//        {
//            drConServerImp* self;
//            self = (drConServerImp*) arg;
//
//            drConServerSessionImp session(drConTargetImp(self->mType));
//
//            if (session.workable())
//            {
//                drConSessionHandlerBridge handler(self);
//
//                session.setHandler(&handler);
//
//                static drTime timeLast;
//                while(!self->mNeedStop && session.workable())
//                {
//                    session.process();
//
//                    if (self->mNeedKickClient)
//                    {
//                        session.kickClient();
//                        self->mNeedKickClient = false;
//                    }
//                }
//
//                session.shutdown();
//            }
//        }
//        drCon::log(__FUNCTION__, "OK - drConServerImp thread exits.");
//
//        return NULL;
//    }
} // end namespace dragonBoat
