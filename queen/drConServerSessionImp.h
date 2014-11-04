#ifndef __DR_CON_SERVER_SESSION_IMP_H__
#define __DR_CON_SERVER_SESSION_IMP_H__

#include "drConSessionImp.h"
#include "drConTargetImp.h"
#include "drConTime.h"
#include "Timer.h"

#include <map>
#include <set>

namespace dragonBoat {
    // forward declaration
    class drConCommandLogin;
    class drConCommandLogout;

    class drConServerSessionImp : public drConServerSession
    {
        private:
            enum {
                // when 5000 mili-seconds with no packet(udp) received for
                // a target, disconnect the client target.
                E_CLIENT_NONACTIVE_TIMEOUT = 5000,
                E_KICKCLIENT_TIME = 2500,
                E_SCREENCAP_INTERVAL = 2000,
            };

            // element to store client target info
            struct SClientTarget {
                drTime lastTime; // to store last packet time
                drTime lastKickTime; // to store last kick time
                drConTargetImp target;
            };
            // containers to support multiple targets
            typedef std::map<unsigned int, SClientTarget> TTargets;
            typedef std::set<unsigned int> TIDs;

            // to store info of screencap
            struct ScreencapInfo {
                uint32_t w, h, f, size;
                const void * pbuf;
            };

        public:
            //    virtual drConTarget* getTarget();
            virtual bool startup();
            virtual void shutdown();
            virtual bool workable();

            virtual void setHandler(drConSessionHandler *handler) { mHandler = handler; }
            virtual void process();
            virtual void kickClient();

            // process drCON_HELLO, drCON_LOGIN, drCON_LOGOUT packet
            void answerHello(const drConTransmitDiagramAddress& addr);
            void answerLogin(const drConCommandLogin& login, const drConTransmitDiagramAddress& addr);
            void answerLogout(const drConCommandLogout& logout, const drConTransmitDiagramAddress& addr);
            void setScreenSize(int width, int height);
        public:
            //    static drConServerSessionImp *newOne(drConTransmitType type);
            drConServerSessionImp(const drConTargetImp& target);
            virtual ~drConServerSessionImp();

        private:
            // helper functions
            // get target id from target list, if no target with same addr, return false
            // else return id of the client target
            bool getTargetID(const drConTransmitDiagramAddress& addr, unsigned int& id) const;

            // register new id from ids pool(mIDs)
            unsigned int newTargetID();
            // unregister id and put it back into ids pool(mIDs)
            void reclaimTargetID(unsigned int);

            // helper function to dump ids pool
            void dumpIDspool();

            // update target lastTime and disconnect non-active target when TIMEOUT reached.
            void updateTargetsTime(const drTime& uptime);

            // capture screen transaction function
            // and convert screen data to CoolconMsg
            void doCaptureScreen( std::string& strmsg );
        protected:
            drConSessionImpBase mImp;

            // support multi-clients
            //            drConTargetImp mTarget;
            TTargets mTargets;
            TIDs mIDs;

            // zmq sockets, server end point
            void * mZmqContext;
            void * mServerEp;
            void * mScreenCap;

            int mScreenWidth, mScreenHeight;
            int mTargetWidth, mTargetHeight;

            drConTransmitDiagramAddress mScratchAddress;
            drConSessionHandler * mHandler;

            drConSequence mPacketSequence;

            // timer to calc time elapsed
            TM::Timer mTimer;
            // timer interval to do the screencap
            unsigned int mCapIntrvl;

            // buffer to store screencap data.
            ScreencapInfo mScreenBuf;

            // flag to indicate capture screen should be done
            bool mDoCapture;
    };
} // end namespace dragonBoat
#endif //__DR_CON_SERVER_SESSION_IMP_H__
