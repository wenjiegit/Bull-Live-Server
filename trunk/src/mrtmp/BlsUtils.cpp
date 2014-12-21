#include "BlsUtils.hpp"

#include <MStringList>
#include <MStream>
#include <MLoger>

BlsInternalMsg::BlsInternalMsg()
    : m_headerSize(0)
    , m_bodySize(0)
{

}

MString BlsInternalMsg::header() const
{
    return m_header;
}

void BlsInternalMsg::setHeader(const MString &header)
{
    m_header = header;
}

MString BlsInternalMsg::body() const
{
    return m_body;
}

void BlsInternalMsg::setBody(const MString &body)
{
    m_body = body;
}

MStream BlsInternalMsg::encode() const
{
    MStream stream;

    stream.write4Bytes(m_header.size());
    stream.writeString(m_header);
    stream.write4Bytes(m_body.size());
    stream.writeString(m_body);

    return stream;
}

void BlsInternalMsg::dump() const
{
    log_trace("msg header: %s  body: %s", m_header.c_str(), m_body.c_str());
}

int readInternalMsg(BlsInternalMsg &msg, MTcpSocket *socket)
{
    int ret = E_SUCCESS;

    int headerSize = -1;

    if ((ret = socket->readInt(headerSize)) != E_SUCCESS) {
        return ret;
    }

    if (headerSize <= 0) {
        return -1;
    }

    char header[headerSize];
    if (socket->readFully(header, headerSize) != headerSize) {
        return -2;
    }

    int bodySize = -1;

    if ((ret = socket->readInt(bodySize)) != E_SUCCESS) {
        return ret;
    }

    if (bodySize <= 0) {
        return -3;
    }

    char body[bodySize];
    if (socket->readFully(body, bodySize) != bodySize) {
        return -4;
    }

    msg.setHeader(MString(header, headerSize));
    msg.setBody(MString(body, bodySize));

    return E_SUCCESS;
}

int writeInternalMsg(const BlsInternalMsg &msg, MTcpSocket *socket)
{
    MStream stream = msg.encode();

    if (socket->write(stream) != stream.size()) {
        return -1;
    }

    return E_SUCCESS;
}
