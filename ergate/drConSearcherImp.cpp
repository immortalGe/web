#include "drConSearcherImp.h"
#include "drConSearchSessionImp.h"
#include "drConTargetImp.h"
#include "drConCommand.h"
#include "drConTime.h"
#include "drConLogger.h"
#include "drConTransmitterDiagram.h"

#include <stdio.h>

namespace dragonBoat {
    bool drConSearcherImp::startSearch(TTargets& tgts, unsigned long msecs)
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
                //drCon::log(__FUNCTION__, "found a target.");
                //                    handler->onOneTargetFound(*this, target);

                snprintf(stream_buf, sizeof(stream_buf), "found target, who: %s, addr: %s"
                        , who.c_str(), addr.toString().c_str());
//                drCon::log(stream_buf);

                tgts.push_back(target);
                break;
            }
        }
        return !tgts.empty();
    }

    void drConSearcherImp::stopSearch()
    {
        mNeedStop = true;
    }
} // end namespace dragonBoat
