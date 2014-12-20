
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
    enum {
        ReadReady   = POLLIN,
        WriteReady  = POLLOUT,
        Error       = POLLPRI,
    };

public:
    MTcpSocket(MObject *parent = 0);
    MTcpSocket(int fd, MObject *parent = 0);
    MTcpSocket(st_netfd_t stfd, MObject *parent = 0);

    virtual ~MTcpSocket();

    /*!
    * used with MTcpSocket(MObject *parent = 0)
    * to create fd.
    */
    int initSocket();

    int connectToHost(const MString &host, muint16 port);
    void close();

    st_netfd_t stFD();
    int osFD();

    int testFeature(mint32 type, mint64 timeout = 0);

    /*!
    * read a line of data.
    */
    int readLine(MString &line, const MString &delimer = "\n");
    int readToLineCache();

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

    // useful util
    int readInt(int &value);

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
    MString m_line;
};

#endif // MSTTCPSOCKET_H
