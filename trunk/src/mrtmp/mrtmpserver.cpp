#include "mrtmpserver.hpp"

#include <MLoger>
#include <MHostInfo>

#include "mrtmpconnection.hpp"

MRtmpServer::MRtmpServer(MObject *parent)
    : MTcpServer(parent)
{
}

MRtmpServer::~MRtmpServer()
{

}

int MRtmpServer::newConnection(MTcpSocket *socket)
{
    MStringList address = MHostInfo::getPeerInfo(socket->osFD()).address();
    if (!address.empty()) {
      MString ip = address.front();
      log_trace("a new connection from %s with pid %d", ip.c_str(), (int)getpid());
    }
    MRtmpConnection *conn = new MRtmpConnection;
    socket->setParent(conn);
    conn->setSocket(socket);
    conn->start();
}
