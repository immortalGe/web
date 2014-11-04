#ifndef _DR_CON_TRANSIMIT_SOCKET_STREAM_H__
#define _DR_CON_TRANSIMIT_SOCKET_STREAM_H__

#include "drConTransmitStream.h"

namespace dragonBoat {
    /*
     *******************************************************************************
     class drConTransmitStream
     *******************************************************************************
     */

    class drConTransmitDiagramStream /*: public drConTransmitStream*/
    {
        friend class drConTransmitDiagramOutStream;
        friend class drConTransmitDiagramInStream;

        public:
        // virtual functions
        virtual int getCapacity() const;
        virtual int getSize() const;
        virtual const void* getBuffer() const;

        void dump() const;

        public:
        explicit drConTransmitDiagramStream(int size);
        virtual ~drConTransmitDiagramStream();

        int position() const { return mPosition; }
        bool position(int pos);
        bool proceed(int size);
        bool hasMore(int size) const { return ((mPosition + size) <= mSize); }
        bool hasMore() const { return hasMore(1); }
        void* pointer();

        void* getWriteBuffer();

        private:
        drConTransmitDiagramStream(const drConTransmitDiagramStream&);
        drConTransmitDiagramStream& operator = (const drConTransmitDiagramStream&);

        protected:
        char* mBuffer;
        int mSize;
        int mPosition;
    };

    /*
     *******************************************************************************
     class drConTransmitDiagramOutStream
     *******************************************************************************
     */

    class drConTransmitDiagramOutStream : public drConTransmitOutStream
    {
        public:
            virtual int getCapacity() const;
            virtual int getSize() const;
            virtual const void* getBuffer() const;
            virtual void dump() const {};

        public:
            virtual bool putInteger(unsigned int val);
            virtual bool putFloat(float val);
            virtual bool putShort(unsigned short val);
            virtual bool putChar(unsigned char val);
            virtual bool putBuffer(const char *val, int size);
            virtual bool putString(const std::string& str);

        public:
            bool position(int pos);
            drConTransmitDiagramOutStream(int size);
            virtual ~drConTransmitDiagramOutStream() {}

        private:
            drConTransmitDiagramStream mStream;
    };

    inline drConTransmitDiagramOutStream::drConTransmitDiagramOutStream(int size)
        : mStream(size)
    {
    }

    inline int drConTransmitDiagramOutStream::getCapacity() const
    {
        return mStream.getCapacity();
    }

    inline int drConTransmitDiagramOutStream::getSize() const
    {
        return mStream.getSize();
    }

    inline const void* drConTransmitDiagramOutStream::getBuffer() const
    {
        return mStream.getBuffer();
    }

    inline bool drConTransmitDiagramOutStream::position(int pos)
    {
        return mStream.position(pos);
    }

    /*
     *******************************************************************************
     class drConTransmitDiagramInStream
     *******************************************************************************
     */

    class drConTransmitDiagramInStream : public drConTransmitInStream
    {
        public:
            virtual int getCapacity() const;
            virtual int getSize() const;
            virtual const void* getBuffer() const;
            virtual void dump() const;

        public:
            virtual bool getInteger(unsigned int *val);
            virtual bool getFloat(float *val);
            virtual bool getShort(unsigned short *val);
            virtual bool getChar(unsigned char *val);
            virtual bool getBuffer(char *val, int size);
            virtual bool getString(std::string& str);

        public:
            void* getWriteBuffer();
            bool position(int pos);

            drConTransmitDiagramInStream(int size);
            virtual ~drConTransmitDiagramInStream() {}

        private:
            drConTransmitDiagramStream mStream;
    };

    inline drConTransmitDiagramInStream::drConTransmitDiagramInStream(int size)
        : mStream(size)
    {
    }

    inline int drConTransmitDiagramInStream::getCapacity() const
    {
        return mStream.getCapacity();
    }

    inline int drConTransmitDiagramInStream::getSize() const
    {
        return mStream.getSize();
    }

    inline const void* drConTransmitDiagramInStream::getBuffer() const
    {
        return mStream.getBuffer();
    }

    inline void* drConTransmitDiagramInStream::getWriteBuffer()
    {
        return mStream.getWriteBuffer();
    }

    inline bool drConTransmitDiagramInStream::position(int pos)
    {
        return mStream.position(pos);
    }
    inline void drConTransmitDiagramInStream::dump() const
    {
        mStream.dump();
    }
} // end namespace dragonBoat
#endif //_DR_CON_TRANSIMIT_SOCKET_STREAM_H__
