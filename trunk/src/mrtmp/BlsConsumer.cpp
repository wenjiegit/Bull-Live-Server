#include "BlsConsumer.hpp"
#include "BlsRtmpProtocol.hpp"
#include "BlsTimestampCorrector.hpp"
#include "BlsRtmpSource.hpp"

#include <MLoger>

BlsConsumer::BlsConsumer(const MString &url, MObject *parent)
    : MObject(parent)
    , m_corrector(new BlsTimestampCorrector(this))
    , m_url(url)
{

}

BlsConsumer::~BlsConsumer()
{
    BlsRtmpSource *source = BlsRtmpSource::findSource(m_url);
    if (source) {
        source->removePool(this);
    }
}

int BlsConsumer::onVideo(BlsRtmpMessage &msg)
{
    m_corrector->correct(msg);
    m_msgs.push_back(msg);

    return E_SUCCESS;
}

int BlsConsumer::onAudio(BlsRtmpMessage &msg)
{
    m_corrector->correct(msg);
    m_msgs.push_back(msg);

    return E_SUCCESS;
}

int BlsConsumer::onMetadata(BlsRtmpMessage &msg)
{
    m_corrector->correct(msg);
    m_msgs.push_back(msg);

    return E_SUCCESS;
}

int BlsConsumer::onMessage(BlsRtmpMessage &msg)
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

list<BlsRtmpMessage> BlsConsumer::getMessage()
{
    list<BlsRtmpMessage> ret = m_msgs;
    m_msgs.clear();

    return ret;
}
