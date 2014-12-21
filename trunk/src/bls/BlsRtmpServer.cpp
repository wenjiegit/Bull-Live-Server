#include "BlsRtmpServer.hpp"

#include <MLoger>
#include <MHostInfo>

#include "BlsRtmpConnection.hpp"

BlsRtmpServer::BlsRtmpServer(MObject *parent)
    : MTcpServer(parent)
{
}

BlsRtmpServer::~BlsRtmpServer()
{

}

int BlsRtmpServer::newConnection(MTcpSocket *socket)
{
    MStringList address = MHostInfo::getPeerInfo(socket->osFD()).address();
    if (!address.empty()) {
      MString ip = address.front();
      log_trace("a new connection from %s with pid %d", ip.c_str(), (int)getpid());
    }
    BlsRtmpConnection *conn = new BlsRtmpConnection;
    socket->setParent(conn);
    conn->setSocket(socket);
    conn->start();
}
