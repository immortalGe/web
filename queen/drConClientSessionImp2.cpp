#include "drConClientSessionImp2.h"

#include "drConCommandSayHello.h"
#include "drConCommandAnsHello.h"
#include "drConCommandLogin.h"
#include "drConCommandAnsLogin.h"
#include "drConCommandLogout.h"
#include "drConCommandKey.h"
#include "drConCommand3D.h"
#include "drConCommandShutdown.h"
#include "drConCommandEcho.h"
#include "drConCommandAnsHello.h"
#include "drConCommandMoteState.h"
#include "drConCommandMoteStates.h"
#include "drConCommandMouse.h"
#include "drConCommandGesture.h"
#include "drConCommandCoolconEvent.h"

#include "drConTargetImp.h"
#include "drConLogger.h"

#include "zmq.h"
#include "CoolconMsg.pb.h"
#include <stdio.h>

namespace dragonBoat {
    static const char * COOLCONEVENT_SCREENCAP = "{\"action\":\"screencap\"}";

    /////////////////////////////////////////////////////////////////////////////////
    // implementation of drConClientEventQueue
    void drConClientEventQueue::prepare_events()
    {
        if ( mEvents.size() == E_QUEUE_SIZE )
        {
            drCommandEvent evt = popEvent();
            if ( evt.cmd )
                delete evt.cmd;
        }
    }
    void drConClientEventQueue::pushLogin(const drConTargetImp& target, const std::string& name, const std::string& pin)
    {
        prepare_events();
        drCommandEvent evt;
        evt.target = target;
        evt.cmd = new drConCommandLogin(name, pin);
        mEvents.push_back(evt);
    }
    void drConClientEventQueue::pushLogout(const drConTargetImp& target)
    {
        prepare_events();
        drCommandEvent evt;
        evt.target = target;
        evt.cmd = new drConCommandLogout();
        mEvents.push_back(evt);
    }

    void drConClientEventQueue::pushKey(int key, int status)
    {
        prepare_events();
        drCommandEvent evt;
        evt.cmd = new drConCommandKey(key, status);
        mEvents.push_back(evt);
    }
    void drConClientEventQueue::pushMote(int keystate, float acc[3], float gyro[3])
    {
        prepare_events();
        drCommandEvent evt;
        evt.cmd = new drConCommandMoteState();
        ((drConCommandMoteState*)evt.cmd)->setKey(drConKeyState(keystate));
        ((drConCommandMoteState*)evt.cmd)->setAcc(drConVector(acc[0], acc[1], acc[2]));
        ((drConCommandMoteState*)evt.cmd)->setGyro(drConVector(gyro[0], gyro[1], gyro[2]));
        mEvents.push_back(evt);
    }
    void drConClientEventQueue::pushMouseMoved(int x, int y)
    {
        prepare_events();
        drCommandEvent evt;
        evt.cmd = new drConCommandMouse();
        ((drConCommandMouse*)evt.cmd)->setMouseAction(drMA_MOVED);
        ((drConCommandMouse*)evt.cmd)->setMouseXY(x, y);
        mEvents.push_back(evt);
    }
    void drConClientEventQueue::pushMouseClicked(int x, int y)
    {
        prepare_events();
        drCommandEvent evt;
        evt.cmd = new drConCommandMouse();
        ((drConCommandMouse*)evt.cmd)->setMouseAction(drMA_CLICKED);
        ((drConCommandMouse*)evt.cmd)->setMouseXY(x, y);
        mEvents.push_back(evt);
    }
    void drConClientEventQueue::pushGesture(int gAction, float x, float y, int width, int height)
    {
        prepare_events();
        drCommandEvent evt;
        evt.cmd = new drConCommandGesture();
        ((drConCommandGesture*)evt.cmd)->setGestureAction(gAction);
        ((drConCommandGesture*)evt.cmd)->setGestureXY(x, y);
        ((drConCommandGesture*)evt.cmd)->setScreenSize(width, height);
        mEvents.push_back(evt);
    }
    void drConClientEventQueue::pushCoolconEvent(const std::string& event)
    {
        prepare_events();
        drCommandEvent evt;
        evt.cmd = new drConCommandCoolconEvent();
        ((drConCommandCoolconEvent*)evt.cmd)->setCoolconEvent(event);
        mEvents.push_back(evt);
    }

    // pop the event from the queue
    // return 0 if no event in the queue
    // event object should be delete by
    // user.
    drCommandEvent drConClientEventQueue::popEvent()
    {
        drCommandEvent evt;
        if ( !mEvents.empty() )
        {
            evt = mEvents.front();
            mEvents.pop_front();
        }
        return evt;
    }
    void drConClientEventQueue::clear()
    {
        mEvents.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////
    // implementation of drConClientSessionImp2
    drConClientSessionImp2::drConClientSessionImp2()
        : mLoginSequence(0)
          , mSessionState(E_SESSIONSTATE_DISCONNECTED)
//          , mTargetMissing(true)
          , mImp(drTT_DIAGRAM, false)
          , mTargetsChanged(false)
          , mZmqContext(0)
          , mSenderEp(0)
          , mpCoolconMsg(0)
    {
        drCon::log(__FUNCTION__, "constructing drConClientSessionImp2.");
    }

    drConClientSessionImp2::drConClientSessionImp2(const drConTargetImp& target)
        : mLoginSequence(0)
          , mSessionState(E_SESSIONSTATE_DISCONNECTED)
//          , mTargetMissing(true)
          , mTarget(target)
          , mImp(target.getType(), false)
          , mTargetsChanged(false)
          , mZmqContext(0)
          , mSenderEp(0)
          , mpCoolconMsg(0)
    {
        drCon::log(__FUNCTION__, "constructing drConClientSessionImp2.");
    }

    drConClientSessionImp2::~drConClientSessionImp2()
    {
        drCon::log(__FUNCTION__, "destroyed drConClientSessionImp2.");
        shutdown();
    }

    bool drConClientSessionImp2::startup()
    {
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
        } while (0);

        return mImp.startup();
    }

    void drConClientSessionImp2::shutdown()
    {
        disconnect();
        update();
//        process_recvbuf();
//        process_sendbuf();

        mTargetsChanged = false;
        mAvailTargets.clear();
        mEvents.clear();
        mImp.shutdown();
        releaseCoolconMsg();
    }

    bool drConClientSessionImp2::workable()
    {
        return mImp.workable();
    }

    bool drConClientSessionImp2::sendKeyData(int key, int status)
    {
        mEvents.pushKey(key, status);
        return true;
    }

    bool drConClientSessionImp2::sendMoteData(unsigned long keys, float acc[3], float gyro[3])
    {
        mMoteState.setKey( drConKeyState(keys) );
        mMoteState.setAcc( drConVector(acc[0], acc[1], acc[2]) );
        mMoteState.setGyro( drConVector(gyro[0], gyro[1], gyro[2]) );

        return true;
    }

    bool drConClientSessionImp2::sendMoteData(unsigned long keys, float acc[3], float gyro[3], int joystick[2])
    {
        return true;
    }

    bool drConClientSessionImp2::sendMouseMoved(int x, int y)
    {
        mEvents.pushMouseMoved(x, y);
        return true;
    }

    bool drConClientSessionImp2::sendMouseClicked(int x, int y)
    {
        mEvents.pushMouseClicked(x, y);
        return true;
    }

    bool drConClientSessionImp2::sendMouseDoubleClicked(int x, int y)
    {
        return true;
    }

    bool drConClientSessionImp2::sendGestureData(int gAction, float x, float y, int width, int height)
    {
        drCon::log("zyguo:drConClientSessionImp2::sendGestureData");
        mEvents.pushGesture(gAction, x, y, width, height);
        return true;
    }

    bool drConClientSessionImp2::sendCoolconEvent(const std::string& evt)
    {
        mEvents.pushCoolconEvent(evt);
        return true;
    }

    bool drConClientSessionImp2::_connect(const drConTargetImp& target 
            , const std::string& name, const std::string& pin)
    {
        // do the normal login
        // drCon::log(__FUNCTION__, "mSessionState = E_SESSIONSTATE_CONNECTING");
        mSessionState = E_SESSIONSTATE_CONNECTING;
        mLoginTime = drTime();
        mTarget = target;
        mEvents.pushLogin(mTarget, name, pin);

        // zmq_connect
        // TRANS:8726
        if ( !mSenderEp )
            mSenderEp = zmq_socket(mZmqContext, ZMQ_REQ);

        if ( !mSenderEp )
        {
            snprintf(stream_buf, sizeof(stream_buf), "zmq_socket error: %s", zmq_strerror(errno));
            drCon::log(__FUNCTION__, stream_buf);
            return false;
        }
        int optvalue = 0, rc = 0;
        rc = zmq_setsockopt(mSenderEp, ZMQ_LINGER, &optvalue, sizeof(optvalue));
        if ( 0 != rc )
        {
            snprintf(stream_buf, sizeof(stream_buf), "zmq_setsockopt error: %s", zmq_strerror(errno));
            drCon::log(__FUNCTION__, stream_buf);
            return false;
        }

        std::string addr = target.peekAddress().toString();
        addr = addr.substr(0, addr.find_first_of(':'));
        addr = std::string("tcp://") + addr + ":8726";
        snprintf(stream_buf, sizeof(stream_buf), "connect server: %s", addr.c_str());
        drCon::log(__FUNCTION__, stream_buf);

        rc = zmq_connect(mSenderEp, addr.c_str());
        if ( 0 != rc )
        {
            snprintf(stream_buf, sizeof(stream_buf), "zmq_connect error: %s", zmq_strerror(errno));
            drCon::log(__FUNCTION__, stream_buf);
            return false;
        }

        return true;
    }

    // login action
    bool drConClientSessionImp2::connect(const std::string& targetaddr, const std::string& targetname
            , const std::string& name, const std::string& pin)
    {
        snprintf(stream_buf, sizeof(stream_buf),
                "connect target: [%s, %s]", targetname.c_str(), targetaddr.c_str());
        drCon::log(__FUNCTION__, stream_buf);

        // check current state
        switch (mSessionState)
        {
            case E_SESSIONSTATE_DISCONNECTED:
            case E_SESSIONSTATE_BROKEN:
                _connect(drConTargetImp(targetname, drConTransmitDiagramAddress(targetaddr))
                        , name, pin);
                break;
            case E_SESSIONSTATE_CONNECTED:
                if (mTarget.peekAddress().toString() != targetaddr)
                {
                    // disconnect previous one and do normal login
                    _disconnect(E_SESSIONSTATE_DISCONNECTED);
                    _connect(drConTargetImp(targetname, drConTransmitDiagramAddress(targetaddr))
                            , name, pin);
                }
                else
                {
                    // connection is made, do nothing.
                    drCon::log(__FUNCTION__, "Session already connected...");
                }
                break;
            case E_SESSIONSTATE_CONNECTING:
                drCon::log(__FUNCTION__, "Session in connecting...");
                break;
            default:
                drCon::log(__FUNCTION__, "mSessionState Oops !");
                break;
        }
        return true;
    }
    void drConClientSessionImp2::_disconnect(ESessionState sess_state)
    {
        // drCon::log(__FUNCTION__, "mSessionState = E_SESSIONSTATE_DISCONNECTED");
        mSessionState = sess_state;
        mEvents.pushLogout(mTarget);

        // release zeromq socket
        if ( mSenderEp )
        {
            zmq_close(mSenderEp);
            mSenderEp = 0;
        }
    }
    // logout action
    void drConClientSessionImp2::disconnect()
    {
        if ( E_SESSIONSTATE_CONNECTED == mSessionState )
        {
            snprintf(stream_buf, sizeof(stream_buf),
                    "disconnect target: [%s, %s]", mTarget.getName().c_str(), mTarget.peekAddress().toString().c_str());
            drCon::log(__FUNCTION__, stream_buf);

            // do disconnect work.
            _disconnect(E_SESSIONSTATE_DISCONNECTED);
        }
    }

    // single-step update for session loop
    void drConClientSessionImp2::process_recvbuf()
    {
        drTime now;

        // check current status
        if ( !workable() )
            return;

        // reset targets changed status
        mTargetsChanged = false;

        // 1. update current session state
        // update current state based on time
        if ( E_SESSIONSTATE_CONNECTING == mSessionState
                && now.laterThan(mLoginTime, E_LOGIN_TIMEOUT) )
        {
            drCon::log(__FUNCTION__, "Login timeout, mSessionState = E_SESSIONSTATE_BROKEN");
            // mSessionState = E_SESSIONSTATE_DISCONNECTED;
            _disconnect(E_SESSIONSTATE_BROKEN);
        }
        if ( E_SESSIONSTATE_CONNECTED == mSessionState
                && now.laterThan(mUpdateTime, E_CONNECTION_TIMEOUT) )
        {
            drCon::log(__FUNCTION__, "ECHO timeout, mSessionState = E_SESSIONSTATE_BROKEN");
            // mSessionState = E_SESSIONSTATE_DISCONNECTED;
            _disconnect(E_SESSIONSTATE_BROKEN);
        }

        TServerTargets::iterator itr = itr = mAvailTargets.begin();
        while ( itr != mAvailTargets.end() )
        {
            if ( now.laterThan(itr->second.uptime, E_SERVERALIVE_TIME) )
            {
                mTargetsChanged = true;
                mAvailTargets.erase(itr);
                itr = mAvailTargets.begin();
            }
            else
            {
                // careful...
                ++itr;
            }
        }

        // 2. update session read packet
        // receive packets type: drCT_ANS_HELLO, drCT_ANS_LOGIN, drCT_ECHO
        drConTransmitDiagramAddress scratchAddress;
        drConCommand * pcmd = 0;
        do {
        drConPacketClosure pkt = mImp.readAnyPacket(scratchAddress);
        pcmd = pkt.getCommand();
        // process all packets in buffer
        if ( !pcmd )
            break;

        switch ( pcmd->getType() )
        {
            case drCT_ANS_HELLO:
                {
                    // save server to server list
                    drConCommandAnsHello * pAnsHello = (drConCommandAnsHello*)pcmd;
                    TServerTargets::iterator itr = mAvailTargets.find(scratchAddress);
                    if ( itr == mAvailTargets.end() )
                    {
                        mTargetsChanged = true;
                        mAvailTargets.insert(
                                std::pair<TServerTargets::key_type, TServerTargets::mapped_type>(
                                    scratchAddress, STargetInfo(pAnsHello->getName(), now)
                                    )
                                );
                    }
                    else
                    {
                        itr->second.name = pAnsHello->getName();
                        itr->second.uptime = now;
                    }

                    // for old-server of drConServerSession, update the ECHO time.
                    if ( E_SESSIONSTATE_CONNECTED == mSessionState
                            && scratchAddress.equalTo(mTarget.peekAddress()) )
                    {
                        drCon::log(__FUNCTION__, "update ECHO time");
                        mUpdateTime = now;
                    }

                    break;
                }
            case drCT_ANS_LOGIN:
                {
                    // process login procedure
                    // check state/address/sequence number
                    if ( E_SESSIONSTATE_CONNECTING == mSessionState
                            && scratchAddress.equalTo(mTarget.peekAddress())
                            && mLoginSequence == pcmd->getRequest().getValue() )
                    {
                        drCon::log(__FUNCTION__, "mSessionState = E_SESSIONSTATE_CONNECTED");
                        mSessionState = E_SESSIONSTATE_CONNECTED;
                        mUpdateTime = now;
                    }

                    break;
                }
            case drCT_ECHO:
                {
                    // process server-alive message
                    mUpdateTime = now;
                    break;
                }
            default:
                snprintf(stream_buf, sizeof(stream_buf), "Opps, unknown command type: %d", pcmd->getType());
                drCon::log(__FUNCTION__, stream_buf);
                break;
        } // end switch
        } while (1);

        // receive zmq messages
        if ( mSenderEp )
        {
            zmq_pollitem_t items[] = {
                {mSenderEp, 0, ZMQ_POLLIN, 0},
            };
            int rc = zmq_poll( items, sizeof(items) / sizeof(zmq_pollitem_t), 10);
            if (rc > 0 && items[0].revents & ZMQ_POLLIN)
            {
                zmq_msg_t msg;
                //CoolconMsg msg_pb;
                std::string strmsg;

                while (1)
                {
                    zmq_msg_init(&msg);

                    zmq_msg_recv(&msg, mSenderEp, ZMQ_DONTWAIT);
                    std::string strtmp((const char*)zmq_msg_data(&msg), zmq_msg_size(&msg));
                    strmsg += strtmp;

                    if ( !zmq_msg_more(&msg) )
                    {
                        zmq_msg_close(&msg);
                        break;
                    }
                    zmq_msg_close(&msg);
                }

                releaseCoolconMsg();
                mpCoolconMsg = new CoolconMsg;
                mpCoolconMsg->ParseFromString(strmsg);

//                if ( msg_pb.has_info() )
//                {
//                    snprintf(stream_buf, sizeof(stream_buf), "recv image info: %d, %d, %d, %d, %d"
//                            , msg_pb.info().width()
//                            , msg_pb.info().height()
//                            , msg_pb.info().format()
//                            , msg_pb.info().size()
//                            , msg_pb.info().compressed()
//                            );
//                    drCon::log(stream_buf);
//                }
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
    }
    void drConClientSessionImp2::process_sendbuf()
    {
        drTime now;
        // 3. update session send packet

        drConCommand * pcmd = 0;
        // send packets in the event queue
        while ( !mEvents.empty() )
        {
            drCommandEvent evt = mEvents.popEvent();
            switch ( evt.cmd->getType() )
            {
                case drCT_LOGIN:
                    {
                        drCon::log(__FUNCTION__, "sending login packet");
                        mImp.sendPacket(*evt.cmd, evt.target.peekAddress());
                        mLoginSequence = evt.cmd->getSequence().getValue();
                        break;
                    }
                case drCT_LOGOUT:
                    {
                        drCon::log(__FUNCTION__, "sending logout packet");
                        mImp.sendPacket(*evt.cmd, evt.target.peekAddress());
                        break;
                    }
                case drCT_KEY:
                case drCT_MOUSE:
                case drCT_GESTURE:
                case drCT_COOLCON_EVENT:
                    // drCon::log("zyguo:sendbuffdrCT_GAME_CONTROL");
                    if ( E_SESSIONSTATE_CONNECTED == mSessionState )
                    {
                        mImp.sendPacket(*evt.cmd, mTarget.peekAddress());

//                        if ( evt.cmd->getType() == drCT_MOUSE
//                                && ((drConCommandMouse*)evt.cmd)->getMouseAction() == drMA_CLICKED
//                                && mSenderEp )
                        if ( evt.cmd->getType() == drCT_GESTURE
                                && ((drConCommandGesture*)evt.cmd)->getGestureAction() == drGA_SINGLETAP
                                && mSenderEp )
                        {
                            // test of zeromq event
                            zmq_pollitem_t items[] = {
                                {mSenderEp, 0, ZMQ_POLLOUT, 0},
                            };
                            int rc = zmq_poll( items, sizeof(items) / sizeof(zmq_pollitem_t), 10);
                            if (rc > 0 && items[0].revents & ZMQ_POLLOUT)
                            {
                                drCon::log(__FUNCTION__, "send zmq packet");
                                std::string strmsg;
                                CoolconMsg msg_pb;
                                msg_pb.set_type(1);
                                msg_pb.set_name("GET SCREEN CAP");
                                msg_pb.mutable_sinfo()->set_width( ((drConCommandGesture*)evt.cmd)->getWidth() );
                                msg_pb.mutable_sinfo()->set_height( ((drConCommandGesture*)evt.cmd)->getHeight() );

                                msg_pb.SerializeToString(&strmsg);

                                zmq_msg_t msg;
                                zmq_msg_init_size(&msg, strmsg.size());
                                memcpy(zmq_msg_data(&msg), strmsg.data(), strmsg.size() * sizeof(std::string::value_type));
                                zmq_msg_send(&msg, mSenderEp, ZMQ_DONTWAIT);
                                zmq_msg_close(&msg);
                            }
                            else if (rc < 0)
                            {
                                snprintf(stream_buf, sizeof(stream_buf), "zmq_poll error: %s", zmq_strerror(errno));
                                drCon::log(stream_buf);
                            }
                            else
                            {
                                drCon::log("zmq_poll send timeout");
                            }
                        }
                    }
                    break;
                default:
                    break;
            }

            // release event resource
            delete pcmd;
        } // end while

        // discovery pattern
        // broadcase search packet every 3 seconds
        if ( now.laterThan(mSayhelloTime, E_SAYHELLO_INTERVAL) )
        {
            // drCon::log(__FUNCTION__, "discover targets......");
            // whether to clear ? or use fresh timer

            // second var true: send packet x-times repeatedly
            mImp.sendPacket(drConCommandSayHello(), false);
            mSayhelloTime = now;
        }

        // send current status data (acc, gyro, libPhoneRemote data)
        if ( E_SESSIONSTATE_CONNECTED == mSessionState )
            mImp.sendPacket(mMoteState, mTarget.peekAddress());
    }
    void drConClientSessionImp2::update()
    {
        process_recvbuf();
        process_sendbuf();
    }
    void drConClientSessionImp2::releaseCoolconMsg()
    {
        if (mpCoolconMsg)
        {
            delete mpCoolconMsg;
            mpCoolconMsg = 0;
        }
    }
} // end namespace dragonBoat
