#ifndef MRTMPPLAYER_HPP
#define MRTMPPLAYER_HPP

#include <MThread>
#include <MHash>

#include "mrtmpprotocol.hpp"

class MTcpSocket;
class MRtmpProtocol;
class BlsRtmpSource;

class MRtmpPlayer : public MThread, public MRtmpProtocolAbstract
{
public:
    MRtmpPlayer(MObject *parent = 0);
    virtual ~MRtmpPlayer();

    void setUrl(const MString &host, mint16 port, const MString &app, const MString &url);

    virtual int run();

    // call back functions
    int onCommand(MRtmpMessage *msg, const MString &name, double transactionID, MAMF0Any *arg1
                  , MAMF0Any *arg2 = NULL, MAMF0Any *arg3 = NULL, MAMF0Any *arg4 = NULL);

    int onAudio(MRtmpMessage *msg);
    int onVideo(MRtmpMessage *msg);
    int onMetadata(MRtmpMessage *msg);

private:
    MString findCommand(double id);
    int connectApp();
    int createStream();
    int play(const MString &streamName);

    int service();

private:
    MTcpSocket *m_socket;
    MRtmpProtocol *m_protocol;
    MString m_url;
    MString m_host;
    mint16 m_port;
    MString m_app;

    MHash<double, MString> m_commandList;

    BlsRtmpSource *m_source;
};

#endif // MRTMPPLAYER_HPP
