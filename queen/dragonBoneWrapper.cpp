#include "dragonBoneWrapper.h"
#include "drConLogger.h"
#include "drConSearcherImp.h"

namespace dragonBoat {
    ////////////////////////////////////////////////////////
    // implementation of drServerList
    bool drServerList::getServer(int id, drConTargetImp& target)
    {
        bool bResult = false;
        if ( id >= 1 && id <= mServers.size() )
        {
            target = mServers[id - 1];
            bResult = true;
        }
        return bResult;
    }

    void drServerList::putServer(const drConTargetImp& target)
    {
        mServers.push_back(target);
    }

    void drServerList::clear()
    {
        mServers.clear();
    }

    bool drServerList::hasThis(const drConTargetImp& target) const
    {
        drConTargets::const_iterator i = mServers.begin();
        for (; i!= mServers.end(); ++i)
        {
            if ( target.equalTo(*i) )
            {
                return true;
            }
        }

        return false;
    }

    ////////////////////////////////////////////////////////////
    // implementation of drDragonBoneWrapper
    drDragonBoneWrapper::drDragonBoneWrapper()
        : mSession(0)
          , mCurServer(0)
    {
    }
    drDragonBoneWrapper::~drDragonBoneWrapper()
    {
        clearSession();
        mServers.clear();
    }

    int drDragonBoneWrapper::getServerCount() const
    {
        return mServers.getCount();
    }

    int drDragonBoneWrapper::currentServer()
    {
        return mCurServer;
    }

    bool drDragonBoneWrapper::workable()
    {
        if ( mCurServer > 0 && mCurServer <= mServers.getCount() )
        {
            return mSession->workable();
        }

        return false;
    }

    bool drDragonBoneWrapper::sendKey(int key, int status)
    {
        if (workable())
        {
            return mSession->sendKeyData(key, status);
        }

        return false;
    }

    bool drDragonBoneWrapper::sendAcc(float x, float y, float z)
    {
        if (workable())
        {
            return mSession->sendAccData(x, y, z);
        }

        return false;
    }

    bool drDragonBoneWrapper::sendGyro(float x, float y, float z)
    {
        if (workable())
        {
            return mSession->sendGyroData(x, y, z);
        }

        return false;
    }

    bool drDragonBoneWrapper::sendMote(unsigned long keys, float acc[3], float gyro[3])
    {
        if (workable())
        {
            return mSession->sendMoteData(keys, acc, gyro);
        }

        return false;
    }

    bool drDragonBoneWrapper::sendMote(unsigned long keys, float acc[3], float gyro[3], int joystick[2])
    {
        if (workable())
        {
            return mSession->sendMoteData(keys, acc, gyro, joystick);
        }

        return false;
    }

    bool drDragonBoneWrapper::sendMouseMoved(int x, int y)
    {
        if (workable())
        {
            return mSession->sendMouseMoved(x, y);
        }

        return false;
    }

    bool drDragonBoneWrapper::sendMouseClicked(int x, int y)
    {
        if (workable())
        {
            return mSession->sendMouseClicked(x, y);
        }

        return false;
    }

    bool drDragonBoneWrapper::sendMouseDoubleClicked(int x, int y)
    {
        if (workable())
        {
            return mSession->sendMouseDoubleClicked(x, y);
        }

        return false;
    }

    bool drDragonBoneWrapper::validateTargetStatus()
    {
        if (workable())
        {
            return mSession->validateTargetStatus();
        }

        return false;
    }

    bool drDragonBoneWrapper::sendShutdown()
    {
        if (workable())
        {
            return mSession->sendShutdown();
        }

        return false;
    }

    //inline drDragonBoneWrapper *drDragonBoneWrapper::newOne()
    //{
    //    return new drDragonBoneWrapper();
    //}

    bool drDragonBoneWrapper::init(drConLoggerImpl *logger)
    {
//        mEngine.assign(newConEngine());
//        if (mEngine.notNull())
//        {
//            mEngine->setConLoggerImpl(logger);
//            return true;
//        }
//        return false;

//       drCon::setLoggerImpl(logger);
       return true;
    }

    bool drDragonBoneWrapper::searchServers(unsigned long msecs)
    {
        drConSearcherImp search(drTT_DIAGRAM);
        mServers.clear();
        TTargets targets;
        if( search.startSearch(targets, msecs) )
        {
            TTargets::const_iterator citr;
            for(citr = targets.begin(); citr != targets.end(); ++citr)
            {
                if ( !mServers.hasThis(*citr) )
                {
                    mServers.putServer(*citr);
                }
            }
            return true;
        }

        return false;
    }

    std::string drDragonBoneWrapper::getServerName(int id)
    {
        std::string strres = "";
        drConTargetImp target(drTT_DIAGRAM);
        if (mServers.getServer(id, target))
        {
            strres = target.getName();
        }

        return strres;
    }

    std::string drDragonBoneWrapper::getServerAddress(int id)
    {
        std::string str_null;
        drConTargetImp target(drTT_DIAGRAM);
        if(mServers.getServer(id, target))
        {
            drConTransmitDiagramAddress addr = target.peekAddress();
            str_null = addr.toString();
        }

        return str_null;
    }

    void drDragonBoneWrapper::clearSession()
    {
        if (mSession)
        {
            mSession->shutdown();
            delete mSession;
            mSession = 0;
        }
    }
    bool drDragonBoneWrapper::connect(int id, const std::string& name, const std::string& pin)
    {
        drConTargetImp target(drTT_DIAGRAM);
        if (mServers.getServer(id, target))
        {
            clearSession();
            mSession = new drConClientSessionImp(target);
            if ( mSession && mSession->doLogin(name, pin) )
            {
                mCurServer = id;
                return true;
            }
        }

        return false;
    }
} // end namespace dragonBoat

