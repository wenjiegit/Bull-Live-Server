
/*
 * Copyright (C) wenjie.zhao
 * Copyright (C) www.17173.com
 */


#ifndef MRTMPSOCKET_H
#define MRTMPSOCKET_H

#include <MTcpSocket>
#include "srs_protocol_io.hpp"

class MRtmpSocket : public MTcpSocket, public ISrsProtocolReaderWriter
{
public:
    MRtmpSocket(MObject *parent = 0);

    inline void set_recv_timeout(int64_t timeout_us) { setRecvTimeout(timeout_us); }
    inline int64_t get_recv_timeout() { return recvTimeout(); }
    inline int64_t get_recv_bytes() { return recvBytes(); }

    inline void set_send_timeout(int64_t timeout_us) { setSendTimeout(timeout_us); }
    inline int64_t get_send_timeout() { return sendTimeout(); }
    inline int64_t get_send_bytes() { return sendBytes(); }

    int writev(const iovec *iov, int iov_size, ssize_t* nwrite);

    inline bool is_never_timeout(int64_t timeout_us) { return false; }

    virtual int read(void* buf, size_t size, ssize_t* nread);
    int read_fully(void* buf, size_t size, ssize_t* nread);
    int write(void* buf, size_t size, ssize_t* nwrite);
};

#endif // MRTMPSOCKET_H
