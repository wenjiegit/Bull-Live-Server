
/*
 * Copyright (C) wenjie.zhao
 */


#include "mtcpserver.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <st.h>

#define SERVER_LISTEN_BACKLOG 100
#define DEFAULT_ACCEPT_TIMEOUT 1000000      // 1s

MTcpServer::MTcpServer(MObject *parent)
    : MThread(parent)
    , m_port(-1)
    , m_socket(NULL)
{
}

MTcpServer::~MTcpServer()
{
}

bool MTcpServer::listen(const MString &address, muint16 port)
{
    m_socket = new MTcpSocket(this);
    if (m_socket->initSocket() != E_SUCCESS) {
        return false;
    }

    int fd = m_socket->osFD();

    int reuse_socket = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_socket, sizeof(int)) == -1) {
        merrno = errno;
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (address == "any") {
        addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        addr.sin_addr.s_addr = inet_addr(address.c_str());
    }

    if (bind(fd, (const sockaddr *)&addr, sizeof(sockaddr_in)) == -1) {
        merrno = errno;
        return false;
    }

    if (::listen(fd, SERVER_LISTEN_BACKLOG) == -1) {
        merrno = errno;
        return false;
    }
    m_port = port;
    m_host = address;

    return true;
}

void MTcpServer::close()
{
    if (isRunning()) {
        stop();
        wait();
    }

    if (m_socket) {
        m_socket->close();
        mFree(m_socket);
    }
}

muint16 MTcpServer::port()
{
    return m_port;
}

MString MTcpServer::host()
{
    return m_host;
}

int MTcpServer::newConnection(MTcpSocket *socket)
{
    M_UNUSED(socket);
    return 0;
}

int MTcpServer::run()
{
    st_netfd_t fd = m_socket->stFD();
    if (!fd) {
        return -1;
    }

    while (!RequestStop) {
        st_netfd_t client = st_accept(fd, NULL, NULL, DEFAULT_ACCEPT_TIMEOUT);
        if (!client) {
            continue;
        }

        MTcpSocket *connection = new MTcpSocket(client, this);
        newConnection(connection);
    }

    return 0;
}
