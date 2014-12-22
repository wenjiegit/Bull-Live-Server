
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MRTMPCONNECTION_HPP
#define MRTMPCONNECTION_HPP

#include <MThread>

#include "BlsRtmpUrl.hpp"
#include "BlsRtmpProtocol.hpp"

class MTcpSocket;
class BlsRtmpProtocol;
class BlsRtmpMessage;
class BlsRtmpSource;

#define Role_Connection_Publish             0x01
#define Role_Connection_Play                0x02

class BlsRtmpConnection : public MThread, public BlsRtmpProtocolAbstract
{
public:
    BlsRtmpConnection(MObject *parent = 0);
    virtual ~BlsRtmpConnection();

    int run();

    void setUrl(const MString &url);
    void setSocket(MTcpSocket *socket);

    // call back functions
    int onCommand(BlsRtmpMessage *msg, const MString &name, double transactionID, BlsAMF0Any *arg1
                  , BlsAMF0Any *arg2 = NULL, BlsAMF0Any *arg3 = NULL, BlsAMF0Any *arg4 = NULL);

    int onAudio(BlsRtmpMessage *msg);
    int onVideo(BlsRtmpMessage *msg);
    int onMetadata(BlsRtmpMessage *msg);

private:
    int playService();
    int publishService();
    int parseUrl(BlsAMF0Object *obj);
    int closeConnection();

private:
    BlsRtmpUrl m_url;
    MTcpSocket *m_socket;
    BlsRtmpProtocol *m_protocol;
    BlsRtmpSource *m_source;
    int m_role;
    MString m_vhost;
};

#endif // MRTMPCONNECTION_HPP
