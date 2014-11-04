#ifndef __DR_CON_COMMAND_FACTORY_H__
#define __DR_CON_COMMAND_FACTORY_H__

#include "drConCommand.h"
#include <map>

namespace dragonBoat {
    typedef drConCommand* (*PNEWONEFUNC)();
    typedef bool (*PCOPYFUNC)(drConCommand*, const drConCommand*);
    struct CommandMappings {
        CommandMappings()
            : commandType(0)
              , func(0)
              , copyfunc(0)
        {
        }
        CommandMappings(int arg1, PNEWONEFUNC arg2, PCOPYFUNC arg3)
            : commandType(arg1)
              , func(arg2)
              , copyfunc(arg3)
        {
        }
        int commandType;
        PNEWONEFUNC func;
        PCOPYFUNC copyfunc;
    };
    typedef std::map<drConCommandType, CommandMappings> TCommandMap;

    class drConCommandFactory
    {
        public:
            drConCommand* newCommand(drConCommandType type);
            bool copyCommand(drConCommand*, const drConCommand*);

        public:
            static drConCommandFactory& get();

        protected:
            drConCommandFactory();
            ~drConCommandFactory()
            {}

        private:
            TCommandMap mCommandRegistry;

        private:
            drConCommandFactory(const drConCommandFactory &);
    };
}
#endif //__DR_CON_COMMAND_FACTORY_H__
