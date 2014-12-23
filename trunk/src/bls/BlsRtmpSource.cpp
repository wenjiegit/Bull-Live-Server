#include "BlsRtmpSource.hpp"
#include "BlsConsumer.hpp"
#include "BlsRtmpProtocol.hpp"
#include "BlsFlashVideoInfo.hpp"
#include "BlsConf.hpp"
#include "BlsServerSelector.hpp"
#include "BlsChildChannel.hpp"
#include "BlsUtils.hpp"
#include "BlsRtmpPublisher.hpp"
#include "BlsFlvRecoder.hpp"
#include "BlsRtmpUrl.hpp"

#include <MLoger>

MHash<MString, BlsRtmpSource*> BlsRtmpSource::m_sources;

BlsRtmpSource::BlsRtmpSource(const MString &url, MObject *parent)
    : MObject(parent)
    , m_videoSh(NULL)
    , m_audioSh(NULL)
    , m_metadata(NULL)
{
    m_url = url;
    m_publisher = NULL;
    m_recoder = NULL;
}

BlsRtmpSource::~BlsRtmpSource()
{
}

int BlsRtmpSource::onVideo(BlsRtmpMessage &msg)
{
    dispatch(msg);
    addToGop(msg);

    if (BlsFlashVideoInfo::videoIsSequenceHeader(msg.payload)) {
        mFree(m_videoSh);
        m_videoSh = new BlsRtmpMessage;
        *m_videoSh = msg;

        log_trace("video SH update.");
    }

    return E_SUCCESS;
}

int BlsRtmpSource::onAudio(BlsRtmpMessage &msg)
{
    dispatch(msg);
    addToGop(msg);

    if (BlsFlashVideoInfo::audioIsSequenceHeader(msg.payload)) {
        mFree(m_audioSh);
        m_audioSh = new BlsRtmpMessage;
        *m_audioSh = msg;

        log_trace("audio SH update.");
    }

    return E_SUCCESS;
}

int BlsRtmpSource::onMetadata(BlsRtmpMessage &msg)
{
    dispatch(msg);

    mFree(m_metadata);
    m_metadata = new BlsRtmpMessage;
    *m_metadata = msg;

    log_trace("metadata update.");

    return E_SUCCESS;
}

int BlsRtmpSource::onPublish(const MString &vhost)
{
    int ret = E_SUCCESS;

    int role = BlsConf::instance()->processRole();
    if (role == Process_Role_Worker) {
        m_publisher = new BlsRtmpPublisher;

        int port = BlsServerSelector::instance()->lookUp(m_url);

        m_publisher->setHost("127.0.0.1", port);
        m_publisher->setUrl(m_url);
        m_publisher->start();
    } else if (role == Process_Role_BackSource) {
        DvrInfo info = BlsConf::instance()->getDvrInfo(vhost);
        if (info.enabled) {
            BlsRtmpUrl rtmpUrl(m_url);
            MString fileName = info.path.trimmed() + "/" + rtmpUrl.stream() + ".flv";
            fileName.replace("//", "/");

            log_trace("start record file : %s", fileName.c_str());

            m_recoder = new BlsFlvRecoder;
            m_recoder->setFileName(fileName);
            if (m_recoder->start() != E_SUCCESS) {
                log_error("start record file failed: %s, so dvr will be disabled.", fileName.c_str());
                mFree(m_recoder);
            }

            // push video sh, audo sh, metadata
            if (m_videoSh) {
                m_recoder->onMessage(m_videoSh);
            }

            if (m_audioSh) {
                m_recoder->onMessage(m_audioSh);
            }

            if (m_metadata) {
                m_recoder->onMessage(m_metadata);
            }

            // push gop data
            list<BlsRtmpMessage>::iterator iter;
            for (iter = m_gop.begin(); iter != m_gop.end(); ++iter) {
                BlsRtmpMessage &msg = *iter;
                m_recoder->onMessage(&msg);
            }
        }
    }

    return ret;
}

int BlsRtmpSource::onUnPublish()
{
    int ret = E_SUCCESS;

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

    if (processIsWorker()) {
        if ((ret = release(m_url)) != E_SUCCESS) {
            return ret;
        }
    }

    if (processIsBackSource()) {
        log_trace("dvr stopped.");
        mFree(m_recoder);
    }

    return E_SUCCESS;
}

void BlsRtmpSource::addPool(BlsConsumer *pool)
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

void BlsRtmpSource::removePool(BlsConsumer *pool)
{
    list<BlsConsumer *>::iterator iter;
    for (iter = m_pools.begin(); iter != m_pools.end(); ++iter) {
        BlsConsumer *rp = *iter;
        if (rp == pool) {
            m_pools.erase(iter);
            break;
        }
    }
}

BlsRtmpSource *BlsRtmpSource::findSource(const MString &url)
{
    if (m_sources.contains(url)) {
        return m_sources[url];
    }

    BlsRtmpSource *source = new BlsRtmpSource(url);
    m_sources[url] = source;

    return source;
}

int BlsRtmpSource::acquire(const MString &url, bool &res)
{
    int ret = E_SUCCESS;

    if ((ret = g_cchannel->checkSameStream(url, res)) != E_SUCCESS) {
        return ret;
    }

    if (!res) {
        if ((ret = g_cchannel->informStreamPublished(url)) != E_SUCCESS) {
            return ret;
        }
    }

    return ret;
}

int BlsRtmpSource::release(const MString &url)
{
    int ret = E_SUCCESS;

    if ((ret = g_cchannel->informStreamUnPublished(url)) != E_SUCCESS) {
        return ret;
    }

    return ret;
}

void BlsRtmpSource::addToGop(BlsRtmpMessage &msg)
{
    if (BlsFlashVideoInfo::videoIsKeyFrame(msg.payload)) {
        // clean gop
        m_gop.clear();
    }

    m_gop.push_back(msg);
}

int BlsRtmpSource::dispatch(BlsRtmpMessage &msg)
{
    list<BlsConsumer *>::iterator iter;
    for (iter = m_pools.begin(); iter != m_pools.end(); ++iter) {
        BlsConsumer *rp = *iter;
        rp->onMessage(msg);
    }

    // to dvr
    if (m_recoder) {
        m_recoder->onMessage(&msg);
    }

    return E_SUCCESS;
}

int BlsRtmpSource::fastGop(BlsConsumer *pool)
{
    int ts = 0;
    int index = 0;
    list<BlsRtmpMessage>::iterator iter;
    for (iter = m_gop.begin(); iter != m_gop.end(); ++iter) {
        BlsRtmpMessage msg = *iter;
        if (!msg.isVideo()) continue;

        if (index % 4 != 0) continue;

        msg.header.timestamp = ts;
        ts += 10;

        pool->onMessage(msg);
    }

    return E_SUCCESS;
}
