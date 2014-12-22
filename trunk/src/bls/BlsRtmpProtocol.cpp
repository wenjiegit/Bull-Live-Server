#include "BlsRtmpProtocol.hpp"
#include "BlsRtmpHandshake.hpp"
#include "BlsRtmpUrl.hpp"

#include <MLoger>
#include <MTcpSocket>
#include <iostream>

using namespace std;


BlsRtmpMessage::BlsRtmpMessage()
{

}

BlsRtmpMessage::~BlsRtmpMessage()
{

}

BlsRtmpProtocol::BlsRtmpProtocol(MTcpSocket *socket, MObject *parent)
    : MObject(parent)
    , m_socket(socket)
{
}

int BlsRtmpProtocol::recv_message(BlsRtmpMessage **pmsg)
{
    *pmsg = NULL;

    int ret = E_SUCCESS;

    while (true) {
        BlsRtmpMessage* msg = NULL;

        if ((ret = recv_interlaced_message(&msg)) != E_SUCCESS) {
            mFree(msg);
            return ret;
        }

        if (!msg) {
            continue;
        }

        if (msg->payload.size() == 0 || msg->header.payloadLength <= 0) {
            log_trace("ignore empty message(type=%d, size=%d, time=%lu, sid=%d).",
                msg->header.type, msg->header.payloadLength,
                msg->header.timestamp, msg->header.streamID);
            mFree(msg);
            continue;
        }

        if ((ret = on_recv_message(msg)) != E_SUCCESS) {
            log_error("hook the received msg failed. ret=%d", ret);
            return ret;
        }

#if 0
        log_trace("got a msg, cid=%d, type=%d, size=%d, time=%lu",
            msg->header.perfer_cid, msg->header.type, msg->header.payloadLength,
            msg->header.timestamp);
#endif
        *pmsg = msg;
        break;
    }

    return ret;
}

int BlsRtmpProtocol::send_message(BlsRtmpMessage *msg)
{
    int ret = E_SUCCESS;

    if (msg->header.perfer_cid < 2) {
        log_warn("change the chunk_id=%d to default=%d",
            msg->header.perfer_cid, RTMP_CID_ProtocolControl);
        msg->header.perfer_cid = RTMP_CID_ProtocolControl;
    }

    char out_header_cache[16];
    char* p = (char*)msg->payload.data();
    char* pp;

    do {
        // generate the header.
        char* pheader = out_header_cache;

        if (p == (char*)msg->payload.data()) {
            // write new chunk stream header, fmt is 0
            *pheader++ = 0x00 | (msg->header.perfer_cid & 0x3F);

            // chunk message header, 11 bytes
            // timestamp, 3bytes, big-endian
            mint32 timestamp = (u_int32_t)msg->header.timestamp;
            if (timestamp >= RTMP_EXTENDED_TIMESTAMP) {
                *pheader++ = 0xFF;
                *pheader++ = 0xFF;
                *pheader++ = 0xFF;
            } else {
                pp = (char*)&timestamp;
                *pheader++ = pp[2];
                *pheader++ = pp[1];
                *pheader++ = pp[0];
            }

            // message_length, 3bytes, big-endian
            int payload_length = msg->payload.length();
            pp = (char*)&payload_length;
            *pheader++ = pp[2];
            *pheader++ = pp[1];
            *pheader++ = pp[0];

            // message_type, 1bytes
            *pheader++ = msg->header.type;

            // message_length, 3bytes, little-endian
            pp = (char*)&msg->header.streamID;
            *pheader++ = pp[0];
            *pheader++ = pp[1];
            *pheader++ = pp[2];
            *pheader++ = pp[3];

            // chunk extended timestamp header, 0 or 4 bytes, big-endian
            if(timestamp >= RTMP_EXTENDED_TIMESTAMP){
                pp = (char*)&timestamp;
                *pheader++ = pp[3];
                *pheader++ = pp[2];
                *pheader++ = pp[1];
                *pheader++ = pp[0];
            }
        } else {
            *pheader++ = 0xC0 | (msg->header.perfer_cid & 0x3F);
            u_int32_t timestamp = (u_int32_t)msg->header.timestamp;
            if(timestamp >= RTMP_EXTENDED_TIMESTAMP){
                pp = (char*)&timestamp;
                *pheader++ = pp[3];
                *pheader++ = pp[2];
                *pheader++ = pp[1];
                *pheader++ = pp[0];
            }
        }

        int payload_size = msg->payload.length() - (p - (char*)msg->payload.data());
        payload_size = mMin(payload_size, m_rtmpCtx.outChunkSize);

        // always has header
        int header_size = pheader - out_header_cache;
        mAssert(header_size > 0);

        // send by writev
        iovec iov[2];
        iov[0].iov_base = out_header_cache;
        iov[0].iov_len = header_size;
        iov[1].iov_base = p;
        iov[1].iov_len = payload_size;

        if (m_socket->writev(iov, 2) < 0) {
            log_error("send with writev failed.");
            ret = merrno;
            return ret;
        }

        // consume sendout bytes when not empty packet.
        if (msg->payload.data() && msg->payload.length() > 0) {
            p += payload_size;
        }
    } while (p < (char*)msg->payload.data() + msg->payload.length());

    return ret;
}

void BlsRtmpProtocol::setSession(BlsRtmpProtocolAbstract *session)
{
    m_session = session;
}

int BlsRtmpProtocol::onConnect(double id)
{
    int ret = E_SUCCESS;

    BlsAMF0ShortString *commandName = new BlsAMF0ShortString(RTMP_AMF0_COMMAND_RESULT);
    BlsAMF0Number *transactionID = new BlsAMF0Number(id);

    BlsAMF0Object *object = new BlsAMF0Object;
    BlsAMF0Object *object_status = new BlsAMF0Object;
    BlsAMF0EcmaArray *ecmaArray = new BlsAMF0EcmaArray;

    // object
    object->setValue("fmsVer", new BlsAMF0ShortString("FMS/4,5,0,297"));
    object->setValue("capabilities", new BlsAMF0Number(127));
    object->setValue("mode", new BlsAMF0Number(1));

    // object_status
    object_status->setValue(STATUS_LEVEL, new BlsAMF0ShortString("status"));
    object_status->setValue(STATUS_CODE, new BlsAMF0ShortString(NetConnection_Connect_Success));
    object_status->setValue(STATUS_DESC, new BlsAMF0ShortString(NetConnection_Connect_Success));
    // TODO fix object encoding 0 or 3
    object_status->setValue("objectEncoding", new BlsAMF0Number(3));

    ecmaArray->setValue("version", new BlsAMF0ShortString("4,5,0,297"));
    object_status->setValue("data", ecmaArray);


    BlsRtmpMessageHeader header;
    header.perfer_cid = RTMP_CID_OverConnection;
    header.type = RTMP_MSG_AMF0CommandMessage;

    if ((ret = sendAny(header, commandName, transactionID, object, object_status)) != E_SUCCESS) {
        return ret;
    }

    return ret;
}

int BlsRtmpProtocol::setChunkSize(int chunkSize)
{
    int ret = E_SUCCESS;

    BlsRtmpMessage *msg = new BlsRtmpMessage;
    mAutoFree(BlsRtmpMessage, msg);

    msg->payload.write4Bytes(chunkSize);

    BlsRtmpMessageHeader &header = msg->header;
    header.perfer_cid = RTMP_CID_ProtocolControl;
    header.type = RTMP_MSG_SetChunkSize;
    header.payloadLength = msg->payload.size();
    header.streamID = m_rtmpCtx.streamID;

    if ((ret = send_message(msg)) != E_SUCCESS) {
        return ret;
    }
    m_rtmpCtx.outChunkSize = chunkSize;

    return ret;
}

int BlsRtmpProtocol::setPeerBandwidth(mint32 bandwidth, mint8 type)
{
    int ret = E_SUCCESS;

    BlsRtmpMessage *msg = new BlsRtmpMessage;
    mAutoFree(BlsRtmpMessage, msg);

    msg->payload.write4Bytes(bandwidth);
    msg->payload.write1Bytes(type);

    BlsRtmpMessageHeader &header = msg->header;
    header.perfer_cid = RTMP_CID_ProtocolControl;
    header.type = RTMP_MSG_SetPeerBandwidth;
    header.payloadLength = msg->payload.size();
    header.streamID = m_rtmpCtx.streamID;

    if ((ret = send_message(msg)) != E_SUCCESS) {
        return ret;
    }

    return ret;
}

int BlsRtmpProtocol::onBWDone()
{
    int ret = E_SUCCESS;

    MString cmdName = RTMP_AMF0_COMMAND_ON_BW_DONE;
    BlsRtmpMessageHeader header;
    header.perfer_cid = RTMP_CID_OverConnection;
    header.type = RTMP_MSG_AMF0CommandMessage;

    if ((ret = sendAny(header, new BlsAMF0ShortString(cmdName), new BlsAMF0Number(0), new BlsAMF0Null)) != E_SUCCESS) {
        return ret;
    }

    return ret;
}

int BlsRtmpProtocol::setAckSize(mint32 size)
{
    int ret = E_SUCCESS;

    BlsRtmpMessage *msg = new BlsRtmpMessage;
    mAutoFree(BlsRtmpMessage, msg);

    msg->payload.write4Bytes(size);

    BlsRtmpMessageHeader &header = msg->header;
    header.perfer_cid = RTMP_CID_ProtocolControl;
    header.type = RTMP_MSG_WindowAcknowledgementSize;
    header.payloadLength = msg->payload.size();
    header.streamID = m_rtmpCtx.streamID;

    if ((ret = send_message(msg)) != E_SUCCESS) {
        return ret;
    }

    return ret;

}

int BlsRtmpProtocol::setUCM(int type, int eventData1, int eventData2)
{
    int ret = E_SUCCESS;

    BlsRtmpMessage *msg = new BlsRtmpMessage;
    mAutoFree(BlsRtmpMessage, msg);

    msg->payload.write2Bytes(type);

    if (eventData1 >= 0) {
        msg->payload.write4Bytes(eventData1);
    }

    if (eventData2 >= 0) {
        msg->payload.write4Bytes(eventData2);
    }

    BlsRtmpMessageHeader &header = msg->header;
    header.perfer_cid = RTMP_CID_ProtocolControl;
    header.type = RTMP_MSG_UserControlMessage;
    header.payloadLength = msg->payload.size();
    header.streamID = m_rtmpCtx.streamID;

    if ((ret = send_message(msg)) != E_SUCCESS) {
        return ret;
    }

    return ret;
}

int BlsRtmpProtocol::createStream()
{
    int ret = E_SUCCESS;

    BlsRtmpMessage *msg = new BlsRtmpMessage;
    mAutoFree(BlsRtmpMessage, msg);

    MStream &stream = msg->payload;
    BlsAMF0Serializer::writeShortString(stream, "createStream");
    BlsAMF0Serializer::writeDouble(stream, 2);
    BlsAMF0Serializer::writeNull(stream);

    BlsRtmpMessageHeader &header = msg->header;
    header.perfer_cid = RTMP_CID_ProtocolControl;
    header.type = RTMP_MSG_AMF0CommandMessage;
    header.payloadLength = msg->payload.size();
    header.streamID = m_rtmpCtx.streamID;

    if ((ret = send_message(msg)) != E_SUCCESS) {
        return ret;
    }

    return ret;
}

int BlsRtmpProtocol::publishStream(double transactionId, const MString &streamName)
{
    int ret = E_SUCCESS;

    BlsRtmpMessage *msg = new BlsRtmpMessage;
    mAutoFree(BlsRtmpMessage, msg);

    MStream &stream = msg->payload;
    BlsAMF0Serializer::writeShortString(stream, "publish");
    BlsAMF0Serializer::writeDouble(stream, transactionId);
    BlsAMF0Serializer::writeNull(stream);
    BlsAMF0Serializer::writeShortString(stream, streamName);

    BlsRtmpMessageHeader &header = msg->header;
    header.perfer_cid = RTMP_CID_OverConnection2;
    header.type = RTMP_MSG_AMF0CommandMessage;
    header.payloadLength = msg->payload.size();
    header.streamID = m_rtmpCtx.streamID;

    if ((ret = send_message(msg)) != E_SUCCESS) {
        return ret;
    }

    return ret;
}

int BlsRtmpProtocol::sendAny(const BlsRtmpMessageHeader &header, BlsAMF0Any *arg1, BlsAMF0Any *arg2, BlsAMF0Any *arg3
                           , BlsAMF0Any *arg4, BlsAMF0Any *arg5, BlsAMF0Any *arg6)
{
    int ret = E_SUCCESS;

    mAutoFree(BlsAMF0Any, arg1);
    mAutoFree(BlsAMF0Any, arg2);
    mAutoFree(BlsAMF0Any, arg3);
    mAutoFree(BlsAMF0Any, arg4);
    mAutoFree(BlsAMF0Any, arg5);
    mAutoFree(BlsAMF0Any, arg6);

    BlsRtmpMessage *msg = new BlsRtmpMessage;
    mAutoFree(BlsRtmpMessage, msg);

    msg->header = header;

    if ((ret = encodeAny(msg->payload, arg1, arg2, arg3, arg4, arg5, arg6)) != E_SUCCESS) {
        return ret;
    }

    msg->header.payloadLength = msg->payload.size();
    msg->header.streamID = m_rtmpCtx.streamID;

    if ((ret = send_message(msg)) != E_SUCCESS) {
        return ret;
    }

    return ret;
}

int BlsRtmpProtocol::sendNetStatusEvent(double transactionID, BlsRtmpNetStatusEvent *event)
{
    int ret = E_SUCCESS;

    BlsRtmpMessageHeader header;
    header.perfer_cid = RTMP_CID_OverConnection;
    header.type = RTMP_MSG_AMF0CommandMessage;

    MString cmdName = RTMP_AMF0_COMMAND_ON_STATUS;

    if ((ret = sendAny(header, new BlsAMF0ShortString(cmdName), new BlsAMF0Number(transactionID), new BlsAMF0Null, event)) != E_SUCCESS) {
        return ret;
    }

    return ret;
}

BlsRtmpContext *BlsRtmpProtocol::getRtmpCtx()
{
    return &m_rtmpCtx;
}

int BlsRtmpProtocol::handshakeWithClient()
{
    int ret = E_SUCCESS;
    BlsRtmpHandshake hk;
    if ((ret = hk.handshake_with_client(*m_socket)) != 0) {
        log_error("handshake with client failed. ret=%d", ret);
        return ret;
    }
    log_trace("handshake with client success.");

    return ret;
}

int BlsRtmpProtocol::handshakeWithServer(bool useComplex)
{
    int ret = E_SUCCESS;

    BlsRtmpHandshake hk;
    if (useComplex) {
        if ((ret = hk.handshake_with_server_use_complex(*m_socket, false, false)) != E_SUCCESS) {
            log_error("complex handshake with server failed. ret=%d", ret);
            return ret;
        }

        log_trace("complex handshake with server success.");
    } else {
        if ((ret = hk.handshake_with_server_use_simple(*m_socket)) != E_SUCCESS) {
            log_error("simple handshake with server failed. ret=%d", ret);
            return ret;
        }

        log_trace("simple handshake with server success.");
    }

    return ret;
}

int BlsRtmpProtocol::recv_interlaced_message(BlsRtmpMessage** pmsg)
{
    int ret = E_SUCCESS;

    // chunk stream basic header.
    char fmt = 0;
    int  cid = 0;
    int  bh_size = 0;

    if ((ret = read_basic_header(fmt, cid, bh_size)) != E_SUCCESS) {
        return ret;
    }
    log_verbose("read basic header success. fmt=%d, cid=%d, bh_size=%d", fmt, cid, bh_size);

    // get the cached chunk stream.
    BlsRtmpChunkStream* chunk = NULL;
    if (!m_chunks.contains(cid)) {
        chunk = m_chunks[cid] = new BlsRtmpChunkStream(cid);
        // set the perfer cid of chunk,
        // which will copy to the message received.
        chunk->header.perfer_cid = cid;
    } else {
        chunk = m_chunks[cid];
    }

    // chunk stream message header
    int mh_size = 0;
    if ((ret = read_message_header(chunk, fmt, bh_size, mh_size)) != E_SUCCESS) {
        return ret;
    }

    // read msg payload from chunk stream.
    BlsRtmpMessage* msg = NULL;
    int payload_size = 0;
    if ((ret = read_message_payload(chunk, bh_size, mh_size, payload_size, &msg)) != E_SUCCESS) {
        return ret;
    }

    // not got an entire RTMP message, try next chunk.
    if (!msg) {
        return ret;
    }
    *pmsg = msg;

    return ret;
}

int BlsRtmpProtocol::read_basic_header(char& fmt, int& cid, int& bh_size)
{
    int ret = E_SUCCESS;
    m_chunkHeader.clear();

    if (fillStream(1, m_chunkHeader) != 0) {
        log_error("read 1bytes basic header failed.");
        return -1;
    }

    char* p = STR_DATA(m_chunkHeader);

    fmt = (*p >> 6) & 0x03;
    cid = *p & 0x3f;
    bh_size = 1;

    if (cid > 1) {
        log_verbose("%dbytes basic header parsed. fmt=%d, cid=%d", bh_size, fmt, cid);
        return ret;
    }

    if (cid == 0) {
        if (fillStream(1, m_chunkHeader) != 0) {
            log_error("read 1bytes basic header failed.");
            return -1;
        }

        cid = 64;
        cid += *(++p);
        bh_size = 2;
        log_verbose("%dbytes basic header parsed. fmt=%d, cid=%d", bh_size, fmt, cid);
    } else if (cid == 1) {
        if (fillStream(2, m_chunkHeader) != 0) {
            log_error("read 2bytes basic header failed.");
            return -1;
        }

        cid = 64;
        cid += *(++p);
        cid += *(++p) * 256;
        bh_size = 3;
        log_verbose("%dbytes basic header parsed. fmt=%d, cid=%d", bh_size, fmt, cid);
    } else {
        log_error("invalid path, impossible basic header.");
        mAssert(false);
    }

    return 0;
}

int BlsRtmpProtocol::read_message_header(BlsRtmpChunkStream* chunk, char fmt, int bh_size, int& mh_size)
{

    int ret = E_SUCCESS;

    /**
    * we should not assert anything about fmt, for the first packet.
    * (when first packet, the chunk->msg is NULL).
    * the fmt maybe 0/1/2/3, the FMLE will send a 0xC4 for some audio packet.
    * the previous packet is:
    *     04             // fmt=0, cid=4
    *     00 00 1a     // timestamp=26
    *    00 00 9d     // payload_length=157
    *     08             // message_type=8(audio)
    *     01 00 00 00 // stream_id=1
    * the current packet maybe:
    *     c4             // fmt=3, cid=4
    * it's ok, for the packet is audio, and timestamp delta is 26.
    * the current packet must be parsed as:
    *     fmt=0, cid=4
    *     timestamp=26+26=52
    *     payload_length=157
    *     message_type=8(audio)
    *     stream_id=1
    * so we must update the timestamp even fmt=3 for first packet.
    */
    // fresh packet used to update the timestamp even fmt=3 for first packet.
    bool is_fresh_packet = !chunk->msg;

    // but, we can ensure that when a chunk stream is fresh,
    // the fmt must be 0, a new stream.
    if (chunk->msg_count == 0 && fmt != RTMP_FMT_TYPE0) {
        // for librtmp, if ping, it will send a fresh stream with fmt=1,
        // 0x42             where: fmt=1, cid=2, protocol contorl user-control message
        // 0x00 0x00 0x00   where: timestamp=0
        // 0x00 0x00 0x06   where: payload_length=6
        // 0x04             where: message_type=4(protocol control user-control message)
        // 0x00 0x06            where: event Ping(0x06)
        // 0x00 0x00 0x0d 0x0f  where: event data 4bytes ping timestamp.
        // @see: https://github.com/winlinvip/simple-rtmp-server/issues/98
        if (chunk->cid == RTMP_CID_ProtocolControl && fmt == RTMP_FMT_TYPE1) {
            log_warn("accept cid=2, fmt=1 to make librtmp happy.");
        } else {
            // must be a RTMP protocol level error.
            ret = -1;
            log_error("chunk stream is fresh, fmt must be %d, actual is %d. cid=%d, ret=%d",
                RTMP_FMT_TYPE0, fmt, chunk->cid, ret);
            return ret;
        }
    }

    // when exists cache msg, means got an partial message,
    // the fmt must not be type0 which means new message.
    if (chunk->msg && fmt == RTMP_FMT_TYPE0) {
        ret = -1;
        log_error("chunk stream exists, "
            "fmt must not be %d, actual is %d. ret=%d", RTMP_FMT_TYPE0, fmt, ret);
        return ret;
    }

    // create msg when new chunk stream start
    bool is_first_chunk_of_msg = false;
    if (!chunk->msg) {
        is_first_chunk_of_msg = true;
        chunk->msg = new BlsRtmpMessage;
        log_verbose("create message for new chunk, fmt=%d, cid=%d", fmt, chunk->cid);
    }

    // read message header from socket to buffer.
    static char mh_sizes[] = {11, 7, 3, 0};
    mh_size = mh_sizes[(int)fmt];
    log_verbose("calc chunk message header size. fmt=%d, mh_size=%d", fmt, mh_size);

    if (fillStream(mh_size, m_chunkHeader) != 0) {
        log_error("read %dbytes message header failed. ret=%d", mh_size, ret);
        return -1;
    }

    char* p = STR_DATA(m_chunkHeader) + bh_size;

    if (fmt <= RTMP_FMT_TYPE2) {
        char* pp = (char*)&chunk->header.timestampDelta;
        pp[2] = *p++;
        pp[1] = *p++;
        pp[0] = *p++;
        pp[3] = 0;

        // fmt: 0
        // timestamp: 3 bytes
        // If the timestamp is greater than or equal to 16777215
        // (hexadecimal 0x00ffffff), this value MUST be 16777215, and the
        // extended timestamp header MUST be present. Otherwise, this value
        // SHOULD be the entire timestamp.
        //
        // fmt: 1 or 2
        // timestamp delta: 3 bytes
        // If the delta is greater than or equal to 16777215 (hexadecimal
        // 0x00ffffff), this value MUST be 16777215, and the ‘extended
        // timestamp header MUST be present. Otherwise, this value SHOULD be
        // the entire delta.
        chunk->hasExtendedTimestamp = (chunk->header.timestampDelta >= RTMP_EXTENDED_TIMESTAMP);
        if (!chunk->hasExtendedTimestamp) {
            // Extended timestamp: 0 or 4 bytes
            // This field MUST be sent when the normal timsestamp is set to
            // 0xffffff, it MUST NOT be sent if the normal timestamp is set to
            // anything else. So for values less than 0xffffff the normal
            // timestamp field SHOULD be used in which case the extended timestamp
            // MUST NOT be present. For values greater than or equal to 0xffffff
            // the normal timestamp field MUST NOT be used and MUST be set to
            // 0xffffff and the extended timestamp MUST be sent.
            if (fmt == RTMP_FMT_TYPE0) {
                // 6.1.2.1. Type 0
                // For a type-0 chunk, the absolute timestamp of the message is sent
                // here.
                chunk->header.timestamp = chunk->header.timestampDelta;
            } else {
                // 6.1.2.2. Type 1
                // 6.1.2.3. Type 2
                // For a type-1 or type-2 chunk, the difference between the previous
                // chunk's timestamp and the current chunk's timestamp is sent here.
                chunk->header.timestamp += chunk->header.timestampDelta;
            }
        }

        if (fmt <= RTMP_FMT_TYPE1) {
            pp = (char*)&chunk->header.payloadLength;
            pp[2] = *p++;
            pp[1] = *p++;
            pp[0] = *p++;
            pp[3] = 0;

            // if msg exists in cache, the size must not changed.
            if ((chunk->msg->payload.size() > 0)
                    && ((mint32)chunk->msg->payload.size() != chunk->header.payloadLength)) {
                ret = -2;
                log_error("msg exists in chunk cache, "
                    "size=%d cannot change to %d, ret=%d",
                    chunk->msg->payload.size(), chunk->header.payloadLength, ret);
                return ret;
            }

            chunk->header.type = *p++;

            if (fmt == RTMP_FMT_TYPE0) {
                pp = (char*)&chunk->header.streamID;
                pp[0] = *p++;
                pp[1] = *p++;
                pp[2] = *p++;
                pp[3] = *p++;
            }
        }
    } else {
        // update the timestamp even fmt=3 for first stream
        if (is_fresh_packet && !chunk->hasExtendedTimestamp) {
            chunk->header.timestamp += chunk->header.timestampDelta;
        }
        log_verbose("header read completed. fmt=%d, size=%d, ext_time=%d",
            fmt, mh_size, chunk->hasExtendedTimestamp);
    }

    // read extended-timestamp
    if (chunk->hasExtendedTimestamp) {
        if (fillStream(4, m_chunkHeader) != 0) {
            log_error("read 4bytes message header failed. ret=%d", ret);
            return -1;
        }

        mint32 timestamp = 0x00;
        char* pp = (char*)&timestamp;
        pp[3] = *p++;
        pp[2] = *p++;
        pp[1] = *p++;
        pp[0] = *p++;

        mint32 chunk_timestamp = chunk->header.timestamp;

        if (!is_first_chunk_of_msg && chunk_timestamp > 0 && chunk_timestamp != timestamp) {
            mh_size -= 4;
            log_warn("no 4bytes extended timestamp in the continued chunk");
        } else {
            chunk->header.timestamp = timestamp;
        }
    }

    if (chunk->header.timestamp > 0x7fffffff) {
        log_warn("RTMP 31bits timestamp overflow, time=%lu", chunk->header.timestamp);
    }
    chunk->header.timestamp &= 0x7fffffff;

    // valid message
    if (chunk->header.payloadLength < 0) {
        ret = -3;
        log_error("RTMP message size must not be negative. size=%d, ret=%d",
            chunk->header.payloadLength, ret);
        return ret;
    }

    // copy header to msg
    chunk->msg->header = chunk->header;

    // increase the msg count, the chunk stream can accept fmt=1/2/3 message now.
    chunk->msg_count++;

    return ret;
}

int BlsRtmpProtocol::read_message_payload(BlsRtmpChunkStream* chunk, int bh_size, int mh_size, int &payload_size, BlsRtmpMessage** pmsg)
{
    int ret = E_SUCCESS;

    // empty message
    if (chunk->header.payloadLength <= 0) {
        log_trace("get an empty RTMP "
                "message(type=%d, size=%d, time=%lu, sid=%d)", chunk->header.type,
                chunk->header.payloadLength, chunk->header.timestamp, chunk->header.streamID);

        *pmsg = chunk->msg;
        chunk->msg = NULL;

        return ret;
    }

    // the chunk payload size.
    payload_size = chunk->header.payloadLength - chunk->msg->payload.size();
    payload_size = mMin(payload_size, m_rtmpCtx.inChunkSize);

    // read payload to buffer
    if (fillStream(payload_size, chunk->msg->payload) != 0) {
        log_error("read payload failed. required_size=%d", payload_size);
        return -4;
    }

    // got entire RTMP message?
    if (chunk->header.payloadLength == (mint32)chunk->msg->payload.size()) {
        *pmsg = chunk->msg;
        chunk->msg = NULL;
        log_verbose("get entire RTMP message(type=%d, size=%d, time=%lu, sid=%d)",
                chunk->header.type, chunk->header.payloadLength,
                chunk->header.timestamp, chunk->header.streamID);
        return ret;
    }

    log_verbose("get partial RTMP message(type=%d, size=%d, time=%lu, sid=%d), partial size=%d",
            chunk->header.type, chunk->header.payloadLength,
            chunk->header.timestamp, chunk->header.streamID,
            chunk->msg->payload.size());

    return ret;
}

int BlsRtmpProtocol::on_recv_message(BlsRtmpMessage* msg)
{
    int ret = E_SUCCESS;

    if ((ret = responeAck()) != E_SUCCESS) {
        return ret;
    }

    mAutoFree(BlsRtmpMessage, msg);
    MStream &stream = msg->payload;

    if (msg->isAudio()) {
        if ((ret = m_session->onAudio(msg)) != E_SUCCESS) {
            return ret;
        }
    } else if (msg->isVideo()) {
        if ((ret = m_session->onVideo(msg)) != E_SUCCESS) {
            return ret;
        }
    } else if (msg->isAmf0Data()) {
        if ((ret = m_session->onMetadata(msg)) != E_SUCCESS) {
            return ret;
        }
    } else if (msg->isAmf0Command() || msg->isAmf3Command()) {
        BlsAMF0Any *arg1 = NULL;
        BlsAMF0Any *arg2 = NULL;
        BlsAMF0Any *arg3 = NULL;
        BlsAMF0Any *arg4 = NULL;
        BlsAMF0Any *arg5 = NULL;
        BlsAMF0Any *arg6 = NULL;

        if (msg->isAmf3Command()) {
            if ((ret = stream.skip(1)) != E_SUCCESS) {
                log_error("amf3 command skip error");
                return ret;
            }
        }

        // TODO may be mem leak ?

        if ((ret = decodeAny(stream, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6)) != E_SUCCESS) {
            log_error("protocol decode_any failed. ret = %d", ret);
            mFree(arg1);
            mFree(arg2);
            mFree(arg3);
            mFree(arg4);
            mFree(arg5);
            mFree(arg6);

            return ret;
        }

        mAutoFree(BlsAMF0Any, arg1);
        mAutoFree(BlsAMF0Any, arg2);
        mAutoFree(BlsAMF0Any, arg3);
        mAutoFree(BlsAMF0Any, arg4);
        mAutoFree(BlsAMF0Any, arg5);
        mAutoFree(BlsAMF0Any, arg6);

        if (!arg1->isShortString() || !arg2->isNumber()) {
            ret = E_AMF_TYPE_ERROR;
            log_error("protocol command msg is not correct. ret = %d", ret);
            return ret;
        }

        MString name = dynamic_cast<BlsAMF0ShortString *>(arg1)->var;
        double id = dynamic_cast<BlsAMF0Number *>(arg2)->var;

        if ((ret = m_session->onCommand(msg, name, id, arg3, arg4, arg5, arg6)) != E_SUCCESS) {
            log_error("command(%s) invoke error. ret=%d", name.c_str(), ret);
            return ret;
        }

    } else if (msg->isWindowAckledgementSize()) {
        if ((ret = stream.read4Bytes(m_rtmpCtx.windowAcknowledgementSize)) != E_SUCCESS) {
            log_error("WindowAckledgementSize read 4 bytes failed. ret=%d", ret);
            return ret;
        }
        log_info("rtmp window ack size=%d", m_rtmpCtx.windowAcknowledgementSize);

    } else if (msg->isSetChunkSize()) {
        if ((ret = stream.read4Bytes(m_rtmpCtx.inChunkSize)) != E_SUCCESS) {
            log_error("chunk size read 4 bytes failed. ret=%d", ret);
            return ret;
        }
        log_trace("rtmp in chunk size=%d", m_rtmpCtx.inChunkSize);
    } else if (msg->isUserControl()) {
        mint16 ucm_type ;
        if ((ret = stream.read2Bytes(ucm_type)) != E_SUCCESS) {
            return ret;
        }

        switch (ucm_type) {
        case UCM_StreamBegin:
            break;
        case UCM_StreamEOF:
            break;
        case UCM_StreamDry:
            break;
        case UCM_SetBufferLength:
        {
            int streamID;
            int bufferLength;

            if ((ret = stream.read4Bytes(streamID)) != E_SUCCESS) {
                return ret;
            }

            if ((ret = stream.read4Bytes(bufferLength)) != E_SUCCESS) {
                return ret;
            }
            log_info("stream id = %d, set buffer length to %d ms", streamID, bufferLength);
            m_rtmpCtx.bufferLength = bufferLength;

            break;
        }
        case UCM_StreamIsRecorded:
            break;
        case UCM_PingRequest:
            break;
        case UCM_PingResponse:
            break;
        default:
            break;
        }
    }

    return ret;
}

int BlsRtmpProtocol::decodeAny(MStream &stream, BlsAMF0Any **arg1, BlsAMF0Any **arg2, BlsAMF0Any **arg3, BlsAMF0Any **arg4, BlsAMF0Any **arg5, BlsAMF0Any **arg6)
{
    int ret = E_SUCCESS;

    if (!stream.end()) {
        if ((ret = BlsAMF0Serializer::read(stream, arg1)) != E_SUCCESS) {
            log_error("AMF0CommandMessage decode_any arg1 failed, ret=%d", ret);
            return ret;
        }
    }

    if (!stream.end()) {
        if ((ret = BlsAMF0Serializer::read(stream, arg2)) != E_SUCCESS) {
            log_error("AMF0CommandMessage decode_any arg2 failed, ret=%d", ret);
            return ret;
        }
    }

    if (!stream.end()) {
        if ((ret = BlsAMF0Serializer::read(stream, arg3)) != E_SUCCESS) {
            log_error("AMF0CommandMessage decode_any arg3 failed, ret=%d", ret);
            return ret;
        }
    }

    if (!stream.end()) {
        if ((ret = BlsAMF0Serializer::read(stream, arg4)) != E_SUCCESS) {
            log_error("AMF0CommandMessage decode_any arg4 failed, ret=%d", ret);
            return ret;
        }
    }

    if (!stream.end()) {
        if ((ret = BlsAMF0Serializer::read(stream, arg5)) != E_SUCCESS) {
            log_error("AMF0CommandMessage decode_any arg5 failed, ret=%d", ret);
            return ret;
        }
    }

    if (!stream.end()) {
        if ((ret = BlsAMF0Serializer::read(stream, arg6)) != E_SUCCESS) {
            log_error("AMF0CommandMessage decode_any arg6 failed, ret=%d", ret);
            return ret;
        }
    }

    if (!stream.end()) {
        log_warn("stream has unread data, you may lost data.");
    }

    return ret;
}

int BlsRtmpProtocol::encodeAny(MStream &stream, BlsAMF0Any *arg1, BlsAMF0Any *arg2, BlsAMF0Any *arg3, BlsAMF0Any *arg4, BlsAMF0Any *arg5, BlsAMF0Any *arg6)
{
    int ret = E_SUCCESS;

    if (arg1) {
        if ((ret = BlsAMF0Serializer::write(stream, arg1)) != E_SUCCESS) {
            return ret;
        }
    }

    if (arg2) {
        if ((ret = BlsAMF0Serializer::write(stream, arg2)) != E_SUCCESS) {
            return ret;
        }
    }

    if (arg3) {
        if ((ret = BlsAMF0Serializer::write(stream, arg3)) != E_SUCCESS) {
            return ret;
        }
    }

    if (arg4) {
        if ((ret = BlsAMF0Serializer::write(stream, arg4)) != E_SUCCESS) {
            return ret;
        }
    }

    if (arg5) {
        if ((ret = BlsAMF0Serializer::write(stream, arg5)) != E_SUCCESS) {
            return ret;
        }
    }

    if (arg6) {
        if ((ret = BlsAMF0Serializer::write(stream, arg6)) != E_SUCCESS) {
            return ret;
        }
    }

    return ret;
}

int BlsRtmpProtocol::fillStream(int size, MStream &stream)
{
    char buf[size];
    if (m_socket->readFully(buf, size) != size) {
        return -1;
    }
    stream.append(buf, size);

    return 0;
}

int BlsRtmpProtocol::responeAck()
{
    int ret = E_SUCCESS;

    muint64 recvBytes = m_socket->recvBytes();
    int ackedSize = recvBytes - m_rtmpCtx.ackedSize;

    if (m_rtmpCtx.windowAcknowledgementSize > 0 && ackedSize >= m_rtmpCtx.windowAcknowledgementSize) {
        BlsRtmpMessage *msg = new BlsRtmpMessage;
        mAutoFree(BlsRtmpMessage, msg);

        msg->payload.write4Bytes(ackedSize);

        BlsRtmpMessageHeader &header = msg->header;
        header.perfer_cid = RTMP_CID_ProtocolControl;
        header.type = RTMP_MSG_Acknowledgement;
        header.payloadLength = msg->payload.size();
        header.streamID = m_rtmpCtx.streamID;

        if ((ret = send_message(msg)) != E_SUCCESS) {
            return ret;
        }

        m_rtmpCtx.ackedSize = recvBytes;
    }

    return ret;
}

BlsRtmpMessage *BlsRtmpProtocol::reEncodeMetadata(BlsRtmpMessage *metadata)
{
//    MStream &stream = metadata->payload;
//    if (stream.contains("onMetaData")) {

//    }

    return NULL;
}

BlsRtmpContext::BlsRtmpContext()
    : windowAcknowledgementSize(2500000)
    , streamID(0)
    , outChunkSize(128)
    , inChunkSize(128)
    , bufferLength(0)
    , ackedSize(0)
{
    rtmpUrl = new BlsRtmpUrl;
}

BlsRtmpContext::~BlsRtmpContext()
{
    mFree(rtmpUrl);
}

void BlsRtmpContext::setTcUrl(const MString &tu)
{
    tcUrl =  tu;
}

void BlsRtmpContext::setStreamName(const MString &name)
{
    MString url;
    if (tcUrl.endWith("/")) {
        url = tcUrl + name;
    } else {
        url = tcUrl + "/" + name;
    }

    rtmpUrl->setRtmpUrl(url);
}

MString BlsRtmpContext::url()
{
    return rtmpUrl->url();
}


BlsRtmpNetStatusEvent::BlsRtmpNetStatusEvent(const MString &code, const MString &level)
{
    if (!level.isEmpty())
        setValue(STATUS_LEVEL, new BlsAMF0ShortString(level));

    if (!code.isEmpty())
        setValue(STATUS_CODE, new BlsAMF0ShortString(code));
}

BlsRtmpNetStatusEvent::~BlsRtmpNetStatusEvent()
{

}
