
#include "drConTargetImp.h"
#include "drConTransmitter.h"

namespace dragonBoat {
    drConTargetImp::drConTargetImp()
        : mType(drTT_DIAGRAM)
    {
    }
    drConTargetImp::drConTargetImp(const std::string& name, const drConTransmitDiagramAddress& addr)
        : mType(drTT_DIAGRAM)
        , mName(name)
        , mAddr(addr)
    {
    }
    drConTargetImp::drConTargetImp(drConTransmitType type)
        : mType(type)
    {
    }
#if 0
    drConTargetImp* drConTargetImp::anyOne(drConTransmitType type)
    {
        drAutoRef<drConTargetImp>target;
        drConTransmitterMan &tran_man = drConTransmitterMan::get();

        drAutoRef<drConTransmitAddress> addr(tran_man.createAnyAddress(type));
        if (addr.notNull())
        {
            std::string name("anyone");
            target.assign(drConTargetImp::newOne(name, addr));
        }

        return target.get();
    }
#endif
    bool drConTargetImp::equalTo(const drConTargetImp& other) const
    {
        return mAddr.equalTo(other.mAddr);
    }

} // end namespace dragonBoat
