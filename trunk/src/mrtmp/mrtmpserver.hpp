#ifndef MRTMPSERVER_HPP
#define MRTMPSERVER_HPP

#include <MTcpServer>

class MRtmpServer : public MTcpServer
{
public:
    MRtmpServer(MObject * parent = 0);
    virtual ~MRtmpServer();

    virtual int newConnection(MTcpSocket *socket);
};

#endif // MRTMPSERVER_HPP
