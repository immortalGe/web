#include "drConSocket.h"

#include <sstream>
#include <memory.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "drConLogger.h"

namespace dragonBoat {
    static drSocketLib theSocketLib;

    drSocketLib::drSocketLib()
    {
#if defined(_WIN32)
        WORD    ver = MAKEWORD(2, 2);
        WSADATA data;
        WSAStartup(ver, &data);
#endif
    }

    drSocketLib::~drSocketLib()
    {
#if defined(_WIN32)
        WSACleanup();
#endif
    }


    std::string drConSocketAddrConverter::toString(const sockaddr_in& addr)
    {
        std::stringstream sss;

        sss << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port);

//        const unsigned char* bytes = (unsigned char*)(&addr.sin_addr);
//        sss << (short)bytes[0] << "." << (short)bytes[1] << "."
//            << (short)bytes[2] << "." << (short)bytes[3];
//        sss << ":" << ntohs(addr.sin_port);

        return sss.str();
    }

    bool drConSocketAddrConverter::toSockAddr(const std::string& str, sockaddr_in& addr)
    {
        bool bResult = false;
        size_t comm = str.find(":");
        memset(&addr, 0, sizeof(addr));

        if (comm != std::string::npos)
        {
            std::string str_addr = str.substr(0, comm);
            std::string str_port = str.substr(++comm);

//            struct in_addr ia;
//            ia.s_addr = inet_addr(str_addr.c_str());
            if ( inet_aton(str_addr.c_str(), &addr.sin_addr) )
            {
                addr.sin_port = htons( (uint16_t)atoi(str_port.c_str()) );
                addr.sin_family = AF_INET;
                bResult = true;
            }
        }

        return bResult;
    }

    drConSocket::drConSocket()
        : mSocket(drInvalidSocket)
    {
        mSocket = ::socket(AF_INET, SOCK_DGRAM, 0);

        if (mSocket != drInvalidSocket)
        {
            int opt = 1;
            if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) >= 0)
            {
                drCon::log(__FUNCTION__, "success to set the option SO_REUSEADDR.");
            }

#if defined(_WIN32)
            int tv = 10;
#else
            struct timeval tv; 
            tv.tv_sec = 0;
            tv.tv_usec = 1000;
#endif
            if (setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv)) >= 0)
            {
                drCon::log(__FUNCTION__, "success to create diagram socket.");
            }
        }
        else
        {
            drCon::log(__FUNCTION__, "failed to create the socket.");
        }
    }

    drConSocket::~drConSocket()
    {
        shutdown();
    }

    bool drConSocket::workable()
    {
        return (mSocket != drInvalidSocket);
    }

    void drConSocket::shutdown()
    {
        if (mSocket != drInvalidSocket)
        {
            drCloseSocket(mSocket);
            mSocket = drInvalidSocket;
            drCon::log(__FUNCTION__, "destroyed the socket.");
        }
    }

    int drConSocket::sendTo(const void *buffer, int length, const sockaddr_in& addr_in)
    {
        if (!workable() || (buffer == NULL) || (length <= 0))
            return 0;

        drSocklen_t addr_len = sizeof(addr_in);
        int send_bytes = sendto(mSocket,(const char*)buffer, length, 0,
                (const sockaddr *)&addr_in, addr_len);

        if (send_bytes == length)
        {
            std::stringstream sss;
            sss << "sent (" << send_bytes << ") bytes to "
                << drConSocketAddrConverter::toString(addr_in) << ".";
            std::string log = sss.str();
            //drCon::log(__FUNCTION__, log);
        }

        return send_bytes;
    }

    int drConSocket::recvFrom(void *buffer, int length, sockaddr_in& addr_in)
    {
        if (!workable() || (buffer == NULL) || (length <= 0))
            return 0;

        drSocklen_t addr_len = sizeof(addr_in);
        char buf[1024];

        fd_set fds;
        struct timeval tv;
        int retval;

        FD_ZERO(&fds);
        FD_SET(mSocket, &fds);

        tv.tv_sec = 0;
        tv.tv_usec = 1000;

        retval = select(mSocket + 1, &fds, NULL, NULL, &tv);

        int recv_bytes = 0;
        if (retval > 0)
        {
            recv_bytes = recvfrom(mSocket, (char*)buffer, length, 0,
                    (sockaddr *)&addr_in, &addr_len);

            if (recv_bytes > 0)
            {
                std::stringstream sss;
                sss << "recv (" << recv_bytes << ") bytes from "
                    << drConSocketAddrConverter::toString(addr_in) << ".";
                std::string log = sss.str();
                //            drCon::log(__FUNCTION__, log);
            }
            else if (recv_bytes < 0)
            {
                snprintf(buf, sizeof(buf), "errno: %d", errno);
                drCon::log(buf);
            }

        }
        else if (retval < 0)
        {
            snprintf(buf, sizeof(buf), "select errno: %d", errno);
            drCon::log(buf);
        }

        return recv_bytes;
    }

    class drConServerSocket : public drConSocket
    {
        public:
            drConServerSocket(unsigned short port)
            {
                if (mSocket == drInvalidSocket)
                {
                    return;
                }

                std::stringstream sss;

                struct sockaddr_in sock_addr = {0};
                sock_addr.sin_family = AF_INET;
                sock_addr.sin_port = htons(port);
                sock_addr.sin_addr.s_addr = INADDR_ANY;

                if (bind(mSocket, (struct sockaddr *)&sock_addr, sizeof(sock_addr)))
                {
                    sss << "failure to bind socket address at port " << port << ".";
                    shutdown();
                }
                else
                {
                    sss << "success to bind socket address at port " << port << ".";

                    int opt = 1;
                    setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
                }

                std::string log = sss.str();
                drCon::log(__FUNCTION__, log);
                drCon::log(__FUNCTION__, "a server socket created.");
            }
    };

    class drConClientSocket : public drConSocket
    {
        public:
            drConClientSocket()
            {
                int opt = 1;
                setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
                drCon::log(__FUNCTION__, "a client socket created.");
            }
    };

    drConSocket * drConSocket::createClientSocket()
    {
        return new drConClientSocket();
    }
    drConSocket * drConSocket::createServerSocket(unsigned short port)
    {
        return new drConServerSocket(port);
    }
} // end namespace dragonBoat
