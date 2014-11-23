#ifndef BLSRTMPPUBLISHER_HPP
#define BLSRTMPPUBLISHER_HPP

#include <MThread>
#include <MHash>

#include "mrtmpprotocol.hpp"

class BlsRtmpPublisher : public MThread, public MRtmpProtocolAbstract
{
public:
    BlsRtmpPublisher(MObject *parent = 0);
    virtual ~BlsRtmpPublisher();

    virtual int run();

    void setUrl(const MString &url);
    void setHost(const MString &host, muint16 port);

    virtual int onCommand(MRtmpMessage *msg, const MString &name, double transactionID, MAMF0Any *arg1
                          , MAMF0Any *arg2 = NULL, MAMF0Any *arg3 = NULL, MAMF0Any *arg4 = NULL);

private:
    int service();
    int connectApp();
    MString findCommand(double id);
    int publish();

private:
    MString m_url;
    MRtmpProtocol *m_protocol;
    MHash<double, MString> m_commandList;
    MTcpSocket *m_socket;
    MString m_host;
    muint16 m_port;
};

#endif // BLSRTMPPUBLISHER_HPP
