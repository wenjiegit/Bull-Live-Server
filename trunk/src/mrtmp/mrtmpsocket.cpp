
/*
 * Copyright (C) wenjie.zhao
 * Copyright (C) www.17173.com
 */


#include "mrtmpsocket.hpp"

MRtmpSocket::MRtmpSocket(MObject *parent)
    : MTcpSocket(parent)
{
}

int MRtmpSocket::writev(const iovec *iov, int iov_size, ssize_t *nwrite)
{
    muint64 writeBytes = MTcpSocket::writev(iov, iov_size);
    if (writeBytes == -1) {
        *nwrite = 0;
        return -1;
    }
    *nwrite = writeBytes;

    return 0;
}

int MRtmpSocket::read(void *buf, size_t size, ssize_t *nread)
{
    muint64 readBytes = MTcpSocket::read((char*)buf, size);
    if (readBytes <= 0) {
        *nread = 0;
        return -1;
    }
    *nread = readBytes;

    return 0;
}

int MRtmpSocket::read_fully(void *buf, size_t size, ssize_t *nread)
{
    muint64 readBytes = MTcpSocket::readFully((char*)buf, size);
    if (readBytes <= 0) {
        *nread = 0;
        return -1;
    }
    *nread = readBytes;

    return 0;
}

int MRtmpSocket::write(void *buf, size_t size, ssize_t *nwrite)
{
    muint64 writeBytes = MTcpSocket::write((char*)buf, (mint64)size);
    if (writeBytes == -1) {
        *nwrite = 0;
        return -1;
    }
    *nwrite = writeBytes;

    return 0;
}
