
/*
 * Copyright (C) wenjie.zhao
 */


#include "BlsRtmpConnection.hpp"
#include "BlsRtmpHandshake.hpp"
#include "BlsRtmpProtocol.hpp"
#include "BlsRtmpSource.hpp"
#include "BlsConsumer.hpp"
#include "BlsConf.hpp"
#include "BlsChildChannel.hpp"
#include "BlsUtils.hpp"
#include "BlsBackSource.hpp"
#include "BlsServerSelector.hpp"

#include <MTcpSocket>
#include <MLoger>

BlsRtmpConnection::BlsRtmpConnection(MObject *parent)
    : MThread(parent)
    , m_socket(NULL)
    , m_protocol(NULL)
    , m_source(NULL)
{

}

BlsRtmpConnection::~BlsRtmpConnection()
{

}

int BlsRtmpConnection::run()
{
    int ret = E_SUCCESS;

    if ((ret = m_protocol->handshakeWithClient()) != E_SUCCESS) {
        return ret;
    }

    while (!RequestStop) {
        BlsRtmpMessage *msg = NULL;
        int res = m_protocol->recv_message(&msg);
        if (res == E_SOCKET_CLOSE_NORMALLY) {
            break;
        }

        if (res != E_SUCCESS) {
            log_error("MRtmpConnection recv_message error.");
            break;
        }

        mMSleep(0);
    }

    if (m_role == Role_Connection_Publish) {
        MString url = m_protocol->getRtmpCtx()->url();
        BlsBackSource::instance()->remove(url);

        if (m_source) {
            m_source->onUnPublish();
        }
    }

    m_socket->close();
    deleteLater();

    log_trace("MRtmpConnection exit normally.");

    return E_SUCCESS;
}

void BlsRtmpConnection::setUrl(const MString &url)
{
    m_url.setRtmpUrl(url);
}

void BlsRtmpConnection::setSocket(MTcpSocket *socket)
{
    m_socket = socket;
    m_protocol = new BlsRtmpProtocol(socket, this);
    m_protocol->setSession(this);
}

int BlsRtmpConnection::onCommand(BlsRtmpMessage *msg, const MString &name, double transactionID, BlsAMF0Any *arg1
                               , BlsAMF0Any *arg2, BlsAMF0Any *arg3, BlsAMF0Any *arg4)
{
    log_warn("%s", name.c_str());
    // TODO refer check.
    // TODO set app msg to protocol
    int ret = E_SUCCESS;

    if (name == "connect") {
        if (!arg1->isAmf0Object()) return E_AMF_TYPE_ERROR;

        BlsAMF0Object *obj = dynamic_cast<BlsAMF0Object *>(arg1);
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
        BlsRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection);

        if ((ret = m_protocol->sendAny(header, new BlsAMF0ShortString(cmdName), new BlsAMF0Number(transactionID), new BlsAMF0Null, new BlsAMF0Undefined)) != E_SUCCESS) {
            return ret;
        }
    } else if (name == "FCPublish") {
        MString cmdName = RTMP_AMF0_COMMAND_RESULT;
        BlsRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection);

        if ((ret = m_protocol->sendAny(header, new BlsAMF0ShortString(cmdName), new BlsAMF0Number(transactionID), new BlsAMF0Null, new BlsAMF0Undefined)) != E_SUCCESS) {
            return ret;
        }
    } else if (name == "createStream") {
        MString cmdName = RTMP_AMF0_COMMAND_RESULT;
        BlsRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection);

        if ((ret = m_protocol->sendAny(header, new BlsAMF0ShortString(cmdName), new BlsAMF0Number(transactionID)
                                       , new BlsAMF0Null, new BlsAMF0Number(1))) != E_SUCCESS)
        {
            return ret;
        }
        m_protocol->getRtmpCtx()->streamID = 1;
    } else if (name == "publish") {

        // the stream name is the second arg.
        BlsAMF0ShortString *str = dynamic_cast<BlsAMF0ShortString *>(arg2);
        if (!str) {
            return E_AMF_TYPE_ERROR;
        }

        BlsRtmpContext *ctx = m_protocol->getRtmpCtx();
        ctx->setStreamName(str->var);

        // check vhost
        m_vhost = ctx->rtmpUrl->vhost();
        if (!BlsConf::instance()->containsVhost(m_vhost)) {
            if (BlsConf::instance()->useDefaultVhost()) {
                m_vhost = BLS_DEFAULT_VHOST;
            } else {
                log_error_with_errno(E_VHOST_NOT_EXIST, "can not find vhost.");
                return E_VHOST_NOT_EXIST;
            }
        }

        MString url = ctx->rtmpUrl->url();
        m_source = BlsRtmpSource::findSource(url);

        // if process is worker
        // then check if has other client push the same stream.
        int role = BlsConf::instance()->processRole();
        if (role == Process_Role_Worker) {
            bool isUsed = false;
            ret = m_source->acquire(url, isUsed);
            mAssert(ret == E_SUCCESS);

            if (isUsed) {
                log_error_with_errno(E_STREAM_BADNAME, "stream is in used.");
                return E_STREAM_BADNAME;
            }
        }

        MString cmdName = "FCPublish";
        BlsRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverStream);

        BlsRtmpNetStatusEvent *obj = new BlsRtmpNetStatusEvent(NetStream_Publish_Start);
        obj->setValue(STATUS_DESC, new BlsAMF0ShortString(NetStream_Publish_Start));

        if ((ret = m_protocol->sendAny(header, new BlsAMF0ShortString(cmdName), new BlsAMF0Number(transactionID), new BlsAMF0Null, obj)) != E_SUCCESS) {
            return ret;
        }

        BlsRtmpNetStatusEvent *obj1 = new BlsRtmpNetStatusEvent(NetStream_Publish_Start, STATUS_LEVEL_STATUS);
        obj1->setValue(STATUS_DESC, new BlsAMF0ShortString(NetStream_Publish_Start));
        obj1->setValue(STATUS_CLIENT_ID, new BlsAMF0ShortString("ASAICiss"));

        if ((ret = m_protocol->sendNetStatusEvent(transactionID, obj1)) != E_SUCCESS) {
            return ret;
        }

        m_role = Role_Connection_Publish;

        BlsBackSource::instance()->setHasBackSource(url);

        // on_publish
        if ((ret = m_source->onPublish(m_vhost)) != E_SUCCESS) {
            log_error("on publish error. ret=%d", ret);
            return ret;
        }
        log_trace("begin publish %s", url.c_str());

    } else if (name == "FCUnpublish") {
        MString cmdName = "onFCUnpublish";
        BlsAMF0Object *obj = new BlsAMF0Object;
        obj->setValue(STATUS_CODE, new BlsAMF0ShortString(NetStream_Unpublish_Success));
        obj->setValue(STATUS_DESC, new BlsAMF0ShortString(NetStream_Unpublish_Success));

        BlsRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection);

        if ((ret = m_protocol->sendAny(header, new BlsAMF0ShortString(cmdName), new BlsAMF0Number(transactionID), new BlsAMF0Null, obj)) != E_SUCCESS) {
            return ret;
        }
    } else if (name == "closeStream") {
        MString cmdName = RTMP_AMF0_COMMAND_RESULT;
        BlsRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection);

        if ((ret = m_protocol->sendAny(header, new BlsAMF0ShortString(cmdName), new BlsAMF0Number(transactionID)
                                       , new BlsAMF0Null, new BlsAMF0Undefined)) != E_SUCCESS)
        {
            return ret;
        }

        cmdName = RTMP_AMF0_COMMAND_ON_STATUS;
        BlsAMF0Object *obj1 = new BlsAMF0Object;
        obj1->setValue(STATUS_LEVEL, new BlsAMF0ShortString(STATUS_LEVEL_STATUS));
        obj1->setValue(STATUS_CODE, new BlsAMF0ShortString(NetStream_Unpublish_Success));
        obj1->setValue(STATUS_DESC, new BlsAMF0ShortString(NetStream_Unpublish_Success));
        obj1->setValue(STATUS_CLIENT_ID, new BlsAMF0ShortString("ASAICiss"));

        if ((ret = m_protocol->sendAny(header, new BlsAMF0ShortString(cmdName), new BlsAMF0Number(transactionID), new BlsAMF0Null, obj1)) != E_SUCCESS) {
            return ret;
        }
    } else if (name == "deleteStream") {
        // do clean.
        if (m_source) {
            m_source->onUnPublish();
        }

        // send delete result.
        MString cmdName = RTMP_AMF0_COMMAND_RESULT;
        BlsRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection);

        if ((ret = m_protocol->sendAny(header, new BlsAMF0ShortString(cmdName), new BlsAMF0Number(transactionID)
                                       , new BlsAMF0Null, new BlsAMF0Null)) != E_SUCCESS)
        {
            return ret;
        }

        // release stream id
        m_protocol->getRtmpCtx()->streamID = 0;

    } else if (name == "play") {
        if ((ret = m_protocol->setChunkSize(BlsConf::instance()->chunkSize())) != E_SUCCESS) {
            return ret;
        }

        if ((ret = m_protocol->setUCM(UCM_StreamBegin, m_protocol->getRtmpCtx()->streamID)) != E_SUCCESS) {
            return ret;
        }

        BlsRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection2);
        MString cmdName = RTMP_AMF0_COMMAND_ON_STATUS;

        BlsRtmpNetStatusEvent *obj = new BlsRtmpNetStatusEvent(NetStream_Play_Reset, STATUS_LEVEL_STATUS);
        obj->setValue(STATUS_DESC, new BlsAMF0ShortString(NetStream_Play_Reset));
        obj->setValue(STATUS_DETAILS, new BlsAMF0ShortString(NetStream_Play_Reset));
        obj->setValue(STATUS_CLIENT_ID, new BlsAMF0ShortString("ASAICiss"));

        if ((ret = m_protocol->sendAny(header, new BlsAMF0ShortString(cmdName), new BlsAMF0Number(transactionID), new BlsAMF0Null, obj)) != E_SUCCESS) {
            return ret;
        }

        BlsAMF0Object *obj1 = new BlsAMF0Object;
        obj1->setValue(STATUS_LEVEL, new BlsAMF0ShortString(STATUS_LEVEL_STATUS));
        obj1->setValue(STATUS_CODE, new BlsAMF0ShortString(NetStream_Play_Start));
        obj1->setValue(STATUS_DESC, new BlsAMF0ShortString(NetStream_Play_Start));
        obj1->setValue(STATUS_DETAILS, new BlsAMF0ShortString(NetStream_Play_Start));
        obj1->setValue(STATUS_CLIENT_ID, new BlsAMF0ShortString("ASAICiss"));

        if ((ret = m_protocol->sendAny(header, new BlsAMF0ShortString(cmdName), new BlsAMF0Number(transactionID), new BlsAMF0Null, obj1)) != E_SUCCESS) {
            return ret;
        }

        header.type = RTMP_MSG_AMF0DataMessage;
        cmdName = RTMP_AMF0_DATA_SAMPLE_ACCESS;
        if ((ret = m_protocol->sendAny(header, new BlsAMF0ShortString(cmdName), new BlsAMF0Boolean(false), new BlsAMF0Boolean(false))) != E_SUCCESS) {
            return ret;
        }

        BlsAMF0ShortString *str = dynamic_cast<BlsAMF0ShortString *>(arg2);
        if (!str) {
            return E_AMF_TYPE_ERROR;
        }

        BlsRtmpContext *ctx = m_protocol->getRtmpCtx();
        ctx->setStreamName(str->var);
        MString url = ctx->rtmpUrl->url();

        m_vhost = ctx->rtmpUrl->vhost();
        // check vhost
        if (!BlsConf::instance()->containsVhost(m_vhost)) {
            if (BlsConf::instance()->useDefaultVhost()) {
                m_vhost = BLS_DEFAULT_VHOST;
            } else {
                log_error_with_errno(E_VHOST_NOT_EXIST, "can not find vhost.");
                return E_VHOST_NOT_EXIST;
            }
        }

        MString mode = BlsConf::instance()->getMode(m_vhost);
        MString fullUrl = ctx->rtmpUrl->fullUrl();

        int role = BlsConf::instance()->processRole();
        bool hasBackSource = BlsBackSource::instance()->hasBackSource(fullUrl);

        if (role == Process_Role_BackSource) {
            // if it's in remote mode, back source to origin server
            if (mode == Mode_Remote) {
                vector<BlsHostInfo> origs = BlsConf::instance()->getOriginInfo(m_vhost);
                if (origs.empty()) {
                    log_warn("origin server should not be empty");
                    ret = merrno = E_ORIGIN_NOT_EXIST;
                    return ret;
                }

                // TODO fix support multi-hosts
                // currently use the first origin address
                const BlsHostInfo & origin = origs.at(0);
                BlsBackSource::instance()->add(origin.addr, origin.port, ctx->rtmpUrl->app(), ctx->rtmpUrl->fullUrl());

                log_trace("pull stream from orgin server %s:%d", origin.addr.c_str(), origin.port);
            }
        } else if (role == Process_Role_Worker) {
            muint16 port = BlsServerSelector::instance()->lookUp(url);
            if (!hasBackSource) {
                // back source to local server
                BlsBackSource::instance()->add("127.0.0.1", port, ctx->rtmpUrl->app(), ctx->rtmpUrl->fullUrl());

                log_trace("pull stream from local server %s:%d", url.c_str(), port);
            }
        } else {
            mAssert(false);
        }

        log_trace("play stream: %s", fullUrl.c_str());

        m_source = BlsRtmpSource::findSource(url);
        m_role = Role_Connection_Play;
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

int BlsRtmpConnection::onVideo(BlsRtmpMessage *msg)
{
    return m_source->onVideo(*msg);
}

int BlsRtmpConnection::onMetadata(BlsRtmpMessage *msg)
{
    return m_source->onMetadata(*msg);
}

int BlsRtmpConnection::publishService()
{
    return E_SUCCESS;
}

int BlsRtmpConnection::parseUrl(BlsAMF0Object *obj)
{
    int ret = E_SUCCESS;

    int index = obj->indexOf("tcUrl");
    if (index < 0) {
        ret = E_URL_NO_TCURL;
        return ret;
    }
    BlsAMF0Any *value = obj->value(index);
    BlsAMF0ShortString *str = dynamic_cast<BlsAMF0ShortString *>(value);
    if (!str) {
        return E_AMF_TYPE_ERROR;
    }
    m_protocol->getRtmpCtx()->setTcUrl(str->var);

    return ret;
}

int BlsRtmpConnection::closeConnection()
{
    mMSleep(500);
    return E_SOCKET_CLOSE_NORMALLY;
}

int BlsRtmpConnection::playService()
{
    int ret = E_SUCCESS;

    MString url = m_protocol->getRtmpCtx()->url();
    BlsConsumer *consumer = new BlsConsumer(url, this);
    m_source->addPool(consumer);
    mAutoFree(BlsConsumer, consumer);

    while (!RequestStop) {
        list<BlsRtmpMessage> msgs = consumer->getMessage();

        list<BlsRtmpMessage>::iterator iter;
        for (iter = msgs.begin(); iter != msgs.end(); ++iter) {
            BlsRtmpMessage &msg = *iter;
            if ((ret = m_protocol->send_message(&msg)) != E_SUCCESS) {
                return ret;
            }
        }

        mMSleep(30);
    }

    return ret;
}

int BlsRtmpConnection::onAudio(BlsRtmpMessage *msg)
{
    return m_source->onAudio(*msg);
}
