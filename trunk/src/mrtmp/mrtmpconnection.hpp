
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MRTMPCONNECTION_HPP
#define MRTMPCONNECTION_HPP

#include <MThread>

#include "mrtmpurl.hpp"
#include "mrtmpprotocol.hpp"

class MTcpSocket;
class MRtmpProtocol;
class MRtmpMessage;
class MRtmpSource;

#define Role_Connection_Publish             0x01
#define Role_Connection_Play                0x02

class MRtmpConnection : public MThread, public MRtmpProtocolAbstract
{
public:
    MRtmpConnection(MObject *parent = 0);
    virtual ~MRtmpConnection();

    int run();

    void setUrl(const MString &url);
    void setSocket(MTcpSocket *socket);

    // call back functions
    int onCommand(MRtmpMessage *msg, const MString &name, double transactionID, MAMF0Any *arg1
                  , MAMF0Any *arg2 = NULL, MAMF0Any *arg3 = NULL, MAMF0Any *arg4 = NULL);

    int onAudio(MRtmpMessage *msg);
    int onVideo(MRtmpMessage *msg);
    int onMetadata(MRtmpMessage *msg);

private:
    int playService();
    int publishService();
    int parseUrl(MAMF0Object *obj);
    int closeConnection();

private:
    MRtmpUrl m_url;
    MTcpSocket *m_socket;
    MRtmpProtocol *m_protocol;
    MRtmpSource *m_source;
    int m_role;
};

#endif // MRTMPCONNECTION_HPP
