#ifndef MRTMPPLAYER_HPP
#define MRTMPPLAYER_HPP

#include <MThread>
#include <MHash>

#include "BlsRtmpProtocol.hpp"

class MTcpSocket;
class BlsRtmpProtocol;
class BlsRtmpSource;

class BlsRtmpPlayer : public MThread, public BlsRtmpProtocolAbstract
{
public:
    BlsRtmpPlayer(MObject *parent = 0);
    virtual ~BlsRtmpPlayer();

    void setUrl(const MString &host, mint16 port, const MString &app, const MString &url);

    virtual int run();

    // call back functions
    int onCommand(BlsRtmpMessage *msg, const MString &name, double transactionID, BlsAMF0Any *arg1
                  , BlsAMF0Any *arg2 = NULL, BlsAMF0Any *arg3 = NULL, BlsAMF0Any *arg4 = NULL);

    int onAudio(BlsRtmpMessage *msg);
    int onVideo(BlsRtmpMessage *msg);
    int onMetadata(BlsRtmpMessage *msg);

private:
    MString findCommand(double id);
    int connectApp();
    int createStream();
    int play(const MString &streamName);

    int service();

private:
    MTcpSocket *m_socket;
    BlsRtmpProtocol *m_protocol;
    MString m_url;
    MString m_host;
    mint16 m_port;
    MString m_app;

    MHash<double, MString> m_commandList;

    BlsRtmpSource *m_source;
};

#endif // MRTMPPLAYER_HPP
