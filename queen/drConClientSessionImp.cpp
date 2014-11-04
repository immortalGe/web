#include "drConClientSessionImp.h"

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

#include "drConTargetImp.h"
#include "drConLogger.h"

namespace dragonBoat {
    drConClientSessionImp::drConClientSessionImp(const drConTargetImp& target)
        : mTarget(target)
          , mImp(target.getType(), false)
          , mTargetMissing(true)
    {
        drCon::log(__FUNCTION__, "constructing drConClientSessionImp.");
    }

    drConClientSessionImp::~drConClientSessionImp()
    {
        drCon::log(__FUNCTION__, "destroyed drConClientSessionImp.");
    }

    //drConTarget* drConClientSessionImp::getTarget()
    //{
    //    return mImp.notNull() ? mImp->getTarget() : NULL;
    //}

    bool drConClientSessionImp::startup()
    {
        return mImp.startup();
    }
    void drConClientSessionImp::shutdown()
    {
        mImp.shutdown();
    }

    bool drConClientSessionImp::workable()
    {
        return mImp.workable();
    }

    bool drConClientSessionImp::doLogin(const std::string& name, const std::string& pin)
    {
        if ( !workable() )
        {
            return false;
        }

        //    std::string str_name(name), str_pin(pin);
        //    drAutoRef<drConCommand> cmd(drConCommandLogin::newOne(str_name, str_pin));
        drConCommandLogin cmd(name, pin);

        mImp.discardPackets();
        for (int rept = 0; rept < 5; rept++)
        {
            if ( mImp.sendPacket(cmd, mTarget.peekAddress()) )
            {
                drConPacketClosure ans = mImp.readPacket(mTarget.peekAddress(), true);

                drConCommand * pcmd = 0;
                while ( pcmd = ans.getCommand() )
                {
                    if ( pcmd->hasType(drCT_ANS_LOGIN)
                            && (cmd.getSequence() == pcmd->getRequest()))
                    {
                        drConCommandAnsLogin *ans_ptr = (drConCommandAnsLogin *)pcmd;
                        if ( ans_ptr->getAuth() )
                        {
                            mCheckTime = drTime();
                            mTargetMissing = false;
                            mImp.discardPackets();
                            return true;
                        }

                        return false;
                    }
                    ans = mImp.readPacket(mTarget.peekAddress(), true);
                } // end for readPacket
            } // end if
        } // end for

        return false;
    }

    void drConClientSessionImp::doLogout()
    {
        if (workable())
            mImp.sendPacket(drConCommandLogout(), mTarget.peekAddress(), true);
    }

    bool drConClientSessionImp::sendKeyData(int key, int status)
    {
        if (validateTargetStatus() && workable())
        {
            return mImp.sendPacket(drConCommandKey(key, status), mTarget.peekAddress());
        }

        return false;
    }

    bool drConClientSessionImp::sendAccData(float x, float y, float z)
    {
        if (validateTargetStatus() && workable())
        {
            return mImp.sendPacket(drConCommandAcc(x, y, z), mTarget.peekAddress());
        }

        return false;
    }

    bool drConClientSessionImp::sendGyroData(float x, float y, float z)
    {
        if (validateTargetStatus() && workable())
        {
            return mImp.sendPacket(drConCommandGyro(x, y, z), mTarget.peekAddress());
        }

        return false;
    }

    bool drConClientSessionImp::sendMoteData(unsigned long keys, float acc[3], float gyro[3])
    {
        if (validateTargetStatus() && workable())
        {
            drConCommandMoteState pkt;
            pkt.setKey( drConKeyState(keys) );
            pkt.setAcc( drConVector(acc[0], acc[1], acc[2]) );
            pkt.setGyro( drConVector(gyro[0], gyro[1], gyro[2]) );
            return mImp.sendPacket(pkt, mTarget.peekAddress());
        }

        return false;
    }

    bool drConClientSessionImp::sendMoteData(unsigned long keys, float acc[3], float gyro[3], int joystick[2])
    {
        bool bResult = false;
        if (validateTargetStatus() && workable())
        {
            drConCommandMoteStates pkt;
            pkt.setKey( drConKeyState(keys) );
            pkt.setAcc( drConVector(acc[0], acc[1], acc[2]) );
            pkt.setGyro( drConVector(gyro[0], gyro[1], gyro[2]) );
            pkt.setJoystick( drConJoystick(joystick[0], joystick[1]) );

            return mImp.sendPacket(pkt, mTarget.peekAddress());
        }
        return bResult;
    }

    bool drConClientSessionImp::sendMouseMoved(int x, int y)
    {
        if (validateTargetStatus() && workable())
        {
            drConCommandMouse pkt;
            pkt.setMouseAction(drMA_MOVED);
            pkt.setMouseXY(x, y);

            return mImp.sendPacket(pkt, mTarget.peekAddress());
        }
        return false;
    }

    bool drConClientSessionImp::sendMouseClicked(int x, int y)
    {
        if (validateTargetStatus() && workable())
        {
            drConCommandMouse pkt;
            pkt.setMouseAction(drMA_CLICKED);
            pkt.setMouseXY(x, y);
            return mImp.sendPacket(pkt, mTarget.peekAddress());
        }
        return false;
    }

    bool drConClientSessionImp::sendMouseDoubleClicked(int x, int y)
    {
        if (validateTargetStatus() && workable())
        {
            drConCommandMouse pkt;
            pkt.setMouseAction(drMA_DOUBLECLICKED);
            pkt.setMouseXY(x, y);
            return mImp.sendPacket(pkt, mTarget.peekAddress());
        }
        return false;
    }
    bool drConClientSessionImp::sendGestureData(int gAction, float x, float y)
    {
        return false;
    }

    bool drConClientSessionImp::sendShutdown()
    {
        if (validateTargetStatus() && workable())
        {
            return mImp.sendPacket(drConCommandShutdown(), mTarget.peekAddress());
        }

        return false;
    }

    bool drConClientSessionImp::validateTargetStatus()
    {
        drTime now;
        if ( !now.laterThan(mCheckTime, 3000) )
        {
            return !mTargetMissing;
        }

        mCheckTime = drTime();

        if ( workable() )
        {
            drConPacketClosure pkt = mImp.readPacket(mTarget.peekAddress());
            if (pkt.getCommand())
            {
                mImp.discardPackets();
                mTargetMissing = false;
                return true;
            }
        }

        mTargetMissing = true;
        drCon::log(__FUNCTION__, "seems the server is not available.");
        return false;
    }

    void drConClientSessionImp::sayHello()
    {
        if (workable())
        {
            mImp.discardPackets();
            mImp.sendPacket(drConCommandSayHello(), mTarget.peekAddress(), true);
        }
    }

    bool drConClientSessionImp::getHelloAnswer(std::string& who)
    {
        if (workable())
        {
            drConPacketClosure pkt = mImp.readPacket(mTarget.peekAddress(), true);
            drConCommand * pcmd = pkt.getCommand();
            if ( pcmd
                    && pcmd->hasType(drCT_ANS_HELLO) )
            {
                drConCommandAnsHello * pAnsHello = (drConCommandAnsHello*)pcmd;
                who = pAnsHello->getName();
                return true;
            }
            else
            {
                drCon::log(__FUNCTION__, "no answering packet received.");
                return false;
            }
        }

        return false;
    }
} // end namespace dragonBoat
