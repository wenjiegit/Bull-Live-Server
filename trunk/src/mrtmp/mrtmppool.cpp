#include "mrtmppool.hpp"
#include "mrtmpprotocol.hpp"
#include "mrtmptimestampcorrector.hpp"
#include "mrtmpsource.hpp"

#include <MLoger>

MRtmpPool::MRtmpPool(const MString &url, MObject *parent)
    : MObject(parent)
    , m_corrector(new MRtmpTimestampCorrector(this))
    , m_url(url)
{

}

MRtmpPool::~MRtmpPool()
{
    MRtmpSource *source = MRtmpSource::findSource(m_url);
    if (source) {
        source->removePool(this);
    }
}

int MRtmpPool::onVideo(MRtmpMessage &msg)
{
    m_corrector->correct(msg);
    m_msgs.push_back(msg);

    return E_SUCCESS;
}

int MRtmpPool::onAudio(MRtmpMessage &msg)
{
    m_corrector->correct(msg);
    m_msgs.push_back(msg);

    return E_SUCCESS;
}

int MRtmpPool::onMetadata(MRtmpMessage &msg)
{
    m_corrector->correct(msg);
    m_msgs.push_back(msg);

    return E_SUCCESS;
}

int MRtmpPool::onMessage(MRtmpMessage &msg)
{
    if (msg.isAudio()) {
        return onAudio(msg);
    } else if (msg.isVideo()) {
        return onVideo(msg);
    } else if (msg.isAmf0Data()) {
        return onMetadata(msg);
    } else {
        m_msgs.push_back(msg);
    }

    return E_SUCCESS;
}

list<MRtmpMessage> MRtmpPool::getMessage()
{
    list<MRtmpMessage> ret = m_msgs;
    m_msgs.clear();

    return ret;
}
