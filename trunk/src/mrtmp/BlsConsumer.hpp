#ifndef MRTMPPOOL_HPP
#define MRTMPPOOL_HPP

#include <MObject>
#include <list>

using namespace std;

class BlsRtmpMessage;
class BlsTimestampCorrector;

class BlsConsumer : public MObject
{
public:
    BlsConsumer(const MString &url, MObject *parent = 0);
    ~BlsConsumer();

    int onVideo(BlsRtmpMessage &msg);
    int onAudio(BlsRtmpMessage &msg);
    int onMetadata(BlsRtmpMessage &msg);

    int onMessage(BlsRtmpMessage &msg);

    list<BlsRtmpMessage> getMessage();

private:
    list<BlsRtmpMessage> m_msgs;
    BlsTimestampCorrector *m_corrector;
    MString m_url;
};

#endif // MRTMPPOOL_HPP
