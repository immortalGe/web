#ifndef __DR_CON_SEQUENCE_H__
#define __DR_CON_SEQUENCE_H__

#include "drConTransmitStream.h"

namespace dragonBoat {
    class drConSequence
    {
        protected:
            unsigned int mSequence;
            static unsigned int gSequence;

        public:
            drConSequence()
            {
                gSequence++;
                if (gSequence == 0)
                    gSequence++;

                mSequence = gSequence;
            }

            drConSequence(const drConSequence& other)
            {
                if (this != &other)
                    mSequence = other.mSequence;
            }

            drConSequence(int seq)
                : mSequence(seq)
            {
            }

            drConSequence& operator = (const drConSequence& other)
            {
                if (this != &other)
                    mSequence = other.mSequence;
                return *this;
            }

            bool laterThan(const drConSequence& other) const;

            friend bool operator == (const drConSequence& left, const drConSequence& right);

            unsigned int getValue() const { return mSequence; }

        public:
            virtual bool put(drConTransmitOutStream &out);
            virtual bool get(drConTransmitInStream &in);
    };

    inline bool operator == (const drConSequence& left, const drConSequence& right)
    {
        return &left == &right || left.mSequence == right.mSequence;
    }

    inline bool drConSequence::put(drConTransmitOutStream& out)
    {
        return out.putInteger(mSequence);
    }

    inline bool drConSequence::get(drConTransmitInStream& in)
    {
        return in.getInteger(&mSequence);
    }

    // To avoid number wrapping
    inline bool drConSequence::laterThan(const drConSequence& other) const
    {
        if (this == &other)
            return false;

        if (mSequence == other.mSequence)
        {
            return false;
        }
        else if (mSequence < other.mSequence)
        {
            if (((mSequence & 0xFFFF0000) == 0) && (other.mSequence & 0xFFFF0000))
                return true;

            return false;
        }
        else
        {
            if (((other.mSequence & 0xFFFF0000) == 0) && (mSequence & 0xFFFF0000))
                return false;

            return true;
        }
    }
} // end namespace dragonBoat
#endif //__DR_CON_SEQUENCE_H__
