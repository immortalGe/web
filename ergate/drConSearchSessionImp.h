#ifndef __DR_CON_SEARCH_SESSION_IMP_H__
#define __DR_CON_SEARCH_SESSION_IMP_H__

#include "drConSessionImp.h"

namespace dragonBoat {
    class drConSearchSessionImp : public drConSearchSession
    {
        public:
            //    virtual drConTarget* getTarget();
            virtual bool startup();
            virtual void shutdown();
            virtual bool workable();
            virtual void sayHello();

        public:
            //    static drConSearchSessionImp *newOne(drConTransmitType type);
            drConSearchSessionImp(drConTransmitType type);
            virtual ~drConSearchSessionImp();

            bool getHelloAnswer(std::string& who, drConTransmitDiagramAddress& addr);
        protected:
            drConSessionImpBase mImp;
            drConTransmitDiagramAddress mScratchAddress;
    };

    //inline drConSearchSessionImp *drConSearchSessionImp::newOne(drConTransmitType type)
    //{
    //    return new drConSearchSessionImp(type);
    //}
} // end namespace dragonBoat
#endif //__DR_CON_SEARCH_SESSION_IMP_H__
