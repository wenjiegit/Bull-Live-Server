
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MSTTCPSOCKET_H
#define MSTTCPSOCKET_H

#include <mglobal.hpp>

#include <MObject>
#include <MString>

using namespace std;
class MTcpSocket;

class MTcpSocket : public MObject
{
public:
    MTcpSocket(MObject *parent = 0);
    MTcpSocket(int fd, MObject *parent = 0);
    MTcpSocket(st_netfd_t stfd, MObject *parent = 0);

    virtual ~MTcpSocket();

    int connectToHost(const MString &host, muint16 port);
    void close();

    st_netfd_t stFD();
    int osFD();

    /*!
    * read a line of data.
    */
    MString readLine();

    // micro seconds :  us
    void setRecvTimeout(muint64 recvTimeout);
    void setSendTimeout(muint64 sendTimeout);

    inline muint64 recvTimeout() { return m_recvTimeout; }
    inline muint64 sendTimeout() { return m_sendTimeout; }

    inline muint64 sendBytes() { return m_sendBytes; }
    inline muint64 recvBytes() { return m_recvBytes; }

    // for read
    virtual mint64 read(char *data, mint64 maxSize);
    virtual mint64 readFully(char *data, mint64 maxSize);

    // for write
    virtual mint64 write(const char * data, mint64 maxSize);
    virtual mint64 write(const MString &data);

    virtual mint64 writev(const iovec *iov, int iov_size);

private:
    void init();

private:
    st_netfd_t m_stHandle;
    int m_osHandle;

    muint64 m_recvTimeout;
    muint64 m_sendTimeout;

    MString m_recvBuffer;
    muint64 m_sendBytes;
    muint64 m_recvBytes;
};

#endif // MSTTCPSOCKET_H
