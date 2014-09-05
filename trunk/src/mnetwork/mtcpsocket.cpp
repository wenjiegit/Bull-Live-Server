
/*
 * Copyright (C) wenjie.zhao
 */


#include "mtcpsocket.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <merrno.hpp>

#include "mtcpsocket.hpp"
#include "mhostinfo.hpp"

MTcpSocket::MTcpSocket(MObject *parent)
    : MObject(parent)
{
    m_osHandle = socket(AF_INET, SOCK_STREAM, 0);
    m_stHandle = st_netfd_open_socket(m_osHandle);

    init();
}

MTcpSocket::MTcpSocket(int fd, MObject *parent)
    : MObject(parent)
    , m_osHandle(fd)
{
    m_stHandle = st_netfd_open_socket(m_osHandle);

    init();
}

MTcpSocket::MTcpSocket(st_netfd_t stfd, MObject *parent)
    : MObject(parent)
    , m_stHandle(stfd)
{
    m_osHandle = st_netfd_fileno(m_stHandle);

    init();
}

MTcpSocket::~MTcpSocket()
{
    if (m_stHandle) {
        st_netfd_close(m_stHandle);
    }

    if (m_osHandle > 0) {
        ::close(m_osHandle);
    }
}

int MTcpSocket::connectToHost(const MString &host, muint16 port)
{
    MStringList ips = MHostInfo::lookupHost(host).address();
    if (ips.isEmpty()) {
        return -1;
    }
    MString ip = ips.front();

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (st_connect(m_stHandle, (const struct sockaddr *)&addr, sizeof(sockaddr_in), ST_UTIME_NO_TIMEOUT) == -1) {
        merrno = errno;
        return -1;
    }

    return E_SUCCESS;
}

void MTcpSocket::close()
{
    if (!m_stHandle) {
        st_netfd_close(m_stHandle);
    }
    if (m_osHandle > 0) {
        ::close(m_osHandle);
    }
    m_stHandle = NULL;
    m_osHandle = -1;
}

st_netfd_t MTcpSocket::stFD()
{
    return m_stHandle;
}

int MTcpSocket::osFD()
{
    return m_osHandle;
}

MString MTcpSocket::readLine()
{
    // TODO imp
    return "not supported yet.";
}

void MTcpSocket::setRecvTimeout(muint64 recvTimeout)
{
    m_recvTimeout = recvTimeout;
}

void MTcpSocket::setSendTimeout(muint64 sendTimeout)
{
    m_sendTimeout = sendTimeout;
}

mint64 MTcpSocket::read(char *data, mint64 maxSize)
{
    ssize_t readCount = st_read(m_stHandle, data, maxSize, m_recvTimeout);
    if (readCount == -1) {
        merrno = errno;
    } else if (readCount == 0) {
        merrno = ECONNRESET;
    }
    m_recvBytes += readCount;

    return readCount;
}

mint64 MTcpSocket::readFully(char *data, mint64 maxSize)
{
    ssize_t readCount = st_read_fully(m_stHandle, data, maxSize, m_recvTimeout);
    if (readCount == -1) {
        merrno = errno;
    } else if (readCount == 0) {
        merrno = ECONNRESET;
    }
    m_recvBytes += readCount;

    return readCount;
}

mint64 MTcpSocket::write(const char *data, mint64 maxSize)
{
    ssize_t writeCount = st_write(m_stHandle, data, maxSize, m_sendTimeout);
    if (writeCount == -1) {
        merrno = errno;
    }
    m_sendBytes += writeCount;

    return writeCount;
}

mint64 MTcpSocket::write(const MString &data)
{
    return write(data.data(), data.size());
}

mint64 MTcpSocket::writev(const iovec *iov, int iov_size)
{
    ssize_t writeCount =  st_writev(m_stHandle, iov, iov_size, m_sendTimeout);
    if (writeCount == -1) {
        merrno = errno;
    }
    m_sendBytes += writeCount;

    return writeCount;
}

void MTcpSocket::init()
{
    m_sendBytes = 0;
    m_recvBytes = 0;

    m_recvTimeout = ST_UTIME_NO_TIMEOUT;
    m_sendTimeout = ST_UTIME_NO_TIMEOUT;
}
