/*Whole Porter
drConSocket --- int mSocket;int sendTo(...);int recvFrom(...);
drConTransmitterSocketAddress --- sockaddr_in mSockaddr;
drConTransmitDiagramStream --- char *buffer; int mSize; int mPosition;
  |  
  |
  |     drConTransmitDiagramOutStream
==> friend                =========> private drConTransmitDiagramStream mStream;
        drConTransmitDiagramInStream


drConTransmitterDiagram --- drConSocket *mSocket; bool mAsServer; bool mInitialized; drConTransmitDiagramInStream mInStream; drConTransmitDiagramOutStream mOutStream;
//sendTo LAN all host appointed port
bool sendPacket(drConPacket& packet)
{
    if (workable())
    {
        mOutStream.position(0);
        if (packet.put(mOutStream))
        {
            int data_size = mOutStream.getSize();
            const void *buffer = mOutStream.getBuffer();
            drConSocketAnyAddress any_addr(kServerPort);  //create address
            int send_bytes = mSocket->sendTo(buffer, data_size, any_addr.addr_in);
            return (data_size == send_bytes);
        }
        return false;
    }
}
virtual bool sendPacket(drConPacket& packet, const drConTransmitAddress& addr);
{
    if ( addr.getType() != drTT_DIAGRAM )
        return false;

    if ( workable() )
    {
        mOutStream.position(0);
        if (packet.put(mOutStream))
        {
            const drConTransmitSocketAddress* sock_addr = (const drConTransmitSocketAddress *)&addr;
            const sockaddr_in& addr_in = sock_addr->getSockAddr();

            int data_size = mOutStream.getSize();
            int send_bytes = mSocket->sendTo(mOutStream.getBuffer(), data_size, addr_in);
            return (data_size == send_bytes);
        }
    }
    return false;
}
virtual bool readPacket(drConPacket& packet, drConTransmitAddress& addr)
{
    if ( addr.getType() != drTT_DIAGRAM )
        return false;

    if (workable())
    {
        sockaddr_in addr_in = {0};
        drConTransmitSocketAddress *sock_addr = (drConTransmitSocketAddress *)&addr;
        int buf_size = mInStream.getCapacity();

        mInStream.position(0);
        if (mSocket->recvFrom(mInStream.getWriteBuffer(), buf_size, addr_in) > 0)
        {
            //mInStream->dump();   //just log print
            sock_addr->setSockAddr(addr_in);
            return packet.get(mInStream);   //do what?
        }
    }
    return false;
}
*/


/*food
drConSequence --- protected: unsigned int mSequence;static unsigned int gSequence;
bool drConSequence::put(drConTransmitOutStream& out)
    return out.putInteger(mSequence);
bool drConSequence::get(drConTransmitInStream& in)
    return in.getInteger(&mSequence);

enum drConCommandType {...};

drConCommand --- protected: drConCommandType mType; drConSequence mSequence; drConSequence mRequest; static drConSequence gSequence;
//drConCommand(type):mType(type), mSequence(),mRequest(0)
//mSequence.mSequence = 1 mRequese.mSequence = 0;
bool put(drConTransmitOutStream& out);
{
    bool ret_code = true;
    if (ret_code)
        ret_code = mSequence.put(out);
    if (ret_code)
        ret_code = mRequest.put(out);
    
    return ret_code;
}
bool get(drConTransmitInStream& in);
{
    bool ret_code = true;
    if (ret_code)
        ret_code = mSequence.get(in);
    if (ret_code)
        ret_code = mRequest.get(in);

    return ret_code;
}

//various food type derive drConCommand
static drConCommand *newObject() { return new drConCommandSayHello(); }
static bool copyObject(drConCommand* dst, const drConCommand* src)
{
    *((ThisClass*)dst) = *((const ThisClass*)src);
    return true;
}
bool put(drConTransmitDiagramOutStream &out)
{
    drConCommand::put(out);  //mSequence, mRequest
    out.put..();           //member data
}
bool get(drConTransmitDiagramInStream &in)
{
    drConCommand::get(in);
    in.get...();
}
struct CommandMappings --- int commandType; drConCommand* (*PNEWONEFUNC) func;
bool (*PCOPYFUNC)(drConCommand*, const drConCommand*) copyfunc;
map<drConCommandType, CommandMappings> TCommandMap
drConCommandFactory --- private:TCommandMap mCommandRegistry;
drConCommand* drConCommandFactory::newCommand(drConCommandType type)
{
    drConCommand* cmd = NULL;

    TCommandMap::const_iterator citr = mCommandRegistry.find(type);
    if ( citr != mCommandRegistry.end() && citr->second.func)
    {
        cmd = citr->second.func();
    }

    return cmd;
}
bool drConCommandFactory::copyCommand(drConCommand* dst, const drConCommand* src)
{
    bool bResult = false;

    if (dst->getType() != src->getType())
        return bResult;

    TCommandMap::const_iterator citr = mCommandRegistry.find(src->getType());
    if ( citr != mCommandRegistry.end() && citr->second.copyfunc)
    {
        bResult = citr->second.copyfunc(dst, src);
    }

    return bResult;
}

drConCommandFactory& drConCommandFactory::get()
{
    static drConCommandFactory one;
    return one;
}

drConPacketClosure --- drConCommand * mEntity;
drConCommand* getCommand() const { return mEntity; }
void putCommand(const drConCommand& cmd);
{
    clear();
    mEntity = drConCommandFactory::get().newCommand(cmd.getType());
    drConCommandFactory::get().copyCommand(mEntity, &cmd);
}

bool put(drConTransmitOutStream& out)
{
    if ( mEntity != NULL )
    {
        drConCommandType type = mEntity->getType();
        if (out.putShort(type))
        {
            return mEntity->put(out);
        }
    }
    return false;
}
bool get(drConTransmitInStream& in)
{
    drConCommandType type = drCT_FORCEWORD;
    if ( in.getShort((unsigned short *)&type) )
    {
        clear();
        mEntity = drConCommandFactory::get().newCommand(type);

        if ( mEntity )
        {
            return mEntity->get(in);
        }
}
*/



drConSessionImpBase --- protecetd: drConTransmitterDiagram mTransmitter; drConTransmitDiagramAddress mScratchAddress; drConPacketClosure mPacketClosure;

drConSessionImpBase(drConTransmitType type, bool server)
    : mTransmitter(server)
        , mScratchAddress(drConTransmitterDiagram::kServerPort)
{
    mTransmitter.startup();
}

bool sendPacket(const drConCommand& packet, const drConTransmitDiagramAddress& addr, bool important)
{
    mPacketClosure.putCommand(packet);
    bool ret_send = false;
    int rep_max = important ? 3 : 1;
    for (int rep = 0; rep < rep_max; rep++)
    {
        ret_send |= mTransmitter.sendPacket(mPacketClosure, addr);
    }
    return ret_send != 0;
}
bool sendPacket(const drConCommand& packet, bool important)
{
    mPacketClosure.putCommand(packet);
    bool ret_send = false;
    int rep_max = important ? 3 : 1;
    for (int rep = 0; rep < rep_max; rep++)
    {
        ret_send |= mTransmitter.sendPacket(mPacketClosure);
    }
    return ret_send != 0;
}
//readPacket Two versions
drConPacketClosure readPacket(const drConTransmitDiagramAddress& addr, bool important)
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
drConPacketClosure readAnyPacket(drConTransmitDiagramAddress& addr)
{
    mPacketClosure.clear();
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

drConSearchSession --- protected: drConSessionImpBase mImp; drConTransmitDiagramAddress mScratchAddress;

drConSearchSessionImp(drConTransmitType type)
    : mImp(type, false)
      , mScratchAddress(drConTransmitterDiagram::kServerPort)
{
    drCon::log(__FUNCTION__, "constructing drConSearchSessionImp.");
}
void sayHello()
{
    if (workable())
    {
        mImp.discardPackets();
        mImp.sendPacket(drConCommandSayHello(), true);
    }
}
bool getHelloAnswer(std::string& who, drConTransmitDiagramAddress& addr)
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

drConTargetImp --- protected: drConTransmitType mType; string mName; drConTransmitDiagramAddress mAddr;


typedef std::vector<drConTargetImp> TTargets
drConSearcherImp --- private: drConTransmitType mType; bool mNeedStop;
public:
virtual bool startSearch(TTargets& tgts, unsigned long msecs);
{
    drConSearchSessionImp session(mType);

    std::string who;
    drConTargetImp target(drTT_DIAGRAM);
    session.sayHello();

    drTime t_start;
    mNeedStop = false;
    tgts.clear();
    for (int rept = 0; rept < 5; rept++)
    {
        if ( mNeedStop || drTime().laterThan(t_start, msecs) )
            break;

        drConTransmitDiagramAddress addr(drTT_DIAGRAM);
        if( session.getHelloAnswer(who, addr) )
        {
            target = drConTargetImp(who, addr);
            tgts.push_back(target);
        }
    }
    return !tgts.empty();
}
virtual void stopSearch();
{
    mNeedStop = true;
}


