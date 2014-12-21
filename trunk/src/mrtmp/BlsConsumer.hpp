#ifndef MRTMPPOOL_HPP
#define MRTMPPOOL_HPP

#include <MObject>
#include <list>

using namespace std;

class MRtmpMessage;
class MRtmpTimestampCorrector;

class BlsConsumer : public MObject
{
public:
    BlsConsumer(const MString &url, MObject *parent = 0);
    ~BlsConsumer();

    int onVideo(MRtmpMessage &msg);
    int onAudio(MRtmpMessage &msg);
    int onMetadata(MRtmpMessage &msg);

    int onMessage(MRtmpMessage &msg);

    list<MRtmpMessage> getMessage();

private:
    list<MRtmpMessage> m_msgs;
    MRtmpTimestampCorrector *m_corrector;
    MString m_url;
};

#endif // MRTMPPOOL_HPP
