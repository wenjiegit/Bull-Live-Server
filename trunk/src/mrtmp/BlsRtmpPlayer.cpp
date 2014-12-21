#include "BlsRtmpPlayer.hpp"

#include <MTcpSocket>
#include <MLoger>

#include "BlsRtmpUrl.hpp"
#include "BlsRtmpSource.hpp"

BlsRtmpPlayer::BlsRtmpPlayer(MObject *parent)
    : MThread(parent)
{

}

BlsRtmpPlayer::~BlsRtmpPlayer()
{

}

void BlsRtmpPlayer::setUrl(const MString &host, mint16 port, const MString &app, const MString &url)
{
    m_host = host;
    m_port = port;
    m_app = app;
    m_url = url;
}

int BlsRtmpPlayer::run()
{
    int ret = E_SUCCESS;

    mAssert(!m_url.empty());
    mAssert(!m_host.empty());
    mAssert(m_port > 0);

    while (!RequestStop) {
        m_socket = new MTcpSocket(this);
        mAutoFree(MTcpSocket, m_socket);

        if ((ret = m_socket->initSocket()) != E_SUCCESS) {
            log_error("MRtmpPlayer create socket error.");
            continue;
        }

        if ((ret = m_socket->connectToHost(m_host, m_port)) != E_SUCCESS) {
            log_error("MRtmpPlayer connect to %s:%d failed.", m_host.c_str(), m_port);
            continue;
        }

        m_protocol = new BlsRtmpProtocol(m_socket, this);
        m_protocol->setSession(this);

        if ((ret = service()) != E_SUCCESS) {
            log_error("MRtmpPlayer play error, ret=%d", ret);
            continue;
        }
    }

    return ret;
}

int BlsRtmpPlayer::onCommand(BlsRtmpMessage *msg, const MString &name, double transactionID
                           , BlsAMF0Any *arg1, BlsAMF0Any *arg2, BlsAMF0Any *arg3, BlsAMF0Any *arg4)
{
    int ret = E_SUCCESS;

    if (name == "_result") {
        MString command = findCommand(transactionID);
        if (command == RTMP_AMF0_COMMAND_CONNECT) {
            BlsAMF0Object *obj = dynamic_cast<BlsAMF0Object *>(arg2);
            if (!obj) {
                return E_AMF_TYPE_ERROR;
            }
            MString level = obj->value("level");
            MString code = obj->value("code");

            m_commandList.erase(transactionID);

            log_trace("level %s code %s", level.c_str(), code.c_str());
            if (code != NetConnection_Connect_Success) {
                return E_CONNECTION_FAILED;
            }
        } else if (command == RTMP_AMF0_COMMAND_CREATE_STREAM) {
            m_commandList.erase(transactionID);
            BlsAMF0Number *streamID = dynamic_cast<BlsAMF0Number *>(arg2);
            if (!streamID) {
                return E_AMF_TYPE_ERROR;
            }
            m_protocol->getRtmpCtx()->streamID = streamID->var;

            log_trace("MRtmpPlayer Stream ID set to %d", streamID->var);

            // play stream
            BlsRtmpUrl url(m_url);
            if ((ret = play(url.stream())) != E_SUCCESS) {
                return ret;
            }

            if ((ret = m_protocol->setUCM(UCM_SetBufferLength, streamID->var, 300)) != E_SUCCESS) {
                return ret;
            }
        }
    } else if (name == "onBWDone") {
        if ((ret = m_protocol->setAckSize(2500000)) != E_SUCCESS) {
            return ret;
        }

        if ((ret = createStream()) != E_SUCCESS) {
            return ret;
        }
    } else if (name == RTMP_AMF0_COMMAND_ON_STATUS) {
    }

    else {
        log_error("MRtmpPlayer onCommand : no method \"%s\"", name.c_str());
        return E_INVOKE_NO_METHOD;
    }

    return ret;
}

int BlsRtmpPlayer::onAudio(BlsRtmpMessage *msg)
{
    m_source->onAudio(*msg);

    return E_SUCCESS;
}

int BlsRtmpPlayer::onVideo(BlsRtmpMessage *msg)
{
    m_source->onVideo(*msg);
    return E_SUCCESS;
}

int BlsRtmpPlayer::onMetadata(BlsRtmpMessage *msg)
{
    m_source->onMetadata(*msg);

    return E_SUCCESS;
}

MString BlsRtmpPlayer::findCommand(double id)
{
    if (m_commandList.contains(id)) {
        return m_commandList[id];
    }

    return "";
}

int BlsRtmpPlayer::connectApp()
{
    int ret = E_SUCCESS;
    MString commandName = RTMP_AMF0_COMMAND_CONNECT;
    double  transactionID = 1;
    BlsRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection);

    BlsAMF0Object *obj = new BlsAMF0Object;
    obj->setValue("app", new BlsAMF0ShortString("live"));
    obj->setValue("flashVer", new BlsAMF0ShortString("WIN 14,0,0,125"));
    obj->setValue("swfUrl", new BlsAMF0ShortString("http://www.cutv.com/demo/live_test.swf"));

    BlsRtmpUrl url(m_url);

    obj->setValue("tcUrl", new BlsAMF0ShortString(url.tcUrl()));
    obj->setValue("fpad", new BlsAMF0Boolean);
    obj->setValue("capabilities", new BlsAMF0Number(239));
    obj->setValue("audioCodecs", new BlsAMF0Number(3575));
    obj->setValue("videoCodecs", new BlsAMF0Number(251));
    obj->setValue("videoFunction", new BlsAMF0Number(1));
    obj->setValue("pageUrl", new BlsAMF0ShortString("http://www.cutv.com/demo/live_test.swf"));
    obj->setValue("objectEncoding", new BlsAMF0Number(3));

    if ((ret = m_protocol->sendAny(header, new BlsAMF0ShortString(commandName), new BlsAMF0Number(transactionID), obj)) != E_SUCCESS) {
        log_error("MRtmpPlayer connectApp failed.");
        return ret;
    }
    m_commandList[transactionID] = commandName;

    return ret;
}

int BlsRtmpPlayer::createStream()
{
    int ret = E_SUCCESS;
    MString commandName = RTMP_AMF0_COMMAND_CREATE_STREAM;
    double  transactionID = 1;
    BlsRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection);

    if ((ret = m_protocol->sendAny(header, new BlsAMF0ShortString(commandName), new BlsAMF0Number(transactionID), new BlsAMF0Null)) != E_SUCCESS) {
        log_error("MRtmpPlayer connectApp failed.");
        return ret;
    }
    m_commandList[transactionID] = commandName;

    return ret;
}

int BlsRtmpPlayer::play(const MString &streamName)
{
    int ret = E_SUCCESS;
    MString commandName = RTMP_AMF0_COMMAND_PLAY;
    double  transactionID = 0;
    BlsRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverStream2);

    if ((ret = m_protocol->sendAny(header, new BlsAMF0ShortString(commandName), new BlsAMF0Number(transactionID)
                                   , new BlsAMF0Null, new BlsAMF0ShortString(streamName))) != E_SUCCESS) {
        log_error("MRtmpPlayer play failed.");
        return ret;
    }

    return ret;
}

int BlsRtmpPlayer::service()
{
    int ret = E_SUCCESS;

    if ((ret = m_protocol->handshakeWithServer(false)) != E_SUCCESS) {
        log_error("MRtmpPlayer handshake with server failed.");
        return ret;
    }

    if ((ret = connectApp()) != E_SUCCESS) {
        return ret;
    }

    BlsRtmpUrl url(m_url);
    m_source = BlsRtmpSource::findSource(url.url());

    while (!RequestStop) {
        BlsRtmpMessage *msg = NULL;
        if ((ret = m_protocol->recv_message(&msg)) != E_SUCCESS) {
            return ret;
        }

        mMSleep(10);
    }

    return ret;
}
