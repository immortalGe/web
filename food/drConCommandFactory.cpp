#include "drConCommandFactory.h"

#include "drConCommand.h"
#include "drConCommandSayHello.h"
#include "drConCommandAnsHello.h"
#include "drConCommandLogin.h"
#include "drConCommandAnsLogin.h"
#include "drConCommandShutdown.h"
#include "drConCommandEcho.h"
#include "drConCommandLogout.h"
#include "drConCommandKey.h"
#include "drConCommand3D.h"
#include "drConCommandMoteState.h"
#include "drConCommandMoteStates.h"
#include "drConCommandMouse.h"
#include "drConCommandGesture.h"
#include "drConCommandCoolconEvent.h"
#include <utility>

namespace dragonBoat {
    CommandMappings regs[] = {
        CommandMappings(drCT_KEY, drConCommandKey::newObject, drConCommandKey::copyObject),
        CommandMappings(drCT_ACC, drConCommandAcc::newObject, drConCommandAcc::copyObject),
        CommandMappings(drCT_GYRO, drConCommandGyro::newObject, drConCommandGyro::copyObject),
        CommandMappings(drCT_LOGIN, drConCommandLogin::newObject, drConCommandLogin::copyObject),
        CommandMappings(drCT_ANS_LOGIN, drConCommandAnsLogin::newObject, drConCommandAnsLogin::copyObject),
        CommandMappings(drCT_SAY_HELLO, drConCommandSayHello::newObject, drConCommandSayHello::copyObject),
        CommandMappings(drCT_ANS_HELLO, drConCommandAnsHello::newObject, drConCommandAnsHello::copyObject),
        CommandMappings(drCT_MOTE_STATE, drConCommandMoteState::newObject, drConCommandMoteState::copyObject),
        CommandMappings(drCT_MOUSE, drConCommandMouse::newObject, drConCommandMouse::copyObject),
        CommandMappings(drCT_LOGOUT, drConCommandLogout::newObject, drConCommandLogout::copyObject),
        CommandMappings(drCT_ECHO, drConCommandEcho::newObject, drConCommandEcho::copyObject),
        CommandMappings(drCT_SHUTDOWN, drConCommandShutdown::newObject, drConCommandShutdown::copyObject),
        CommandMappings(drCT_MOTE_STATES, drConCommandMoteStates::newObject, drConCommandMoteStates::copyObject),
        CommandMappings(drCT_GESTURE, drConCommandGesture::newObject, drConCommandGesture::copyObject),
        CommandMappings(drCT_COOLCON_EVENT, drConCommandCoolconEvent::newObject, drConCommandCoolconEvent::copyObject),
    };
    drConCommandFactory::drConCommandFactory()
    {
        for (int i = 0; i < sizeof(regs) / sizeof(regs[0]); i++)
            mCommandRegistry.insert(
                    std::pair<drConCommandType, CommandMappings>(drConCommandType(regs[i].commandType), regs[i])
                    );
    }

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
} // end namespace dragonBoat
