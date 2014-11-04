#ifndef __DR_CON_SOCKET_H__
#define __DR_CON_SOCKET_H__

#include <string>
#include "drConRefObject.h"

#if defined(_WIN32)

#include <winsock2.h>
#define drSocket_t        SOCKET
#define drInvalidSocket   INVALID_SOCKET
#define drSocklen_t       int
#define drCloseSocket(s)  closesocket(s)
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define drSocket_t        int
#define drInvalidSocket   -1
#define drSocklen_t       socklen_t
#define drCloseSocket(s)  ::close(s)
#endif

namespace dragonBoat {
    class drSocketLib
    {
        public:
            drSocketLib();
            ~drSocketLib();
    };

    class drConSocket : public drConRefObject
    {
        public:
            drConSocket();
            virtual ~drConSocket();

        private:
            drConSocket(const drConSocket&);
            drConSocket& operator = (const drConSocket&);

        public:
            static drConSocket *createClientSocket();
            static drConSocket *createServerSocket(unsigned short port);

            void shutdown();
            bool workable();

            int sendTo(const void *buffer, int length, const sockaddr_in& addr_in);
            int recvFrom(void *buffer, int length, sockaddr_in& addr_in);

        protected:
            drSocket_t mSocket;
    };

    class drConSocketAddrConverter
    {
        public:
            static std::string toString(const sockaddr_in& addr);
            static bool toSockAddr(const std::string& str, sockaddr_in& addr);
    };
} // end namespace dragonBoat
#endif //__DR_CON_SOCKET_H__
