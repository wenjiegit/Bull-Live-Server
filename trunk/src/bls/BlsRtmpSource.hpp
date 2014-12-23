#ifndef MRTMPSOURCE_HPP
#define MRTMPSOURCE_HPP

#include <MObject>
#include <MHash>
#include <list>

using namespace std;

class BlsRtmpMessage;
class BlsConsumer;
class BlsRtmpPublisher;
class BlsFlvRecoder;

class BlsRtmpSource : public MObject
{
public:
    BlsRtmpSource(const MString &url, MObject *parent = 0);
    ~BlsRtmpSource();

    int onVideo(BlsRtmpMessage &msg);
    int onAudio(BlsRtmpMessage &msg);
    int onMetadata(BlsRtmpMessage &msg);

    int onPublish(const MString &vhost);
    int onUnPublish();

    void addPool(BlsConsumer *pool);
    void removePool(BlsConsumer *pool);

    static BlsRtmpSource * findSource(const MString &url);

    /*!
        return whether the source is in use.
    */
    int acquire(const MString &url, bool &res);

    /*!
        release @url stream
    */
    int release(const MString &url);

private:
    void addToGop(BlsRtmpMessage &msg);
    int dispatch(BlsRtmpMessage &msg);
    int fastGop(BlsConsumer *pool);
    void release();

private:
    list<BlsRtmpMessage> m_gop;
    list<BlsConsumer *> m_pools;

    static MHash<MString, BlsRtmpSource*> m_sources;

    BlsRtmpMessage *m_videoSh;
    BlsRtmpMessage *m_audioSh;
    BlsRtmpMessage *m_metadata;
    MString m_url;
    BlsRtmpPublisher *m_publisher;
    bool m_isActive;
    int m_lockFd;

    BlsFlvRecoder *m_recoder;
};

#endif // MRTMPSOURCE_HPP
