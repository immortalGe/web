#include "drConSearchSessionImp.h"

#include "drConLogger.h"

#include "drConCommandSayHello.h"
#include "drConCommandAnsHello.h"

#include "drConTargetImp.h"

namespace dragonBoat {
    drConSearchSessionImp::drConSearchSessionImp(drConTransmitType type)
        : mImp(type, false)
          , mScratchAddress(drConTransmitterDiagram::kServerPort)
    {
        drCon::log(__FUNCTION__, "constructing drConSearchSessionImp.");
    }

    drConSearchSessionImp::~drConSearchSessionImp()
    {
        drCon::log(__FUNCTION__, "destroyed drConSearchSessionImp.");
    }

//    drConTarget* drConSearchSessionImp::getTarget()
//    {
//        return mImp.getTarget();
//    }

    bool drConSearchSessionImp::startup()
    {
        return mImp.startup();
    }

    void drConSearchSessionImp::shutdown()
    {
        mImp.shutdown();
    }

    bool drConSearchSessionImp::workable()
    {
        return mImp.workable();
    }

    void drConSearchSessionImp::sayHello()
    {
        if (workable())
        {
            mImp.discardPackets();
            mImp.sendPacket(drConCommandSayHello(), true);
        }
    }

    bool drConSearchSessionImp::getHelloAnswer(std::string& who, drConTransmitDiagramAddress& addr)
    {
        bool bResult = false;
        drConPacketClosure pkt = mImp.readAnyPacket(mScratchAddress);

        drConCommand* pcmd = pkt.getCommand();
        if ( pcmd && pcmd->hasType(drCT_ANS_HELLO) )
        {
            drConCommandAnsHello * pAnsHello = (drConCommandAnsHello*)pcmd;
            who = pAnsHello->getName();
            addr = mScratchAddress;
            bResult = true;
        }

        return bResult;
    }
} // end namespace dragonBoat
