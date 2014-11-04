#include "drConLogger.h"
#include "drConLoggerImpl.h"

#if defined(ANDROID) || defined(__ANDROID__)
#include <android/log.h>
#endif

// this should be defined by build script
#ifndef LOGGER_TAG
#define LOGGER_TAG "DragonBone"
#endif

namespace dragonBoat {
    char stream_buf[STREAM_BUF_SIZE] = {0};
    // Logger Implementation
#if defined(ANDROID) || defined(__ANDROID__)
    class AndroidLoggerImpl : public drConLoggerImpl
    {
        public:
            virtual void log(const std::string& message) const;
    };
    ////////////////////////////////////////////////////////////////
    // Implementation of AndroidLoggerImpl
    void AndroidLoggerImpl::log(const std::string& message) const
    {
        __android_log_print(ANDROID_LOG_INFO, LOGGER_TAG, "%s", message.c_str());
    }
#else
    class ConsoleLoggerImpl : public drConLoggerImpl
    {
        public:
            virtual void log(const std::string& message) const;
    };
    ////////////////////////////////////////////////////////////////
    // Implementation of ConsoleLoggerImpl
    void ConsoleLoggerImpl::log(const std::string& message) const
    {
        std::cout << message << std::endl;
    }
#endif //defined(ANDROID)

    /*
     *******************************************************************************
     class drConLogger
     *******************************************************************************
     */
    class drConLogger : public drConRefObject
    {
        public:
            void log(const std::string& message) const;
            void setLoggerImpl(drConLoggerImpl *impl);

        public:
            friend class drConLoggerMan;

            drConLogger();
            virtual ~drConLogger();
        private:
            drConLogger(const drConLogger&);
            drConLogger& operator = (const drConLogger&);

            drConLoggerImpl * mImpl;
    };

    /*
     *******************************************************************************
     class drConLoggerMan
     *******************************************************************************
     */
    class drConLoggerMan
    {
        public:
            static drConLoggerMan& get();
            ~drConLoggerMan();
            void log(const std::string& message) const;

        protected:
            drConLoggerMan();
            void setLoggerImpl(drConLoggerImpl *impl);
        private:
            drConLogger mLogger;
            drConLoggerImpl * mLoggerImp;
    };

    /////////////////////////////////////////////////////////////
    // Implementation of drConLogger
    drConLogger::drConLogger()
    {
    }

    drConLogger::~drConLogger()
    {
        std::cout << "drConLogger: byebye..." << std::endl;
    }

    void drConLogger::log(const std::string& message) const
    {
        if ( mImpl )
        {
            mImpl->log(message);
        }
        else
        {
            std::cout << message << std::endl;
        }
    }

    void drConLogger::setLoggerImpl(drConLoggerImpl *impl)
    {
        mImpl = impl;
    }

    /*
     *******************************************************************************
     class drConLoggerMan implementation
     *******************************************************************************
     */
    drConLoggerMan::drConLoggerMan()
    {
#if defined(ANDROID)
        mLoggerImp = new AndroidLoggerImpl();
#else
        mLoggerImp = new ConsoleLoggerImpl();
#endif
        setLoggerImpl(mLoggerImp);
        mLogger.log("drConLoggerman initialized...");
    }

    drConLoggerMan::~drConLoggerMan()
    {
#if defined(ANDROID)
        __android_log_print(ANDROID_LOG_INFO, LOGGER_TAG, "drConLoggerman freed...");
#else
        std::cout << "drConLoggerMan: freed..." << std::endl;
#endif //defined(ANDROID)
        setLoggerImpl(0);
        delete mLoggerImp;
    }

    void drConLoggerMan::log(const std::string& message) const
    {
        mLogger.log(message);
    }

    void drConLoggerMan::setLoggerImpl(drConLoggerImpl *impl)
    {
        mLogger.setLoggerImpl(impl);
    }

    drConLoggerMan& drConLoggerMan::get()
    {
        static drConLoggerMan _gOne;
        return _gOne;
    }

    /////////////////////////////////////////////////////////////
    // implementation of drCon
    void drCon::log(const std::string& message)
    {
        drConLoggerMan::get().log(message);
    }

//    void drCon::setLoggerImpl(drConLoggerImpl *impl)
//    {
//        drConLoggerMan::get().setLoggerImpl(impl);
//    }

    void drCon::log(const char *tag, const char *message)
    {
        std::string str;
        str = tag ? tag : "";
        str.append(": ").append(message ? message : "");

        drConLoggerMan::get().log(str);
    }

    void drCon::log(const char *tag, const std::string& message)
    {
        std::string str;
        str = tag ? tag : "";
        str.append(": ").append(message);

        drConLoggerMan::get().log(str);
    }
} // end namespace dragonBoat
