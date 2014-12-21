
/*
 * Copyright (C) wenjie.zhao
 */


#include "mrtmpconnection.hpp"
#include "mrtmphandshake.hpp"
#include "mrtmpprotocol.hpp"
#include "BlsRtmpSource.hpp"
#include "BlsConsumer.hpp"
#include "BlsConf.hpp"
#include "BlsChildChannel.hpp"
#include "BlsUtils.hpp"
#include "BlsBackSource.hpp"
#include "BlsServerSelector.hpp"

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

}

int MRtmpConnection::run()
{
    int ret = E_SUCCESS;

    if ((ret = m_protocol->handshakeWithClient()) != E_SUCCESS) {
        return ret;
    }

    while (!RequestStop) {
        MRtmpMessage *msg = NULL;
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
    log_warn("%s", name.c_str());
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
        MRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection);

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID), new MAMF0Null, new MAMF0Undefined)) != E_SUCCESS) {
            return ret;
        }
    } else if (name == "FCPublish") {
        MString cmdName = RTMP_AMF0_COMMAND_RESULT;
        MRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection);

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID), new MAMF0Null, new MAMF0Undefined)) != E_SUCCESS) {
            return ret;
        }
    } else if (name == "createStream") {
        MString cmdName = RTMP_AMF0_COMMAND_RESULT;
        MRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection);

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID)
                                       , new MAMF0Null, new MAMF0Number(1))) != E_SUCCESS)
        {
            return ret;
        }
        m_protocol->getRtmpCtx()->streamID = 1;
    } else if (name == "publish") {

        // the stream name is the second arg.
        MAMF0ShortString *str = dynamic_cast<MAMF0ShortString *>(arg2);
        if (!str) {
            return E_AMF_TYPE_ERROR;
        }

        MRtmpContext *ctx = m_protocol->getRtmpCtx();
        ctx->setStreamName(str->var);

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
        MRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverStream);

        MRtmpNetStatusEvent *obj = new MRtmpNetStatusEvent(NetStream_Publish_Start);
        obj->setValue(STATUS_DESC, new MAMF0ShortString(NetStream_Publish_Start));

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID), new MAMF0Null, obj)) != E_SUCCESS) {
            return ret;
        }

        MRtmpNetStatusEvent *obj1 = new MRtmpNetStatusEvent(NetStream_Publish_Start, STATUS_LEVEL_STATUS);
        obj1->setValue(STATUS_DESC, new MAMF0ShortString(NetStream_Publish_Start));
        obj1->setValue(STATUS_CLIENT_ID, new MAMF0ShortString("ASAICiss"));

        if ((ret = m_protocol->sendNetStatusEvent(transactionID, obj1)) != E_SUCCESS) {
            return ret;
        }

        m_role = Role_Connection_Publish;

        log_trace("start publish %s", url.c_str());

        BlsBackSource::instance()->setHasBackSource(url);

        // on_publish
        if ((ret = m_source->onPublish()) != E_SUCCESS) {
            log_error("on publish error. ret=%d", ret);
            return ret;
        }

    } else if (name == "FCUnpublish") {
        MString cmdName = "onFCUnpublish";
        MAMF0Object *obj = new MAMF0Object;
        obj->setValue(STATUS_CODE, new MAMF0ShortString(NetStream_Unpublish_Success));
        obj->setValue(STATUS_DESC, new MAMF0ShortString(NetStream_Unpublish_Success));

        MRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection);

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID), new MAMF0Null, obj)) != E_SUCCESS) {
            return ret;
        }
    } else if (name == "closeStream") {

        log_warn("---------------->");
        m_protocol->getRtmpCtx()->streamID = 0;
        if (m_source) {
            m_source->onUnPublish();
        }
        log_warn("---------------->--------");


        MString cmdName = RTMP_AMF0_COMMAND_RESULT;
        MRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection);

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
    } else if (name == "deleteStream") {
        MString cmdName = RTMP_AMF0_COMMAND_RESULT;
        MRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection);

        if ((ret = m_protocol->sendAny(header, new MAMF0ShortString(cmdName), new MAMF0Number(transactionID)
                                       , new MAMF0Null, new MAMF0Null)) != E_SUCCESS)
        {
            return ret;
        }
    } else if (name == "play") {
        if ((ret = m_protocol->setChunkSize(40960)) != E_SUCCESS) {
            return ret;
        }

        if ((ret = m_protocol->setUCM(UCM_StreamBegin, m_protocol->getRtmpCtx()->streamID)) != E_SUCCESS) {
            return ret;
        }

        MRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection2);
        MString cmdName = RTMP_AMF0_COMMAND_ON_STATUS;

        MRtmpNetStatusEvent *obj = new MRtmpNetStatusEvent(NetStream_Play_Reset, STATUS_LEVEL_STATUS);
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

        MString vhost = ctx->rtmpUrl->vhost();
        MString mode = BlsConf::instance()->getMode(vhost);
        MString fullUrl = ctx->rtmpUrl->fullUrl();

        int role = BlsConf::instance()->processRole();
        bool hasBackSource = BlsBackSource::instance()->hasBackSource(fullUrl);

        if (role == Process_Role_BackSource) {
            log_trace("i am here.");
        } else if (role == Process_Role_Worker) {
            muint16 port = BlsServerSelector::instance()->lookUp(url);
            if (!hasBackSource) {
                // back source to local server
                BlsBackSource::instance()->add("127.0.0.1", port, ctx->rtmpUrl->app(), ctx->rtmpUrl->fullUrl());
                log_trace("url %s back source to local server at %d", url.c_str(), port);
//                if (mode == Mode_Remote) {
//                    if (port == 0) {
//                        // back source to local server
//                        BlsBackSource::instance()->add(ctx->rtmpUrl->vhost(), ctx->rtmpUrl->port(), ctx->rtmpUrl->app(), ctx->rtmpUrl->fullUrl());
//                        log_trace("begin back source to %s:%d pid=%d", ctx->rtmpUrl->vhost().c_str(), ctx->rtmpUrl->port(), getpid());
//                    } else if (port > 0) {
//                        // back source to origin server
//                        BlsBackSource::instance()->add("127.0.0.1", port, ctx->rtmpUrl->app(), ctx->rtmpUrl->fullUrl());
//                        log_trace("begin back source to %s:%d pid=%d", "127.0.0.1", port, getpid());
//                    }
//                } else if (mode == Mode_Local) {
//                    BlsBackSource::instance()->add("127.0.0.1", port, ctx->rtmpUrl->app(), ctx->rtmpUrl->fullUrl());      // back source to origin server
//                }
            }
        } else {
            mAssert(false);
        }

        log_trace("start play : %s", fullUrl.c_str());

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

int MRtmpConnection::onVideo(MRtmpMessage *msg)
{
    return m_source->onVideo(*msg);
}

int MRtmpConnection::onMetadata(MRtmpMessage *msg)
{
    return m_source->onMetadata(*msg);
}

int MRtmpConnection::publishService()
{
    return E_SUCCESS;
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

int MRtmpConnection::closeConnection()
{
    mMSleep(500);
    return E_SOCKET_CLOSE_NORMALLY;
}

int MRtmpConnection::playService()
{
    int ret = E_SUCCESS;

    MString url = m_protocol->getRtmpCtx()->url();
    BlsConsumer *pool = new BlsConsumer(url, this);
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

        mMSleep(30);
    }

    return ret;
}

int MRtmpConnection::onAudio(MRtmpMessage *msg)
{
    return m_source->onAudio(*msg);
}
