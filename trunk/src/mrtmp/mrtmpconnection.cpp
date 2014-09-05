
/*
 * Copyright (C) wenjie.zhao
 * Copyright (C) www.17173.com
 */


#include "mrtmpconnection.hpp"
#include "srs_core_handshake.hpp"
#include "mrtmpprotocol.hpp"
#include "mrtmpsource.hpp"
#include "mrtmppool.hpp"

#include <MTcpSocket>
#include <MLoger>

MRtmpConnection::MRtmpConnection(MObject *parent)
    : MThread(parent)
    , m_socket(NULL)
    , m_protocol(NULL)
    , m_source(NULL)
{
}

MRtmpConnection::~MRtmpConnection()
{
    log_trace("------------------> MRtmpConnection destroy.");
}

int MRtmpConnection::run()
{
    int ret = E_SUCCESS;

    if ((ret = m_protocol->handshakeWithClient()) != E_SUCCESS) {
        return ret;
    }

    while (!RequestStop) {
        MRtmpMessage *msg = NULL;
        if (m_protocol->recv_message(&msg) != E_SUCCESS)
        {
            log_error("MRtmpConnection recv_message error.");
            break;
        }
        mMSleep(10);
    }

    m_socket->close();
    deleteLater();

    log_trace("MRtmpConnection exit normally.");

    return E_SUCCESS;
}

void MRtmpConnection::setUrl(const MString &url)
{
    m_url.setRtmpUrl(url);
}

void MRtmpConnection::setSocket(MTcpSocket *socket)
{
    m_socket = socket;
    m_protocol = new MRtmpProtocol(socket, this);
    m_protocol->setSession(this);
}

int MRtmpConnection::onCommand(MRtmpMessage *msg, const MString &name, double transactionID, MAMF0Any *arg1
                               , MAMF0Any *arg2, MAMF0Any *arg3, MAMF0Any *arg4)
{
    // TODO refer check.
    // TODO set app msg to protocol
    int ret = E_SUCCESS;

    if (name == "connect") {
        if (!arg1->isAmf0Object()) return E_AMF_TYPE_ERROR;

        MAMF0Object *obj = dynamic_cast<MAMF0Object *>(arg1);
        if ((ret = parseUrl(obj)) != E_SUCCESS) {
            return ret;
        }

        if ((ret = m_protocol->setAckSize(2500000)) != E_SUCCESS) {
            return ret;
        }

        if ((ret = m_protocol->setPeerBandwidth(2500000)) != E_SUCCESS) {
            return ret;
        }

        if ((ret = m_protocol->onConnect(transactionID)) != E_SUCCESS) {
            return ret;
        }

        if ((ret = m_protocol->onBWDone()) != E_SUCCESS) {
            return ret;
        }
    } else if (name == "releaseStream") {
        MString cmdName = RTMP_AMF0_COMMAND_RESULT;
        MRtmpMessageHeader header;
        header.perfer_cid = RTMP_CID_OverConnection;
        header.type = RTMP_MSG_AMF0CommandMessage;

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID), new MAMF0Null, new MAMF0Undefined)) != E_SUCCESS) {
            return ret;
        }
    } else if (name == "FCPublish") {
        MString cmdName = RTMP_AMF0_COMMAND_RESULT;
        MRtmpMessageHeader header;
        header.perfer_cid = RTMP_CID_OverConnection;
        header.type = RTMP_MSG_AMF0CommandMessage;

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID), new MAMF0Null, new MAMF0Undefined)) != E_SUCCESS) {
            return ret;
        }
    } else if (name == "createStream") {
        MString cmdName = RTMP_AMF0_COMMAND_RESULT;
        MRtmpMessageHeader header;
        header.perfer_cid = RTMP_CID_OverConnection;
        header.type = RTMP_MSG_AMF0CommandMessage;

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID)
                                       , new MAMF0Null, new MAMF0Number(1))) != E_SUCCESS)
        {
            return ret;
        }
        m_protocol->getRtmpCtx()->streamID = 1;
    } else if (name == "publish") {
        MString cmdName = "FCPublish";
        MAMF0Object *obj = new MAMF0Object;
        obj->setValue(STATUS_CODE, new MAMF0ShortString(NetStream_Publish_Start));
        obj->setValue(STATUS_DESC, new MAMF0ShortString(NetStream_Publish_Start));

        MRtmpMessageHeader header;
        header.perfer_cid = RTMP_CID_OverStream;
        header.type = RTMP_MSG_AMF0CommandMessage;

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID), new MAMF0Null, obj)) != E_SUCCESS) {
            return ret;
        }

        cmdName = RTMP_AMF0_COMMAND_ON_STATUS;
        MAMF0Object *obj1 = new MAMF0Object;
        obj1->setValue(STATUS_LEVEL, new MAMF0ShortString(STATUS_LEVEL_STATUS));
        obj1->setValue(STATUS_CODE, new MAMF0ShortString(NetStream_Publish_Start));
        obj1->setValue(STATUS_DESC, new MAMF0ShortString(NetStream_Publish_Start));
        obj1->setValue(STATUS_CLIENT_ID, new MAMF0ShortString("ASAICiss"));

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID), new MAMF0Null, obj1)) != E_SUCCESS) {
            return ret;
        }

        MAMF0ShortString *str = dynamic_cast<MAMF0ShortString *>(arg2);
        if (!str) {
            return E_AMF_TYPE_ERROR;
        }
        MRtmpContext *ctx = m_protocol->getRtmpCtx();
        ctx->setStreamName(str->var);

        MString url = ctx->rtmpUrl->url();
        m_source = new MRtmpSource(url, this);
        log_trace("start publish %s", url.c_str());

    } else if (name == "FCUnpublish") {
        MString cmdName = "onFCUnpublish";
        MAMF0Object *obj = new MAMF0Object;
        obj->setValue(STATUS_CODE, new MAMF0ShortString(NetStream_Unpublish_Success));
        obj->setValue(STATUS_DESC, new MAMF0ShortString(NetStream_Unpublish_Success));

        MRtmpMessageHeader header;
        header.perfer_cid = RTMP_CID_OverConnection;
        header.type = RTMP_MSG_AMF0CommandMessage;

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID), new MAMF0Null, obj)) != E_SUCCESS) {
            return ret;
        }
    } else if (name == "closeStream") {
        MString cmdName = RTMP_AMF0_COMMAND_RESULT;
        MRtmpMessageHeader header;
        header.perfer_cid = RTMP_CID_OverConnection;
        header.type = RTMP_MSG_AMF0CommandMessage;

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID)
                                       , new MAMF0Null, new MAMF0Undefined)) != E_SUCCESS)
        {
            return ret;
        }

        cmdName = RTMP_AMF0_COMMAND_ON_STATUS;
        MAMF0Object *obj1 = new MAMF0Object;
        obj1->setValue(STATUS_LEVEL, new MAMF0ShortString(STATUS_LEVEL_STATUS));
        obj1->setValue(STATUS_CODE, new MAMF0ShortString(NetStream_Unpublish_Success));
        obj1->setValue(STATUS_DESC, new MAMF0ShortString(NetStream_Unpublish_Success));
        obj1->setValue(STATUS_CLIENT_ID, new MAMF0ShortString("ASAICiss"));

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID), new MAMF0Null, obj1)) != E_SUCCESS) {
            return ret;
        }
        m_protocol->getRtmpCtx()->streamID = 0;
    } else if (name == "deleteStream") {
        MString cmdName = RTMP_AMF0_COMMAND_RESULT;
        MRtmpMessageHeader header;
        header.perfer_cid = RTMP_CID_OverConnection;
        header.type = RTMP_MSG_AMF0CommandMessage;

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID)
                                       , new MAMF0Null, new MAMF0Null)) != E_SUCCESS)
        {
            return ret;
        }
    } else if (name == "play") {
        if ((ret = m_protocol->setChunkSize(4096)) != E_SUCCESS) {
            return ret;
        }

        if ((ret = m_protocol->setUCM(UCM_StreamBegin, m_protocol->getRtmpCtx()->streamID)) != E_SUCCESS) {
            return ret;
        }

        MRtmpMessageHeader header;
        header.perfer_cid = RTMP_CID_OverConnection2;
        header.type = RTMP_MSG_AMF0CommandMessage;

        MString cmdName = RTMP_AMF0_COMMAND_ON_STATUS;

        MAMF0Object *obj = new MAMF0Object;
        obj->setValue(STATUS_LEVEL, new MAMF0ShortString(STATUS_LEVEL_STATUS));
        obj->setValue(STATUS_CODE, new MAMF0ShortString(NetStream_Play_Reset));
        obj->setValue(STATUS_DESC, new MAMF0ShortString(NetStream_Play_Reset));
        obj->setValue(STATUS_DETAILS, new MAMF0ShortString(NetStream_Play_Reset));
        obj->setValue(STATUS_CLIENT_ID, new MAMF0ShortString("ASAICiss"));

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID), new MAMF0Null, obj)) != E_SUCCESS) {
            return ret;
        }

        MAMF0Object *obj1 = new MAMF0Object;
        obj1->setValue(STATUS_LEVEL, new MAMF0ShortString(STATUS_LEVEL_STATUS));
        obj1->setValue(STATUS_CODE, new MAMF0ShortString(NetStream_Play_Start));
        obj1->setValue(STATUS_DESC, new MAMF0ShortString(NetStream_Play_Start));
        obj1->setValue(STATUS_DETAILS, new MAMF0ShortString(NetStream_Play_Start));
        obj1->setValue(STATUS_CLIENT_ID, new MAMF0ShortString("ASAICiss"));

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID), new MAMF0Null, obj1)) != E_SUCCESS) {
            return ret;
        }

        header.type = RTMP_MSG_AMF0DataMessage;
        cmdName = RTMP_AMF0_DATA_SAMPLE_ACCESS;
        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Boolean(false), new MAMF0Boolean(false))) != E_SUCCESS) {
            return ret;
        }

        MAMF0ShortString *str = dynamic_cast<MAMF0ShortString *>(arg2);
        if (!str) {
            return E_AMF_TYPE_ERROR;
        }

        MRtmpContext *ctx = m_protocol->getRtmpCtx();
        ctx->setStreamName(str->var);
        MString url = ctx->rtmpUrl->url();
        while (true) {
            MRtmpSource *source = MRtmpSource::findSource(url);
            if (!source) {
                mSleep(1);
                continue;
            }

            m_source = source;
            break;
        }

        log_trace("start play : %s", url.c_str());
        playService();
    } else if (name == "_checkbw") {
        return ret;
    }

    else {
        log_error("MRtmpConnection onCommand : no method \"%s\"", name.c_str());
        return E_INVOKE_NO_METHOD;
    }

    return ret;
}

int MRtmpConnection::onVideo(MRtmpMessage *msg)
{
    return m_source->onVideo(*msg);
}

int MRtmpConnection::onMetadata(MRtmpMessage *msg)
{
    int ret = E_SUCCESS;

    return ret;
}

int MRtmpConnection::publishService()
{

}

int MRtmpConnection::parseUrl(MAMF0Object *obj)
{
    int ret = E_SUCCESS;

    int index = obj->indexOf("tcUrl");
    if (index < 0) {
        ret = E_URL_NO_TCURL;
        return ret;
    }
    MAMF0Any *value = obj->value(index);
    MAMF0ShortString *str = dynamic_cast<MAMF0ShortString *>(value);
    if (!str) {
        return E_AMF_TYPE_ERROR;
    }
    m_protocol->getRtmpCtx()->setTcUrl(str->var);

    return ret;
}

int MRtmpConnection::playService()
{
    int ret = E_SUCCESS;

    MString url = m_protocol->getRtmpCtx()->url();
    MRtmpPool *pool = new MRtmpPool(url, this);
    m_source->addPool(pool);
    while (!RequestStop) {
        list<MRtmpMessage> msgs = pool->getMessage();

        list<MRtmpMessage>::iterator iter;
        for (iter = msgs.begin(); iter != msgs.end(); ++iter) {
            MRtmpMessage &msg = *iter;
            if ((ret = m_protocol->send_message(&msg)) != E_SUCCESS) {
                return ret;
            }
        }

        mMSleep(100);
    }

    return ret;
}

int MRtmpConnection::onAudio(MRtmpMessage *msg)
{
    return m_source->onAudio(*msg);
}
