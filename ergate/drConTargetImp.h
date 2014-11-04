#ifndef __DR_CON_TARGET_IMP_H__
#define __DR_CON_TARGET_IMP_H__

#include "drConTarget.h"
#include "drConTransmitType.h"
//#include "drConTransmitSocketAddress.h"
#include "drConTransmitterDiagram.h"

namespace dragonBoat {
    class drConTargetImp : public drConTarget
    {
        public:
            drConTargetImp();
            drConTargetImp(const std::string& name, const drConTransmitDiagramAddress& addr);
            drConTargetImp(drConTransmitType type);
            virtual ~drConTargetImp() {}
        public:
            virtual std::string getName() const { return mName; }
            drConTransmitDiagramAddress peekAddress() const { return mAddr; }
            drConTransmitType getType() const { return mType; }

            bool equalTo(const drConTargetImp& other) const;
        protected:
            drConTransmitType mType;
            std::string mName;
            drConTransmitDiagramAddress mAddr;
    };
} // end namespace dragonBoat
#endif //__DR_CON_TARGET_IMP_H__
