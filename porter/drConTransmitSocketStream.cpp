#include "drConTransmitSocketStream.h"

#include <string>
#include <memory.h>
#include "drConSocket.h"
#include "drConLogger.h"
#include <sstream>
#include <stdio.h>

namespace dragonBoat {
    /*
     *******************************************************************************
     class drConTransmitStream
     *******************************************************************************
     */

    int drConTransmitDiagramStream::getCapacity() const
    {
        return mSize;
    }

    int drConTransmitDiagramStream::getSize() const
    {
        return mPosition;
    }

    const void* drConTransmitDiagramStream::getBuffer() const
    {
        return mBuffer;
    }

    void drConTransmitDiagramStream::dump() const
    {
        std::stringstream ss;
        ss << "position: mPosition: " << mPosition;
        ss << "drConTransmitDiagramStream::dump data: ";
        for (int i = 0; i < mSize; i++)
        {
            ss << std::hex << (unsigned int)(mBuffer[i]) << " ";
        }
        drCon::log(ss.str());
    }

    drConTransmitDiagramStream::drConTransmitDiagramStream(int size)
        : mBuffer(NULL)
          , mSize(size)
    {
        if (mSize > 0)
        {
            mBuffer = new char[mSize];
        }

#if 0
        char buf[1024];
        snprintf(buf, sizeof(buf), "drConTransmitDiagramStream::drConTransmitDiagramStream - sizeof(float): %ld", sizeof(float));
        drCon::log(buf);
        snprintf(buf, sizeof(buf), "drConTransmitDiagramStream::drConTransmitDiagramStream - sizeof(long): %ld", sizeof(long));
        drCon::log(buf);
        snprintf(buf, sizeof(buf), "drConTransmitDiagramStream::drConTransmitDiagramStream - sizeof(int): %ld", sizeof(int));
        drCon::log(buf);
        snprintf(buf, sizeof(buf), "drConTransmitDiagramStream::drConTransmitDiagramStream - sizeof(short): %ld", sizeof(short));
        drCon::log(buf);
        snprintf(buf, sizeof(buf), "drConTransmitDiagramStream::drConTransmitDiagramStream - sizeof(char): %ld", sizeof(char));
        drCon::log(buf);
#endif
    }

    drConTransmitDiagramStream::~drConTransmitDiagramStream()
    {
        if (mBuffer != NULL)
        {
            delete []mBuffer;
        }
    }

    bool drConTransmitDiagramStream::position(int pos)
    {
        if (pos < mSize)
        {
            mPosition = pos;
            return true;
        }
        return false;
    }

    bool drConTransmitDiagramStream::proceed(int size)
    {
        int pos = mPosition + size;
        return position(pos);
    }

    void* drConTransmitDiagramStream::pointer()
    {
        return (void*)((char *)mBuffer + mPosition);
    }

    void* drConTransmitDiagramStream::getWriteBuffer()
    {
        return mBuffer;
    }

    /*
     *******************************************************************************
     class drConTransmitDiagramOutStream
     *******************************************************************************
     */

    bool drConTransmitDiagramOutStream::putInteger(unsigned int val)
    {
        unsigned int *ptr = (unsigned int *)mStream.pointer();
        //    int size = sizeof(val);
        int size = drConTransmitStream::E_SIZE_4;
        if ( sizeof(val) != size )
            drCon::log("warning in drConTransmitDiagramOutStream::putInteger, different sizeof(int).");

        if (mStream.hasMore(size))
        {
            //        *ptr = htonl(val);
            uint32_t nvalue = htonl(val);
            memcpy(ptr, &nvalue, size);
            return mStream.proceed(size);
        }

        return false;
    }

    bool drConTransmitDiagramOutStream::putFloat(float val)
    {
        unsigned int *long_ptr = (unsigned int *)(void *)&val;
        return putInteger(*long_ptr);
    }

    bool drConTransmitDiagramOutStream::putShort(unsigned short val)
    {
        unsigned short *ptr = (unsigned short *)mStream.pointer();    
        //    int size = sizeof(val);
        int size = drConTransmitStream::E_SIZE_2;
        if ( sizeof(val) != size )
            drCon::log("warning in drConTransmitDiagramOutStream::putShort, different sizeof(short).");

        if (mStream.hasMore(size))
        {
            //        *ptr = htons(val);
            uint16_t nvalue = htons(val);
            memcpy(ptr, &nvalue, size);
            return mStream.proceed(size);
        }

        return false;
    }

    bool drConTransmitDiagramOutStream::putChar(unsigned char val)
    {
        unsigned char *ptr = (unsigned char *)mStream.pointer();
        //    int size = sizeof(val);
        int size = drConTransmitStream::E_SIZE_1;
        if ( sizeof(val) != size )
            drCon::log("warning in drConTransmitDiagramOutStream::putChar, different sizeof(char).");

        if (mStream.hasMore(size))
        {
            *ptr = (val);
            return mStream.proceed(size);
        }

        return false;
    }

    bool drConTransmitDiagramOutStream::putBuffer(const char *val, int size)
    {
        unsigned char *ptr = (unsigned char *)mStream.pointer();

        if (mStream.hasMore(size))
        {
            memcpy(ptr, val, size);
            return mStream.proceed(size);
        }

        return false;
    }

    bool drConTransmitDiagramOutStream::putString(const std::string& str)
    {
        bool ret_code = true;

        if (ret_code) ret_code = putInteger(str.length());
        if (ret_code) ret_code = putBuffer(str.c_str(), str.length());

        return ret_code;
    }

    /*
     *******************************************************************************
     class drConTransmitDiagramInStream
     *******************************************************************************
     */

    bool drConTransmitDiagramInStream::getInteger(unsigned int *val)
    {
        unsigned int *ptr = (unsigned int *)mStream.pointer();
        //    int size = sizeof(*val);
        int size = drConTransmitStream::E_SIZE_4;
        if ( sizeof(*val) != size )
            drCon::log("warning in drConTransmitDiagramInStream::getInteger, different sizeof(int).");

        if (mStream.hasMore(size))
        {
            *val = ntohl(*ptr);
            mStream.proceed(size);
            return true;
        }

        return false;
    }

    bool drConTransmitDiagramInStream::getFloat(float *val)
    {
        unsigned int *long_ptr = (unsigned int *)val;
        return getInteger(long_ptr);
    }

    bool drConTransmitDiagramInStream::getShort(unsigned short *val)
    {
        unsigned short *ptr = (unsigned short *)mStream.pointer();
        //    int size = sizeof(*val);
        int size = drConTransmitStream::E_SIZE_2;
        if ( sizeof(*val) != size )
            drCon::log("warning in drConTransmitDiagramInStream::getShort, different sizeof(short).");

        if (mStream.hasMore(size))
        {
            *val = ntohs(*ptr);
            mStream.proceed(size);
            return true;
        }

        return false;
    }

    bool drConTransmitDiagramInStream::getChar(unsigned char *val)
    {
        unsigned char *ptr = (unsigned char *)mStream.pointer();
        //    int size = sizeof(*val);
        int size = drConTransmitStream::E_SIZE_1;
        if ( sizeof(*val) != size )
            drCon::log("warning in drConTransmitDiagramInStream::getChar, different sizeof(char).");

        if (mStream.hasMore(size))
        {
            *val = (*ptr);
            mStream.proceed(size);
            return true;
        }

        return false;
    }

    bool drConTransmitDiagramInStream::getBuffer(char *val, int size)
    {
        unsigned char *ptr = (unsigned char *)mStream.pointer();
        if (mStream.hasMore(size))
        {
            memcpy(val, ptr, size);
            mStream.proceed(size);
            return true;
        }

        return false;
    }

    bool drConTransmitDiagramInStream::getString(std::string& str)
    {
        bool ret_code = true;
        char buffer[256] = {0};
        unsigned int len = 0;

        if (ret_code) ret_code = getInteger((unsigned int *)&len);
        if (ret_code)
        {
            ret_code = getBuffer(buffer, len);
            if (ret_code)
            {
                str = buffer;
                return true;
            }
        }

        return ret_code;
    }
} // end namespace dragonBoat
