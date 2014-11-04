#include "drConPacketClosure.h"
#include "drConTransmitStream.h"
#include "drConCommandFactory.h"
#include "drConCommandLogin.h"
#include "drConLogger.h"

#include <sstream>
#include <cstddef>

namespace dragonBoat {
    drConPacketClosure::drConPacketClosure()
        : mEntity(0)
    {
    }
    drConPacketClosure::~drConPacketClosure()
    {
        clear();
    }

    drConPacketClosure::drConPacketClosure(const drConPacketClosure& other)
    {
        if ( this != &other)
        {
            if (other.mEntity)
            {
                mEntity = drConCommandFactory::get().newCommand(other.mEntity->getType());
                drConCommandFactory::get().copyCommand(mEntity, other.mEntity);
            }
            else
            {
                mEntity = other.mEntity;
            }
        }
    }
    drConPacketClosure& drConPacketClosure::operator = (const drConPacketClosure& other)
    {
        if ( this != &other )
        {
            clear();
            if (other.mEntity)
            {
                mEntity = drConCommandFactory::get().newCommand(other.mEntity->getType());
                drConCommandFactory::get().copyCommand(mEntity, other.mEntity);
            }
            else
            {
                mEntity = other.mEntity;
            }
        }
        return *this;
    }
    void drConPacketClosure::clear()
    {
        if ( mEntity )
        {
            delete mEntity;
            mEntity = 0;
        }
    }
    void drConPacketClosure::putCommand(const drConCommand& cmd)
    {
        clear();
        mEntity = drConCommandFactory::get().newCommand(cmd.getType());
        drConCommandFactory::get().copyCommand(mEntity, &cmd);
    }

    bool drConPacketClosure::put(drConTransmitOutStream& out)
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

    bool drConPacketClosure::get(drConTransmitInStream& in)
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
            else
            {
                std::string str(__FUNCTION__);
                str.append("(): unsupported command type - ");

                std::stringstream sss;
                sss << (unsigned long)type;
                sss << "(" << drConCommand::typeToString(type) << ")";

                str.append(sss.str());
                drCon::log(str);
            }
        }

        return false;
    }
} // end namespace dragonBoat
