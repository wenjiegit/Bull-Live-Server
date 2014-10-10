
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MTCPSERVER_HPP
#define MTCPSERVER_HPP

#include <MString>
#include <MThread>
#include <MTcpSocket>

class MTcpServer : public MThread
{
public:
    MTcpServer(MObject * parent = 0);
    virtual ~MTcpServer();

    bool listen(const MString & address,  muint16 port);
    void close();

    muint16 port();
    MString host();

    virtual int newConnection(MTcpSocket *socket);

protected:
    virtual int run();

private:
    MTcpSocket *m_socket;
    muint16 m_port;
    MString m_host;
};

#endif // MTCPSERVER_HPP
