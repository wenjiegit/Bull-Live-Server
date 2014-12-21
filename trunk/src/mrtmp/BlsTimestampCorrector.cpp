#include "BlsTimestampCorrector.hpp"
#include "BlsRtmpProtocol.hpp"

#define CONST_MAX_JITTER_MS             500
#define DEFAULT_FRAME_TIME_MS           40

BlsTimestampCorrector::BlsTimestampCorrector(MObject *parent)
    : MObject(parent)
    , m_lastPktCorrectTime(0)
    , m_lastPktTime(0)
{

}

int BlsTimestampCorrector::correct(BlsRtmpMessage &msg, int tba, int tbv)
{
    int ret = E_SUCCESS;

    if (!msg.isAudio() && !msg.isVideo()) {
        msg.header.timestamp = 0;
        return ret;
    }

    int sample_rate = tba;
    int frame_rate = tbv;

    int64_t time = msg.header.timestamp;
    int64_t delta = time - m_lastPktTime;

    if (delta < 0 || delta > CONST_MAX_JITTER_MS) {
        if (msg.isAudio() && sample_rate > 0) {
            delta = (int64_t)(delta * 1000.0 / sample_rate);
        } else if (msg.isVideo() && frame_rate > 0) {
            delta = (int64_t)(delta * 1.0 / frame_rate);
        } else {
            delta = DEFAULT_FRAME_TIME_MS;
        }

        if (delta < 0 || delta > CONST_MAX_JITTER_MS) {
            delta = DEFAULT_FRAME_TIME_MS;
        }
    }
    m_lastPktCorrectTime = mMax(0, m_lastPktCorrectTime + delta);
    msg.header.timestamp = m_lastPktCorrectTime;
    m_lastPktTime = time;

    return ret;
}

mint64 BlsTimestampCorrector::getCurrentTime()
{
    return m_lastPktCorrectTime;
}

void BlsTimestampCorrector::reset()
{
    m_lastPktCorrectTime = m_lastPktTime = 0;
}
