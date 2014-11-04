#include "drConServerSessionImp.h"

#include "drConCommandAnsHello.h"
#include "drConCommandKey.h"
#include "drConCommand3D.h"
#include "drConCommandLogin.h"
#include "drConCommandLogout.h"
#include "drConCommandAnsLogin.h"
#include "drConCommandMoteState.h"
#include "drConCommandMoteStates.h"
#include "drConCommandMouse.h"
#include "drConCommandEcho.h"
#include "drConCommandGesture.h"
#include "drConCommandCoolconEvent.h"
#include "drConTargetImp.h"
#include "drConLogger.h"

#include "zmq.h"
#include "CoolconMsg.pb.h"
#include "ComprManager.h"

#if defined(__ANDROID__) || defined(ANDROID)
#include "LauncherJni.h"
#include "screencap.h"
#endif // only for android platform

#include <stdio.h>
#include <sstream>
#include <errno.h>

namespace dragonBoat {
    // log message buffer
    typedef void (*SCREENCAPINIT) ();
    typedef void (*SCREENCAPCLOSE) ();
    typedef int (*GETDATA)( ScreencapData* );

    drConServerSessionImp::drConServerSessionImp(const drConTargetImp& target)
        : mImp(target.getType(), true)
//          , mTarget(target)
          , mZmqContext(0)
          , mServerEp(0)
          , mScreenCap(0)
          , mScreenWidth(0)
          , mScreenHeight(0)
          , mTargetWidth(0)
          , mTargetHeight(0)
          , mScratchAddress(drConTransmitterDiagram::kServerPort)
          , mHandler(0)
          , mPacketSequence(0)
          , mCapIntrvl(0)
          , mDoCapture(false)
    {
//        ScreencapInit();
        memset(&mScreenBuf, 0, sizeof(mScreenBuf));
    }

    drConServerSessionImp::~drConServerSessionImp()
    {
        shutdown();

#if defined(__ANDROID__) || defined(ANDROID)
        if (mScreenCap)
        {
            SCREENCAPCLOSE scclose = (SCREENCAPCLOSE)LauncherJni::getInstance()
                ->getModuleProc(mScreenCap, "ScreencapClose");
            if (scclose)
            {
                scclose();
                drCon::log(__FUNCTION__, "ScreencapClose called.");
            }
            LauncherJni::getInstance()->unloadModule(mScreenCap);
            mScreenCap = 0;
        }
#endif
//        ScreencapClose();
    }

    //drConTarget* drConServerSessionImp::getTarget()
    //{
    //    if (mImp.notNull())
    //    {
    //        return mImp->getTarget();
    //    }
    //
    //    return NULL;
    //}
    bool drConServerSessionImp::startup()
    {
#if defined(__ANDROID__) || defined(ANDROID)
        std::string moduleName = std::string("libscreencap.")
            + LauncherJni::getInstance()->getReleaseName() + ".so";
        mScreenCap = LauncherJni::getInstance()->loadModule(moduleName);
        SCREENCAPINIT scinit = (SCREENCAPINIT)LauncherJni::getInstance()
            ->getModuleProc(mScreenCap, "ScreencapInit");
        if (scinit)
        {
            scinit();
            drCon::log(__FUNCTION__, "ScreencapInit called.");
        }
        else
        {
            drCon::log(__FUNCTION__, "ScreencapInit not called.");
        }
#endif // only for android

        do {
        if ( !mZmqContext )
        {
            mZmqContext = zmq_ctx_new();
            if ( !mZmqContext )
            {
                snprintf(stream_buf, sizeof(stream_buf), "zmq_ctx_new error: %s", zmq_strerror(errno));
                drCon::log(__FUNCTION__, stream_buf);
                break;
            }
        }
        mServerEp = zmq_socket(mZmqContext, ZMQ_ROUTER);
        if ( !mServerEp )
        {
            snprintf(stream_buf, sizeof(stream_buf), "zmq_socket error: %s", zmq_strerror(errno));
            drCon::log(__FUNCTION__, stream_buf);
            break;
        }
        int optvalue = 0, rc = 0;
        rc = zmq_setsockopt(mServerEp, ZMQ_LINGER, &optvalue, sizeof(optvalue));
        if ( 0 != rc )
        {
            snprintf(stream_buf, sizeof(stream_buf), "zmq_setsockopt error: %s", zmq_strerror(errno));
            drCon::log(__FUNCTION__, stream_buf);
            break;
        }
        // TRANS:8726
        rc = zmq_bind(mServerEp, "tcp://*:8726");
        if ( 0 != rc )
        {
            snprintf(stream_buf, sizeof(stream_buf), "zmq_bind error: %s", zmq_strerror(errno));
            drCon::log(__FUNCTION__, stream_buf);
            break;
        }
        } while (0);

        return mImp.startup();
    }

    void drConServerSessionImp::shutdown()
    {
        mImp.shutdown();
        mTargets.clear();
        mIDs.clear();

        // release zmq resources
        if ( mServerEp )
        {
            zmq_close(mServerEp);
            mServerEp = 0;
        }
        if ( mZmqContext )
        {
            zmq_ctx_term(mZmqContext);
            mZmqContext = 0;
        }
    }

    bool drConServerSessionImp::workable()
    {
        return mImp.workable();
    }

    void drConServerSessionImp::process()
    {
        if ( !workable() )
            return;

        drTime now;

        // check target last packet time and dump non-active client
        updateTargetsTime(now);
        drConCommand * pcmd = 0;

        int pktcount = 0;

        do {
        drConPacketClosure pkt = mImp.readAnyPacket(mScratchAddress);

        pcmd = pkt.getCommand();
        if ( !pcmd )
            break;
        pktcount++;
//            return;

        mPacketSequence = pcmd->getSequence();
//        bool valid_addr = mTarget.peekAddress().equalTo(mScratchAddress);
        unsigned int targetID = 0;
        bool valid_addr = getTargetID(mScratchAddress, targetID);

        // update client target's last packet time.
        if ( valid_addr )
        {
            snprintf(stream_buf, sizeof(stream_buf), "update addr: %s", mScratchAddress.toString().c_str());
            // drCon::log(__FUNCTION__, stream_buf);
        }

        switch ( pcmd->getType() )
        {
            case drCT_KEY:
                drCon::log("zyguo", "on drCT_KEY");
                if (valid_addr)
                {
                    drConCommandKey *key = (drConCommandKey *)pcmd;
                    if ( mHandler )
                        mHandler->onKeyData(targetID, key->getKey(), key->getStatus());

                    mTargets[targetID].lastTime = now;
                }
                break;

            case drCT_ACC:
                if (valid_addr)
                {
                    drConCommandAcc *acc = (drConCommandAcc *)pcmd;
                    if ( mHandler )
                        mHandler->onAccData(targetID, acc->getX(), acc->getY(), acc->getZ());

                    mTargets[targetID].lastTime = now;
                }
                break;

            case drCT_GYRO:
                if (valid_addr)
                {
                    drConCommandGyro *gyro = (drConCommandGyro *)pcmd;
                    if ( mHandler )
                        mHandler->onAccData(targetID, gyro->getX(), gyro->getY(), gyro->getZ());

                    mTargets[targetID].lastTime = now;
                }
                break;

            case drCT_MOTE_STATE:
                if (valid_addr)
                {
                    drConCommandMoteState *mote = (drConCommandMoteState *)pcmd;
                    drConVector vacc = mote->getAcc();
                    float acc[3] = {vacc.x, vacc.y, vacc.z};
                    drConVector vgyro = mote->getGyro();
                    float gyro[3] = {vgyro.x, vgyro.y, vgyro.z};
                    if ( mHandler )
                        mHandler->onMoteData(targetID, mote->getKey().getKeys(), acc, gyro);

                    mTargets[targetID].lastTime = now;
                }
                break;

            case drCT_MOTE_STATES:
                if (valid_addr)
                {
                    drConCommandMoteStates *mote = (drConCommandMoteStates *)pcmd;
                    drConVector vacc = mote->getAcc();
                    float acc[3] = {vacc.x, vacc.y, vacc.z};
                    drConVector vgyro = mote->getGyro();
                    float gyro[3] = {vgyro.x, vgyro.y, vgyro.z};
                    drConJoystick vjoystick = mote->getJoystick();
                    int joystick[2] = {vjoystick.joy_x, vjoystick.joy_y};
                    if ( mHandler )
                        mHandler->onMoteData(targetID, mote->getKey().getKeys(), acc, gyro, joystick);

                    mTargets[targetID].lastTime = now;
                }
                break;

            case drCT_MOUSE:
                if (valid_addr)
                {
                    drConCommandMouse *mouse = (drConCommandMouse *)pcmd;
                    drMouseAction action = mouse->getMouseAction();
                    if ( mHandler )
                    {
                        switch ( action )
                        {
                            case drMA_MOVED:
                                mHandler->onMouseMoved(targetID, mouse->getMouseX(), mouse->getMouseY());
                                break;
                            case drMA_CLICKED:
                                mHandler->onMouseClicked(targetID, mouse->getMouseX(), mouse->getMouseY());
                                break;
                            case drMA_DOUBLECLICKED:
                                mHandler->onMouseDoubleClicked(targetID, mouse->getMouseX(), mouse->getMouseY());
                                break;
                            default:
                                break;
                        }
                    }

                    mTargets[targetID].lastTime = now;
                }
                break;
            case drCT_SAY_HELLO:
                {
                    answerHello(mScratchAddress);
                }
                break;
            case drCT_LOGIN:
                {
                    drConCommandLogin *login = (drConCommandLogin *)pcmd;
                    answerLogin(*login, mScratchAddress);
                }
                break;
            case drCT_LOGOUT:
                {
                    drConCommandLogout *logout = (drConCommandLogout *)pcmd;
                    answerLogout(*logout, mScratchAddress);
                }
                break;
            case drCT_SHUTDOWN:
                if (valid_addr)
                {
                    if ( mHandler )
                        mHandler->onShutdown();
                }
                break;
            case drCT_GESTURE:
                if (valid_addr)
                {
                    drConCommandGesture *gesture = (drConCommandGesture *)pcmd;
                    if ( mHandler )
                    {
                        drConGestureEvent evt;
                        drConGestureResult result;

                        evt.mAction = gesture->getGestureAction();
                        evt.mEventX = gesture->getGestureX();
                        evt.mEventY = gesture->getGestureY();
                        evt.mScreenWidth = gesture->getWidth();
                        evt.mScreenHeight = gesture->getHeight();

                        mHandler->onGestureData(targetID, evt, result);

                        mDoCapture = result.mScreenCap;
                        snprintf(stream_buf, sizeof(stream_buf), "mDoCapture set to %d", mDoCapture ? 1 : 0);
                        drCon::log(stream_buf);
                    }
                    mTargets[targetID].lastTime = now;
                }
                break;
            case drCT_COOLCON_EVENT:
                if (valid_addr)
                {
                    drConCommandCoolconEvent *event = (drConCommandCoolconEvent *)pcmd;
                    if ( mHandler )
                    {
                        mHandler->onCoolconEvent(targetID,
                                event->getCoolconEvent());
                    }
                    mTargets[targetID].lastTime = now;
                }
                break;

            default:
                break;
        } // end switch
        }
        while (1);

        // process zmq messages
        if ( mServerEp )
        {
            zmq_pollitem_t items[] = {
                {mServerEp, 0, ZMQ_POLLIN, 0},
            };
            int rc = zmq_poll( items, sizeof(items) / sizeof(zmq_pollitem_t), 10);
            if (rc > 0 && items[0].revents & ZMQ_POLLIN)
            {
                zmq_msg_t msg;
                CoolconMsg msg_pb;
                std::string strmsg;
                std::string msgid;

                // receive packet
                while (1)
                {
                    zmq_msg_init(&msg);
                    zmq_msg_recv(&msg, mServerEp, ZMQ_DONTWAIT);
                    std::string strtmp((const char*)zmq_msg_data(&msg), zmq_msg_size(&msg));

                    if ( msgid.empty() )
                        msgid = strtmp;
                    else
                        strmsg += strtmp;
                    //strmsg += strtmp;

                    if ( !zmq_msg_more(&msg) )
                    {
                        zmq_msg_close(&msg);
                        break;
                    }
                    zmq_msg_close(&msg);
                } // end while loop

                // send packet

                msg_pb.ParseFromString(strmsg);
                if ( msg_pb.has_sinfo() )
                {
                    mTargetWidth = msg_pb.sinfo().width();
                    mTargetHeight = msg_pb.sinfo().height();
                }

                snprintf(stream_buf, sizeof(stream_buf), "recv msg: %s", msg_pb.DebugString().c_str());
                drCon::log(stream_buf);
//            }
//            else if (rc > 0 && items[0].revents & ZMQ_POLLOUT)
//            {

                // prepare msg
                strmsg.clear();
                if ( msg_pb.type() == 1
                        && msg_pb.name() == "GET SCREEN CAP"
//                        && mDoCapture
                   )
                {
                    drCon::log("mDoCapture is true");
                    doCaptureScreen(strmsg);
                    mDoCapture = false;
                }
                else
                {
                    drCon::log("mDoCapture is false");
                }

                //zmq_msg_t msg;
                // send id
                zmq_msg_init_size(&msg, msgid.size());
                memcpy(zmq_msg_data(&msg), msgid.data(), msgid.size() * sizeof(std::string::value_type));
                zmq_msg_send(&msg, mServerEp, ZMQ_SNDMORE);
                zmq_msg_close(&msg);
                // send empty delimiter
                zmq_msg_init(&msg);
                zmq_msg_send(&msg, mServerEp, ZMQ_SNDMORE);
                zmq_msg_close(&msg);
                // send body data
                zmq_msg_init_size(&msg, strmsg.size());
                memcpy(zmq_msg_data(&msg), strmsg.data(), strmsg.size() * sizeof(std::string::value_type));
                zmq_msg_send(&msg, mServerEp, ZMQ_DONTWAIT);
                zmq_msg_close(&msg);
            }
            else if (rc < 0)
            {
                snprintf(stream_buf, sizeof(stream_buf), "zmq_poll error: %s", zmq_strerror(errno));
                drCon::log(stream_buf);
            }
            else
            {
//                drCon::log("zmq_poll recv timeout.");
            }
        }

        snprintf(stream_buf, sizeof(stream_buf), "process packet count: %d", pktcount);
//        drCon::log(stream_buf);
    }
    void drConServerSessionImp::doCaptureScreen(std::string& strmsg)
    {
        // calculation of crop image
        int width = mScreenWidth;
        int height = mScreenHeight;
        int newWidth = mTargetWidth;
        int newHeight = mTargetHeight;
        int quality = 10;

        float fScale = ((float) newHeight) / height;
        int startx, starty, mappedWidth, mappedHeight;
        mappedHeight = height;
        mappedWidth = (int)(newWidth / fScale);
        startx = (width  - mappedWidth) >> 1;
        starty = 0;

        //snprintf(stream_buf, sizeof(stream_buf), "origin image: %d, %d, %d, %d",
        //        width, height, newWidth, newHeight);
        //drCon::log(__FUNCTION__, stream_buf);

        //snprintf(stream_buf, sizeof(stream_buf), "crop image: %d, %d, %d, %d",
        //        startx, starty, mappedWidth, mappedHeight);
        //drCon::log(__FUNCTION__, stream_buf);

        uint32_t w, h, f, size;
        const void * pbuf = 0;

#if defined(__ANDROID__) || defined(ANDROID)
        GETDATA scgetdata = (GETDATA)LauncherJni::getInstance()
            ->getModuleProc(mScreenCap, "GetScreencapData");
        if ( scgetdata
//                && mTimer.elapsed() > mCapIntrvl
           )
        {
            // reset call screencap timer
            mTimer.reset();
            ScreencapData scdata;
            memset(&scdata, 0, sizeof(scdata));
            scdata.x = startx;
            scdata.y = starty;
            scdata.width = mappedWidth;
            scdata.height = mappedHeight;
            // 0: raw data 0, 1: jpeg data
            scdata.format = 1;
            scdata.quality = quality;
            int rc = scgetdata( &scdata );

            snprintf(stream_buf, sizeof(stream_buf),
                    "GetScreencapData called, screensize, %d, %d, %d, %d",
                    scdata.width, scdata.height, scdata.format, scdata.size);
            drCon::log(__FUNCTION__, stream_buf);

            if ( 0 == rc )
            {
                // store the buff data
                mScreenBuf.pbuf = scdata.data;
                mScreenBuf.w = scdata.width;
                mScreenBuf.h = scdata.height;
                mScreenBuf.f = scdata.format;
                mScreenBuf.size = scdata.size;
            }

            unsigned intrvl = mTimer.elapsed();
            if (intrvl > mCapIntrvl)
                mCapIntrvl = intrvl;
        }
        else
        {
            snprintf(stream_buf, sizeof(stream_buf),
                    "scgetdata: %p, elapsed() : %d, mCapIntrvl: %d", scgetdata, mTimer.elapsed(), mCapIntrvl);
            drCon::log(__FUNCTION__, stream_buf);
        }
//        else
//        {
//            pbuf = mScreenBuf.pbuf;
//            w = mScreenBuf.w;
//            h = mScreenBuf.h;
//            f = mScreenBuf.f;
//            size = mScreenBuf.size;
//        }
        //GetScreencapData(w, h, f, size);
#endif // only for android
        CoolconMsg msg_pb;
        msg_pb.set_type(2);
        msg_pb.set_name("CAP DATA");

        if ( mScreenBuf.pbuf )
        {
            // compress data
            std::string infodata;
            bool bcompr = false;
            //bcompr = TM::ComprManager::compressData(pbuf, size, infodata);
            // encode message
            msg_pb.mutable_info()->set_encoded( 0 != mScreenBuf.f );
            msg_pb.mutable_info()->set_width(mScreenBuf.w);
            msg_pb.mutable_info()->set_height(mScreenBuf.h);
            msg_pb.mutable_info()->set_format(mScreenBuf.f);
            msg_pb.mutable_info()->set_size(mScreenBuf.size);
            if (bcompr)
            {
                snprintf(stream_buf, sizeof(stream_buf), "new size: %lu", (unsigned long int)infodata.size());
                drCon::log(__FUNCTION__, stream_buf);

                msg_pb.mutable_info()->set_compressed(true);
                msg_pb.mutable_info()->set_data(infodata);
            }
            else
            {
                msg_pb.mutable_info()->set_compressed(false);
                msg_pb.mutable_info()->set_data(mScreenBuf.pbuf, mScreenBuf.size);
            }

        }

        msg_pb.SerializeToString(&strmsg);
    }

    void drConServerSessionImp::answerHello(const drConTransmitDiagramAddress& addr)
    {
        std::string name(mHandler ? mHandler->getName() : "");
//        mImp.sendPacket(drConCommandAnsHello(name), addr, true);
        mImp.sendPacket(drConCommandAnsHello(name), addr, false);
    }

    void drConServerSessionImp::answerLogin(const drConCommandLogin& login, const drConTransmitDiagramAddress& addr)
    {
        std::string name = login.getUserName();
        std::string pin = login.getPassword();

        bool auth = false;
        if ( mHandler )
            auth = mHandler->onAuth(name, pin);

        drConCommandAnsLogin ans(auth);
        ans.setRequest( login.getSequence() );
//        mImp.sendPacket(ans, addr, true);
        mImp.sendPacket(ans, addr, false);

        if (auth)
        {
            unsigned int id;
            if ( !getTargetID(addr, id) )
            {
                // new client id, insert client target into target list
                unsigned int id = newTargetID();
                SClientTarget clientTarget;
                clientTarget.target = drConTargetImp(name, addr);
                mTargets.insert(
                        std::pair<TTargets::key_type, TTargets::mapped_type>(id, clientTarget)
                        );

                snprintf(stream_buf, sizeof(stream_buf), "client %s connected.", addr.toString().c_str());
                drCon::log(__FUNCTION__, stream_buf);
            }
            else
            {
                // client target already in the target list, no need to insert target again
                snprintf(stream_buf, sizeof(stream_buf), "%s already in target list.", addr.toString().c_str());
                drCon::log(__FUNCTION__, stream_buf);
            }
        }
    }
    void drConServerSessionImp::answerLogout(const drConCommandLogout& logout, const drConTransmitDiagramAddress& addr)
    {
        TTargets::iterator citr;
        for(citr = mTargets.begin(); citr != mTargets.end(); )
        {
            if ( addr.equalTo(citr->second.target.peekAddress()) )
            {
                snprintf(stream_buf, sizeof(stream_buf), "client %s logout."
                        , citr->second.target.peekAddress().toString().c_str());
                drCon::log(__FUNCTION__, stream_buf);
                reclaimTargetID(citr->first);
                mTargets.erase(citr);
                citr = mTargets.begin();
            }
            else
            {
                ++citr;
            }
        } // end for
    }

    void drConServerSessionImp::kickClient()
    {
        // kick every clients to make them alive
        TTargets::const_iterator citr;
        for (citr = mTargets.begin(); citr != mTargets.end(); ++citr)
            mImp.sendPacket(drConCommandEcho(), citr->second.target.peekAddress());
    }

    // helper functions
    bool drConServerSessionImp::getTargetID(const drConTransmitDiagramAddress& addr, unsigned int& id) const
    {
        bool bResult = false;
        TTargets::const_iterator citr;
        unsigned int index = 0;
        for(citr = mTargets.begin(); citr != mTargets.end(); ++citr, ++index)
        {
            if ( addr.equalTo(citr->second.target.peekAddress()) )
            {
                id = citr->first;
                bResult = true;
            }
        }
        return bResult;
    }
    // get new id from ids pool(mIDs)
    unsigned int drConServerSessionImp::newTargetID()
    {
        unsigned int id = 0;
        if ( mIDs.empty() )
        {
            id = mTargets.size();
        }
        else
        {
            TIDs::iterator citr;
            citr = mIDs.begin();
            id = *citr;
            mIDs.erase(citr);
        }
        dumpIDspool();
        return id;
    }
    // put id into ids pool(mIDs)
    void drConServerSessionImp::reclaimTargetID(unsigned int id)
    {
        mIDs.insert(id);
        dumpIDspool();
    }
    // helper function to dump ids pool
    void drConServerSessionImp::dumpIDspool()
    {
        std::stringstream ss;
        ss << "dump IDs pool: ";
        TIDs::const_iterator citr;
        for (citr = mIDs.begin(); citr != mIDs.end(); ++citr)
        {
            ss << *citr << ", ";
        }
        drCon::log(__FUNCTION__, ss.str().c_str());
    }

    // update target lastTime and disconnect non-active target
    void drConServerSessionImp::updateTargetsTime(const drTime& uptime)
    {
        TTargets::iterator citr = mTargets.begin();
        while ( citr != mTargets.end() )
        {
            // kick client to keep it alive
            if ( uptime.laterThan(citr->second.lastKickTime, E_KICKCLIENT_TIME) )
            {
                citr->second.lastKickTime = uptime;
                mImp.sendPacket(drConCommandEcho(), citr->second.target.peekAddress());
            }

            // check last packet time
            if ( uptime.laterThan(citr->second.lastTime, E_CLIENT_NONACTIVE_TIMEOUT) )
            {
                snprintf(stream_buf, sizeof(stream_buf), "client %s disconnected."
                        , citr->second.target.peekAddress().toString().c_str());
                drCon::log(__FUNCTION__, stream_buf);
                reclaimTargetID(citr->first);
                mTargets.erase(citr);
                citr = mTargets.begin();
            }
            else
            {
                ++citr;
            }
        } // end while
    }

    //drConServerSessionImp * drConServerSessionImp::newOne(drConTransmitType type)
    //{
    //    return new drConServerSessionImp(drConTargetImp::anyOne(type));
    //}
    void drConServerSessionImp::setScreenSize(int width, int height)
    {
        mScreenWidth = width;
        mScreenHeight = height;
    }
} // end namespace dragonBoat
