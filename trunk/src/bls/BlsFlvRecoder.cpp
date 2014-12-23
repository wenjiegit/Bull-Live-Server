#include "BlsFlvRecoder.hpp"

#include <MFile>
#include <MLoger>
#include <MStream>

char flv_header[] = {'F', 'L', 'V',
                            0x01, 0x05, 0x00, 0x00, 0x00, 0x09,
                            0x00, 0x00, 0x00, 0x00};

MStream serialFlv(BlsRtmpMessage *msg)
{
    MStream stream;

    // tag header
    mint64 dts = msg->header.timestamp;
    mint8 type = (mint8)(msg->header.type);

    // metadata dts : 0
    if (type == RTMP_MSG_AMF0DataMessage) {
        //dts = 0x00;
    }

    stream.write1Bytes(type);
    stream.write3Bytes(msg->payload.size());
    stream.write3Bytes(dts);
    stream.write1Bytes(dts >> 24 & 0xFF);
    stream.write3Bytes(0);
    stream.writeString(msg->payload);

    // pre tag size
    int preTagSize = msg->payload.size() + 11;
    stream.write4Bytes(preTagSize);

    return stream;
}

static int writeFlv(BlsRtmpMessage *msg, MFile *file)
{
    MStream stream = serialFlv(msg);
    if (file->write(stream) != stream.size()) {
        log_error("write to file failed, %s", file->fileName().c_str());
        return -1;
    }

    return E_SUCCESS;
}

BlsFlvRecoder::BlsFlvRecoder(MObject *parent)
    : MObject(parent)
    , m_file(NULL)
{
}

BlsFlvRecoder::~BlsFlvRecoder()
{
    mFree(m_file);
}

void BlsFlvRecoder::setFileName(const MString &name)
{
    m_fileName = name;
}

int BlsFlvRecoder::start()
{
    if (m_file) {
        log_warn("file has been opened.");
        return E_SUCCESS;
    }

    m_file = new MFile(m_fileName);
    if (!m_file->open("w")) {
        log_error("open file failed, %s", m_fileName.c_str());
        mFree(m_file);
        return -1;
    }

    m_file->setAutoFlush(true);

    // write flv header 9 + 4bytes' previous tag size.
    // TODO process write error
    m_file->write(flv_header, 13);

    return E_SUCCESS;
}

int BlsFlvRecoder::onMessage(BlsRtmpMessage *msg)
{
    log_trace("recv message type %d size %d dts %d", msg->header.type, msg->payload.size(), msg->header.timestamp);

    if (msg->isVideo()) {
        return onVideo(msg);
    } else if (msg->isAudio()) {
        return onAudio(msg);
    } else if (msg->isAmf0Data()) {
        return onMetadata(msg);
    } else {
        return onOther(msg);
    }

    return E_SUCCESS;
}

int BlsFlvRecoder::onVideo(BlsRtmpMessage *msg)
{
    return writeFlv(msg, m_file);
}

int BlsFlvRecoder::onAudio(BlsRtmpMessage *msg)
{
    return writeFlv(msg, m_file);
}

int BlsFlvRecoder::onMetadata(BlsRtmpMessage *msg)
{
     return writeFlv(msg, m_file);
}

int BlsFlvRecoder::onOther(BlsRtmpMessage *msg)
{
    log_trace("recv message type %d size %d", msg->header.type, msg->payload.size());

    return E_SUCCESS;
}
