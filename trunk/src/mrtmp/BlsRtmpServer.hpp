#ifndef MRTMPSERVER_HPP
#define MRTMPSERVER_HPP

#include <MTcpServer>

class BlsRtmpServer : public MTcpServer
{
public:
    BlsRtmpServer(MObject * parent = 0);
    virtual ~BlsRtmpServer();

    virtual int newConnection(MTcpSocket *socket);
};

#endif // MRTMPSERVER_HPP
