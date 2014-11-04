#ifndef __DR_CON_SEARCHER_IMPL_H__
#define __DR_CON_SEARCHER_IMPL_H__

#include "drConSearcher.h"
#include "drConTransmitType.h"
#include "drConTargetImp.h"
#include <vector>

namespace dragonBoat {
    class drConSearchSession;
    typedef std::vector<drConTargetImp> TTargets;

    class drConSearcherImp// : public drConSearcher
    {
        public:
            virtual bool startSearch(TTargets& tgts, unsigned long msecs);
            virtual void stopSearch();

        public:
            //    static drConSearcherImp *newOne(drConTransmitType type);
            drConSearcherImp(drConTransmitType type);
            virtual ~drConSearcherImp();

        private:
            drConTransmitType mType;

            bool mNeedStop;
    };

    //inline drConSearcherImp *drConSearcherImp::newOne(drConTransmitType type)
    //{
    //    return new drConSearcherImp(type);
    //}

    inline drConSearcherImp::drConSearcherImp(drConTransmitType type)
        : mType(type)
          , mNeedStop(false)
    {
    }

    inline drConSearcherImp::~drConSearcherImp()
    {
    }
} // end namespace dragonBoat
#endif //__DR_CON_SEARCHER_IMPL_H__
