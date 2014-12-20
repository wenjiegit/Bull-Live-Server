#include "mrtmpsource.hpp"
#include "mrtmppool.hpp"
#include "mrtmpprotocol.hpp"
#include "mflashvideoinfo.hpp"
#include "BlsConf.hpp"
#include "BlsServerSelector.hpp"
#include "BlsChildChannel.hpp"
#include "BlsUtils.hpp"
#include "BlsRtmpPublisher.hpp"

#include <MLoger>

MHash<MString, MRtmpSource*> MRtmpSource::m_sources;

MRtmpSource::MRtmpSource(const MString &url, MObject *parent)
    : MObject(parent)
    , m_videoSh(NULL)
    , m_audioSh(NULL)
    , m_metadata(NULL)
{
    m_url = url;
    m_publisher = NULL;
}

MRtmpSource::~MRtmpSource()
{
}

int MRtmpSource::onVideo(MRtmpMessage &msg)
{
    dispatch(msg);
    addToGop(msg);

    if (MFlashVideoInfo::videoIsSequenceHeader(msg.payload)) {
        mFree(m_videoSh);
        m_videoSh = new MRtmpMessage;
        *m_videoSh = msg;

        log_trace("video SH update.");
    }

    return E_SUCCESS;
}

int MRtmpSource::onAudio(MRtmpMessage &msg)
{
    dispatch(msg);
    addToGop(msg);

    if (MFlashVideoInfo::audioIsSequenceHeader(msg.payload)) {
        mFree(m_audioSh);
        m_audioSh = new MRtmpMessage;
        *m_audioSh = msg;

        log_trace("audio SH update.");
    }

    return E_SUCCESS;
}

int MRtmpSource::onMetadata(MRtmpMessage &msg)
{
    dispatch(msg);

    mFree(m_metadata);
    m_metadata = new MRtmpMessage;
    *m_metadata = msg;

    log_trace("metadata update.");

    return E_SUCCESS;
}

int MRtmpSource::onPublish()
{
    int ret = E_SUCCESS;

    int role = BlsConf::instance()->processRole();
    if (role == Process_Role_Child) {
        m_publisher = new BlsRtmpPublisher;

        int port = BlsServerSelector::instance()->lookUp(m_url);

        m_publisher->setHost("127.0.0.1", port);
        m_publisher->setUrl(m_url);
        m_publisher->start();
    }

    return ret;
}

int MRtmpSource::onUnPublish()
{
    // do clean
    m_sources.erase(m_url);
    mFree(m_videoSh);
    mFree(m_audioSh);
    mFree(m_metadata);

    if (m_publisher &&m_publisher->isRunning()) {
        m_publisher->stop();
        m_publisher->wait();
        mFree(m_publisher);
    }

    return E_SUCCESS;
}

void MRtmpSource::addPool(MRtmpPool *pool)
{
    m_pools.push_back(pool);

    // push video sh, audo sh, metadata
    if (m_videoSh) {
        pool->onMessage(*m_videoSh);
    }

    if (m_audioSh) {
        pool->onMessage(*m_audioSh);
    }

    if (m_metadata) {
        pool->onMessage(*m_metadata);
    }

//    fastGop(pool);
#if 0
    // push gop data
    list<MRtmpMessage>::iterator iter;
    for (iter = m_gop.begin(); iter != m_gop.end(); ++iter) {
        MRtmpMessage &msg = *iter;
        pool->onMessage(msg);
    }
#endif
}

void MRtmpSource::removePool(MRtmpPool *pool)
{
    list<MRtmpPool *>::iterator iter;
    for (iter = m_pools.begin(); iter != m_pools.end(); ++iter) {
        MRtmpPool *rp = *iter;
        if (rp == pool) {
            m_pools.erase(iter);
            break;
        }
    }
}

MRtmpSource *MRtmpSource::findSource(const MString &url)
{
    if (m_sources.contains(url)) {
        return m_sources[url];
    }

    MRtmpSource *source = new MRtmpSource(url);
    m_sources[url] = source;

    return source;
}

int MRtmpSource::acquire(const MString &url, bool &res)
{
    int ret = E_SUCCESS;

    if ((ret = g_cchannel->checkSameStream(url, res)) != E_SUCCESS) {
        return ret;
    }

    if (!res) {
        if ((ret = g_cchannel->informStreamUsed(url)) != E_SUCCESS) {
            return ret;
        }
    }

    return ret;
}

void MRtmpSource::addToGop(MRtmpMessage &msg)
{
    if (MFlashVideoInfo::videoIsKeyFrame(msg.payload)) {
        // clean gop
        m_gop.clear();
    }

    m_gop.push_back(msg);
}

int MRtmpSource::dispatch(MRtmpMessage &msg)
{
    list<MRtmpPool *>::iterator iter;
    for (iter = m_pools.begin(); iter != m_pools.end(); ++iter) {
        MRtmpPool *rp = *iter;
        rp->onMessage(msg);
    }

    return E_SUCCESS;
}

int MRtmpSource::fastGop(MRtmpPool *pool)
{
    int ts = 0;
    int index = 0;
    list<MRtmpMessage>::iterator iter;
    for (iter = m_gop.begin(); iter != m_gop.end(); ++iter) {
        MRtmpMessage msg = *iter;
        if (!msg.isVideo()) continue;

        if (index % 4 != 0) continue;

        msg.header.timestamp = ts;
        ts += 10;

        pool->onMessage(msg);
    }
}
