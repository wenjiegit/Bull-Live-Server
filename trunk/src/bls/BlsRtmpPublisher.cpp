#include "BlsRtmpPublisher.hpp"

#include <MTcpSocket>

#include "BlsRtmpUrl.hpp"
#include "BlsRtmpSource.hpp"
#include "BlsConsumer.hpp"

BlsRtmpPublisher::BlsRtmpPublisher(MObject *parent)
    : MThread(parent)
    , m_protocol(NULL)
    , m_socket(NULL)
    , m_port(0)
{

}

BlsRtmpPublisher::~BlsRtmpPublisher()
{

}

int BlsRtmpPublisher::run()
{
    int ret = E_SUCCESS;

    mAssert(!m_url.empty());
    mAssert(!m_host.empty());
    mAssert(m_port > 0);

    while (!RequestStop) {

        // init
        m_socket = new MTcpSocket(this);
        mAutoFree(MTcpSocket, m_socket);
        mAutoSleep(1000);

        if ((ret = m_socket->initSocket()) != E_SUCCESS) {
            log_error("BlsRtmpPublisher create socket error.");
            continue;
        }

        if ((ret = m_socket->connectToHost(m_host, m_port)) != E_SUCCESS) {
            log_error("BlsRtmpPublisher connect to %s:%d failed, ret=%d"
                      , m_host.c_str(), m_port, ret);
            continue;
        }
        log_trace("BlsRtmpPublisher connect to %s:%d success.", m_host.c_str(), m_port);

        m_protocol = new BlsRtmpProtocol(m_socket, this);
        mAutoFree(BlsRtmpProtocol, m_protocol);

        m_protocol->setSession(this);

        if ((ret = service()) != E_SUCCESS) {
            log_error("BlsRtmpPublisher publish error, ret=%d", ret);
        }

        if (!RequestStop) mSleep(3);
    }

    return ret;
}

void BlsRtmpPublisher::setUrl(const MString &url)
{
    m_url = url;
}

void BlsRtmpPublisher::setHost(const MString &host, muint16 port)
{
    m_host = host;
    m_port = port;
}

int BlsRtmpPublisher::onCommand(BlsRtmpMessage *msg, const MString &name, double transactionID, BlsAMF0Any *arg1, BlsAMF0Any *arg2, BlsAMF0Any *arg3, BlsAMF0Any *arg4)
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

            log_trace("BlsRtmpPublisher connect App result, level: %s code: %s", level.c_str(), code.c_str());

            if (code != NetConnection_Connect_Success) {
                return E_CONNECTION_FAILED;
            }

            // set ack size
            if ((ret = m_protocol->setAckSize(2500000)) != E_SUCCESS) {
                log_error("BlsRtmpPublisher set ack size failed. ret=%d", ret);
                return ret;
            }

            m_commandList[2] = RTMP_AMF0_COMMAND_CREATE_STREAM;
            // create stream
            if ((ret = m_protocol->createStream() != E_SUCCESS)) {
                log_error("BlsRtmpPublisher createStream failed. ret=%d", ret);
                return ret;
            }

        } else if (command == RTMP_AMF0_COMMAND_CREATE_STREAM) {
            m_commandList.erase(transactionID);

            // get stream id
            BlsAMF0Number *streamID = dynamic_cast<BlsAMF0Number *>(arg2);
            if (!streamID) {
                return E_AMF_TYPE_ERROR;
            }

            // set stream id to ctx
            m_protocol->getRtmpCtx()->streamID = streamID->var;

            log_trace("BlsRtmpPublisher Stream ID set to %lf", streamID->var);

            if (streamID->var <= 0) {
                log_trace("BlsRtmpPublisher Stream ID(%d) should > 0", (int)streamID->var);
                return E_STREAM_ID_TOO_SMALL;
            }

            // publish stream
            BlsRtmpUrl url(m_url);
            if ((ret = m_protocol->publishStream(0, url.stream())) != E_SUCCESS) {
                log_error("BlsRtmpPublisher send publish cmd error, ret=%d", ret);
                return ret;
            }
        }
    } else if (name == RTMP_AMF0_COMMAND_ON_STATUS) {
        BlsAMF0Object *obj = dynamic_cast<BlsAMF0Object *>(arg2);

        if (!obj) {
            log_error("BlsRtmpPublisher the four arg of onStatus Pkt must be amf object.");
            return E_AMF_TYPE_ERROR;
        }

        MString level = obj->value("level");
        MString code = obj->value("code");

        log_trace("BlsRtmpPublisher publish result, level: %s code: %s", level.c_str(), code.c_str());

        if (level != STATUS_LEVEL_STATUS || code != NetStream_Publish_Start) {
            return E_RTMP_PUBLISH_FAILED;
        }

        // start to publish
        return publish();
    }

    return ret;
}

int BlsRtmpPublisher::service()
{
    int ret = E_SUCCESS;
    if ((ret = m_protocol->handshakeWithServer(false)) != E_SUCCESS) {
        log_error("BlsRtmpPublisher handshake with server failed.");
        return ret;
    }

    if ((ret = connectApp()) != E_SUCCESS) {
        log_error("BlsRtmpPublisher connectApp failed.");
        return ret;
    }

    while (!RequestStop) {
        BlsRtmpMessage *msg = NULL;
        if ((ret = m_protocol->recv_message(&msg)) != E_SUCCESS) {
            return ret;
        }
        mMSleep(10);
    }

    return ret;
}

int BlsRtmpPublisher::connectApp()
{
    int ret = E_SUCCESS;

    MString commandName = RTMP_AMF0_COMMAND_CONNECT;
    double  transactionID = 1;
    BlsRtmpMessageHeader header(RTMP_MSG_AMF0CommandMessage, RTMP_CID_OverConnection);

    BlsRtmpUrl url(m_url);
    BlsAMF0Object *obj = new BlsAMF0Object;

    obj->setValue("app", new BlsAMF0ShortString("live"));
    obj->setValue("flashVer", new BlsAMF0ShortString("WIN 14,0,0,125"));
    obj->setValue("swfUrl", new BlsAMF0ShortString("http://www.cutv.com/demo/live_test.swf"));
    obj->setValue("tcUrl", new BlsAMF0ShortString(url.tcUrl()));
    obj->setValue("fpad", new BlsAMF0Boolean);
    obj->setValue("capabilities", new BlsAMF0Number(239));
    obj->setValue("audioCodecs", new BlsAMF0Number(3575));
    obj->setValue("videoCodecs", new BlsAMF0Number(251));
    obj->setValue("videoFunction", new BlsAMF0Number(1));
    obj->setValue("pageUrl", new BlsAMF0ShortString("http://www.cutv.com/demo/live_test.swf"));
    obj->setValue("objectEncoding", new BlsAMF0Number(0));

    if ((ret = m_protocol->sendAny(header, new BlsAMF0ShortString(commandName), new BlsAMF0Number(transactionID), obj)) != E_SUCCESS) {
        log_error("BlsRtmpPublisher connectApp failed.");
        return ret;
    }

    m_commandList[transactionID] = commandName;

    return ret;
}

MString BlsRtmpPublisher::findCommand(double id)
{
    if (m_commandList.contains(id)) {
        return m_commandList[id];
    }

    return "";
}

int BlsRtmpPublisher::publish()
{
    int ret = E_SUCCESS;

    BlsRtmpUrl url(m_url);
    BlsRtmpSource *source = BlsRtmpSource::findSource(url.url());
    BlsConsumer *pool = new BlsConsumer(url.url());
    source->addPool(pool);

    while (!RequestStop) {
        list<BlsRtmpMessage> msgs = pool->getMessage();

        list<BlsRtmpMessage>::iterator iter;
        for (iter = msgs.begin(); iter != msgs.end(); ++iter) {
            BlsRtmpMessage &msg = *iter;
            if ((ret = m_protocol->send_message(&msg)) != E_SUCCESS) {
                return ret;
            }
        }

        mMSleep(50);
    }

    return ret;
}
