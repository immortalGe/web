#include "drConSessionImp.h"
#include "drConTransmitter.h"
#include "drConPacketClosure.h"
#include "drConCommand.h"
#include "drConTargetImp.h"
#include "drConLogger.h"
#include "drConCommandLogin.h"

namespace dragonBoat {
    bool drConSessionImpBase::startup()
    {
        return mTransmitter.startup();
    }

    void drConSessionImpBase::shutdown()
    {
        mTransmitter.shutdown();
    }

    bool drConSessionImpBase::workable()
    {
        return mTransmitter.workable();
    }

    drConSessionImpBase::drConSessionImpBase(drConTransmitType type, bool server)
        : mTransmitter(server)
          , mScratchAddress(drConTransmitterDiagram::kServerPort)
    {
        // cannot call virtual function in constructor
        mTransmitter.startup();
//        startup();
    }

    drConSessionImpBase::~drConSessionImpBase()
    {
        shutdown();
    }

    bool drConSessionImpBase::sendPacket(const drConCommand& packet
            , const drConTransmitDiagramAddress& addr
            , bool important)
    {
        mPacketClosure.putCommand(packet);

        //        if (packet != NULL)
        //        {
        //            std::string str("sending a packet of type ");
        //            str += drConCommand::typeToString(packet->getType());
        //            //drCon::log(__FUNCTION__, str);
        //        }

        bool ret_send = false;
        int rep_max = important ? 3 : 1;
        for (int rep = 0; rep < rep_max; rep++)
        {
            ret_send |= mTransmitter.sendPacket(mPacketClosure, addr);
        }

        return ret_send != 0;
    }

    drConPacketClosure drConSessionImpBase::readPacket(const drConTransmitDiagramAddress& addr, bool important)
    {
        mPacketClosure.clear();

        int rep_max = important ? 4 : 1;
        for (int rep = 0; rep < rep_max; rep++)
        {
            if ( mTransmitter.readPacket(mPacketClosure, mScratchAddress)
                    && addr.equalTo(mScratchAddress) )
            {
                std::string str("received a packet of type ");
                str += drConCommand::typeToString(mPacketClosure.getCommand()->getType());
                drCon::log(__FUNCTION__, str);
                break;
            }
        }

        return mPacketClosure;
    }

    bool drConSessionImpBase::sendPacket(const drConCommand& packet, bool important)
    {
        mPacketClosure.putCommand(packet);

        //        if (packet != NULL)
        //        {
        //            if (0)
        //            {
        //                std::string str("sending a packet of type ");
        //                str += drConCommand::typeToString(packet->getType());
        //                drCon::log(__FUNCTION__, str);
        //            }
        //        }

        bool ret_send = false;
        int rep_max = important ? 3 : 1;
        for (int rep = 0; rep < rep_max; rep++)
        {
            ret_send |= mTransmitter.sendPacket(mPacketClosure);
        }

        return ret_send != 0;
    }

    drConPacketClosure drConSessionImpBase::readAnyPacket(drConTransmitDiagramAddress& addr)
    {
        mPacketClosure.clear();

        if (mTransmitter.readPacket(mPacketClosure, addr))
        {
            //            cmd = mPacketClosure->getCommand();
            //            if (cmd != NULL)
            //            {
            //                if (0)
            //                {
            //                    std::string str("received a packet of type ");
            //                    str += drConCommand::typeToString(cmd->getType());
            //                    drCon::log(__FUNCTION__, str);
            //                }
            //            }
        }
        drConCommand * pcmd = mPacketClosure.getCommand();
        if (pcmd && pcmd->getType() == drCT_LOGIN)
        {
            drConCommandLogin* pLogin = (drConCommandLogin*)pcmd;
        }

        return mPacketClosure;
    }

    void drConSessionImpBase::discardPackets()
    {
        drConTransmitDiagramAddress addr(drTT_DIAGRAM);
        for (;;)
        {
            if (mTransmitter.readPacket(mPacketClosure, addr))
            {
            }
            break;
        }
    }

    //drConTransmitAddress *drConSessionImpBase::newAddress()
    //{
    //    if (mTransmitter.notNull())
    //    {
    //        return mTransmitter->newAddress();
    //    }
    //
    //    return NULL;
    //}
} // end namespace dragonBoat
