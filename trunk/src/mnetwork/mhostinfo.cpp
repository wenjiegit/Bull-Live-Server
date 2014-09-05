
/*
 * Copyright (C) wenjie.zhao
 */


#include "mhostinfo.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "mtcpsocket.hpp"

MHostInfo::MHostInfo()
    : _port(0)
{
}

MHostInfo::~MHostInfo()
{

}

MStringList MHostInfo::address()
{
    return _address;
}

int MHostInfo::port()
{
    return _port;
}

MHostInfo MHostInfo::getPeerInfo(int fd)
{
    MHostInfo hostInfo;

    // discovery client information
    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if (getpeername(fd, (sockaddr *)&addr, &addrlen) == -1) {
        merrno = errno;
        return hostInfo;
    }

    // ip v4 or v6
    char buf[INET6_ADDRSTRLEN];
    memset(buf, 0, sizeof(buf));
    if ((inet_ntop(addr.sin_family, &addr.sin_addr, buf, sizeof(buf))) == NULL) {
        merrno = errno;
        return hostInfo;
    }
    hostInfo._address.push_back(buf);

    return hostInfo;
}

MHostInfo MHostInfo::getLocalInfo(int fd)
{
    MHostInfo hostInfo;

    // discovery client information
    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if (getsockname(fd, (sockaddr *)&addr, &addrlen) == -1) {
        merrno = errno;
        return hostInfo;
    }

    // ip v4 or v6
    char buf[INET6_ADDRSTRLEN];
    memset(buf, 0, sizeof(buf));
    if ((inet_ntop(addr.sin_family, &addr.sin_addr, buf, sizeof(buf))) == NULL) {
        merrno = errno;
        return hostInfo;
    }
    hostInfo._address.push_back(buf);

    return hostInfo;
}

MHostInfo MHostInfo::lookupHost(const MString &host)
{
    MHostInfo ret;

    hostent *answer = gethostbyname(host.c_str());
    if (answer == NULL) {
        return ret;
    }

    char ipv4[16];
    memset(ipv4, '\0', sizeof(ipv4));
    switch(answer->h_addrtype)
    {
    case AF_INET:
    case AF_INET6:
        for(char **pptr=answer->h_addr_list; *pptr!=NULL; pptr++) {
            inet_ntop(answer->h_addrtype, *pptr, ipv4, sizeof(ipv4));
            ret._address.push_back(ipv4);
        }
        break;
    default:
        printf("unknown address type\n");
        break;
    }

    return ret;
}
