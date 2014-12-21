#include "BlsConsumer.hpp"
#include "mrtmpprotocol.hpp"
#include "mrtmptimestampcorrector.hpp"
#include "BlsRtmpSource.hpp"

#include <MLoger>

BlsConsumer::BlsConsumer(const MString &url, MObject *parent)
    : MObject(parent)
    , m_corrector(new MRtmpTimestampCorrector(this))
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

int BlsConsumer::onVideo(MRtmpMessage &msg)
{
    m_corrector->correct(msg);
    m_msgs.push_back(msg);

    return E_SUCCESS;
}

int BlsConsumer::onAudio(MRtmpMessage &msg)
{
    m_corrector->correct(msg);
    m_msgs.push_back(msg);

    return E_SUCCESS;
}

int BlsConsumer::onMetadata(MRtmpMessage &msg)
{
    m_corrector->correct(msg);
    m_msgs.push_back(msg);

    return E_SUCCESS;
}

int BlsConsumer::onMessage(MRtmpMessage &msg)
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

list<MRtmpMessage> BlsConsumer::getMessage()
{
    list<MRtmpMessage> ret = m_msgs;
    m_msgs.clear();

    return ret;
}
