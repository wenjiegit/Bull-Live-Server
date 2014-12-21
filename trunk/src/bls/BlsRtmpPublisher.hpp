#ifndef BLSRTMPPUBLISHER_HPP
#define BLSRTMPPUBLISHER_HPP

#include <MThread>
#include <MHash>

#include "BlsRtmpProtocol.hpp"

class BlsRtmpPublisher : public MThread, public BlsRtmpProtocolAbstract
{
public:
    BlsRtmpPublisher(MObject *parent = 0);
    virtual ~BlsRtmpPublisher();

    virtual int run();

    void setUrl(const MString &url);
    void setHost(const MString &host, muint16 port);

    virtual int onCommand(BlsRtmpMessage *msg, const MString &name, double transactionID, BlsAMF0Any *arg1
                          , BlsAMF0Any *arg2 = NULL, BlsAMF0Any *arg3 = NULL, BlsAMF0Any *arg4 = NULL);

private:
    int service();
    int connectApp();
    MString findCommand(double id);
    int publish();

private:
    MString m_url;
    BlsRtmpProtocol *m_protocol;
    MHash<double, MString> m_commandList;
    MTcpSocket *m_socket;
    MString m_host;
    muint16 m_port;
};

#endif // BLSRTMPPUBLISHER_HPP
