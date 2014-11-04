#ifndef __DRAGON_BONE_WRAPPER_H__
#define __DRAGON_BONE_WRAPPER_H__

#include "drConLoggerImpl.h"
#include "drConTargetImp.h"
#include "drConSearcher.h"
#include "drConClientSessionImp.h"

#include <vector>

namespace dragonBoat {
    class drServerList
    {
        typedef std::vector<drConTargetImp> drConTargets;
        drConTargets mServers;

        public:
        int getCount() const { return mServers.size(); }
        ~drServerList()
        {
            clear();
        }

        bool getServer(int id, drConTargetImp& target);
        void putServer(const drConTargetImp& target);
        void clear();
        bool hasThis(const drConTargetImp& target) const;
    };

    class drDragonBoneWrapper// : public drConSearchHandler
    {
        private:
            void clearSession();
        public:
//            virtual void onOneTargetFound(const drConSearcher& searcher, const drConTarget& target);

        public:
            drDragonBoneWrapper();
            virtual ~drDragonBoneWrapper();

        public:
            static drDragonBoneWrapper *newOne();
            bool init(drConLoggerImpl *logger = NULL);

            bool searchServers(unsigned long msecs);

            int getServerCount() const;

            std::string getServerName(int id);
            std::string getServerAddress(int id);

            bool connect(int id, const std::string& name, const std::string& pin);

            int currentServer();
            bool workable();

            bool sendKey(int key, int status);
            bool sendAcc(float x, float y, float z);
            bool sendGyro(float x, float y, float z);

            bool sendMote(unsigned long keys, float acc[3], float gyro[3]);
            bool sendMote(unsigned long keys, float acc[3], float gyro[3], int joystick[2]);

            bool sendMouseMoved(int x, int y);
            bool sendMouseClicked(int x, int y);
            bool sendMouseDoubleClicked(int x, int y);

            bool validateTargetStatus();

            bool sendShutdown();

        private:
            //    drAutoRef<drConEngine> mEngine;
            drConClientSessionImp * mSession;

            drServerList mServers;
            int mCurServer;
    };
} // end namespace dragonBoat
#endif //__DRAGON_BONE_WRAPPER_H__
