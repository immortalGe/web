#ifndef __DR_CON_CLIENT_SESSION_IMP2_H__
#define __DR_CON_CLIENT_SESSION_IMP2_H__

#include "drConSessionImp.h"
#include "drConTargetImp.h"
#include "drConTime.h"
#include "drConCommandMoteState.h"

// use deque to implement event queue
#include <deque>
#include <map>

namespace dragonBoat {
    class CoolconMsg;
    // event
    struct drCommandEvent {
        drConTargetImp target;
        drConCommand * cmd;
    };
    // event queue
    class drConClientEventQueue {
        private:
            enum {
                E_QUEUE_SIZE = 10,
            };
            typedef std::deque<drCommandEvent> TEventQueue;
            TEventQueue mEvents;

            void prepare_events();
        public:
            // push action event into the queue
            void pushLogin(const drConTargetImp& target, const std::string& name, const std::string& pin);
            void pushLogout(const drConTargetImp& target);

            void pushKey(int key, int status);
            void pushMote(int keystate, float acc[3], float gyro[3]);
            void pushMouseMoved(int x, int y);
            void pushMouseClicked(int x, int y);
            void pushGesture(int gAction, float x, float y, int width, int height);
            void pushCoolconEvent(const std::string& event);

            // pop the event from the queue
            // return 0 if no event in the queue
            // event object should be delete by
            // user.
            drCommandEvent popEvent();
            void clear();
            bool empty() const { return mEvents.empty(); }
    };

    struct STargetInfo {
        STargetInfo(const std::string& n, const drTime& t)
            : name(n)
            , uptime(t)
        {
        }
        std::string name;
        drTime uptime; // indicate time received drCON_ANS_HELLO packet
    };
    typedef std::map<drConTransmitDiagramAddress, STargetInfo, drConAddrComp> TServerTargets;

    class drConClientSessionImp2
    {
        public:
            // states of current session
            typedef enum {
                E_SESSIONSTATE_DISCONNECTED = 0,
                E_SESSIONSTATE_CONNECTING,
                E_SESSIONSTATE_CONNECTED,
                E_SESSIONSTATE_BROKEN,
            } ESessionState;

        private:
            // some time-out constant value
            enum {
                E_SAYHELLO_INTERVAL = 3000,     // send search packet every 3000 mili-seconds
                E_LOGIN_TIMEOUT = 1000,         // login time out, if timeout reached, login failed
                E_CONNECTION_TIMEOUT = 5000,    // if no packet received in connection time-out, connection lost
                E_SERVERALIVE_TIME = E_SAYHELLO_INTERVAL * 2, // timeout to remove server from list
            };

            // last update time (received packet from session)
            // and last update time, say-hello time and login time
            drTime mUpdateTime, mSayhelloTime, mLoginTime;
            unsigned int mLoginSequence;

            ESessionState mSessionState;
            // event queue to process key/mouse event
            drConClientEventQueue mEvents;
            drConCommandMoteState mMoteState;

            // status flag to indicate whether connect is broken
            // true: connection is broken
            // false: connection is ok
            // bool mTargetMissing;

            // indicate which target to connect and connected
            drConTargetImp mTarget;

            // session var send/receive packet
            drConSessionImpBase mImp;

            // to record current packet from which address
            // drConTransmitDiagramAddress mScratchAddress;

            // containers to store available targets
            TServerTargets mAvailTargets;

            // indicate in this frame, targets has been changed
            bool mTargetsChanged;

            // zeromq end point
            void * mZmqContext;
            void * mSenderEp;

            // coolcon msg received
            CoolconMsg * mpCoolconMsg;

        private:
            // internal functions
            bool _connect(const drConTargetImp& target,
                    const std::string& name, const std::string& pin);
            // sess_state is disconnect reason
            void _disconnect(ESessionState sess_state);

            // split receive/send packet for thread-safety
            void process_recvbuf();
            void process_sendbuf();

        public:
            bool startup();
            void shutdown();
            bool workable();

            // sending data functions
            bool sendKeyData(int key, int status);
            bool sendMoteData(unsigned long keys, float acc[3], float gyro[3]);
            bool sendMoteData(unsigned long keys, float acc[3], float gyro[3], int joystick[2]);

            bool sendMouseMoved(int x, int y);
            bool sendMouseClicked(int x, int y);
            bool sendMouseDoubleClicked(int x, int y);
            bool sendGestureData(int gAction, float x, float y, int width, int height);
            bool sendCoolconEvent(const std::string& evt);
        public:
            // login action
            bool connect(const std::string& targetaddr, const std::string& targetname
                    , const std::string& name, const std::string& pin);
            void disconnect();
            // single-step update for session loop
            void update();

            // check internal status functions
            TServerTargets getAvailableTargets() const { return mAvailTargets; }
            int getSessionState(drConTargetImp& target) const
            {
                target = mTarget;
                return mSessionState;
            }
            // check targets changed status
            // this function must be call in the frame after update function
            bool targetsChanged() const { return mTargetsChanged; }

            CoolconMsg * getCoolconMsg() const { return mpCoolconMsg; }
            void releaseCoolconMsg();
        public:
            drConClientSessionImp2();
            drConClientSessionImp2(const drConTargetImp& target);
            virtual ~drConClientSessionImp2();
    };
} // end namespace dragonBoat
#endif //__DR_CON_CLIENT_SESSION_IMP2_H__
