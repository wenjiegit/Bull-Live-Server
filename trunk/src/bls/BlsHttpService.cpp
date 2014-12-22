#include "BlsHttpService.hpp"

#include <MLoger>
#include <MUrl>
#include <MHttpParser>
#include <MHttpResponseHeader>
#include <MStream>

#include "BlsRtmpSource.hpp"
#include "BlsConsumer.hpp"
#include "BlsRtmpProtocol.hpp"

#define HTTP_LIVE_FLV_SUFFIX            ".flv"
#define HTTP_DYNAMIC_STEAMING_SUFFIX    ".f4m"
#define HTTP_LIVE_STEAMING_SUFFIX       ".m3u8"

static char flv_header[] = {'F', 'L', 'V',
                            0x01, 0x05, 0x00, 0x00, 0x00, 0x09,
                            0x00, 0x00, 0x00, 0x00};
extern MStream serialFlv(BlsRtmpMessage *msg);

BlsHttpService::BlsHttpService(MObject *parent)
    : MTcpServer(parent)
{
}

BlsHttpService::~BlsHttpService()
{

}

int BlsHttpService::newConnection(MTcpSocket *socket)
{
    BlsHttpClient *c = new BlsHttpClient(socket);
    return c->start();
}

BlsHttpClient::BlsHttpClient(MTcpSocket *socket, MObject *parent)
    : MHttpReactor(socket, parent)
    , m_socket(socket)
{
    m_socket->setParent(this);
}

BlsHttpClient::~BlsHttpClient()
{

}

int BlsHttpClient::run()
{
    int ret = E_SUCCESS;
    MString body;

    if ((ret = readHttpHeader(*m_parser, body, *m_socket)) != E_SUCCESS) {
        log_error("BlsHttpClient read Http Header failed, ret=%d", ret);
        return ret;
    }

    // get http host
    MString host = m_parser->host();
    if (host.empty()) {
        log_error("BlsHttpClient Http Header error");\
        return -1;
    }

    MString vhost;
    MString tempStr = "HTTP://" + host + "" + m_parser->url();
    MUrl tempUrl(tempStr);

    if (MUrl::stringIsIp(host)) {
        vhost = tempUrl.queryByKey("vhost");
    }

    if (vhost.empty()) {
        vhost = tempUrl.host();
    }

    if (tempUrl.path().endWith(HTTP_LIVE_FLV_SUFFIX)) {
        MString rtmpStr = "rtmp://" + vhost + ":1935" + tempUrl.path();
        int suffixLen = sizeof(HTTP_LIVE_FLV_SUFFIX);
        rtmpStr = rtmpStr.substr(0, rtmpStr.size() - suffixLen + 1);

        int ret = sendHttpLiveFlv(rtmpStr);
        if (ret != E_SUCCESS) {
            log_trace("Http Live Flv finished.");
        }
    }

    else {

    }

end:
    clean();

    return ret;
}

int BlsHttpClient::sendHttpLiveFlv(const MString &url)
{
    // response http header
    MHttpResponseHeader header;
    header.setStatusLine(200);
    header.addValue("Connection", "close");
    header.addValue("Accept-Ranges", "bytes");
    header.addValue("Cache-Control", "no-store");
    header.setContentType(MHttpHeader::contentType("flv"));

    if (response(header, flv_header, 13) != E_SUCCESS) {
        log_error("response http failed.");
        return -1;
    }

    BlsRtmpSource *source = BlsRtmpSource::findSource(url);
    BlsConsumer *pool = new BlsConsumer(url);
    source->addPool(pool);
    mAutoFree(BlsConsumer, pool);

    while (true) {
        list<BlsRtmpMessage> msgs = pool->getMessage();
        list<BlsRtmpMessage>::iterator iter;
        for (iter = msgs.begin(); iter != msgs.end(); ++iter) {
            BlsRtmpMessage &msg = *iter;

            // to FLV message
            MStream stream = serialFlv(&msg);
            if (m_socket->write(stream) != stream.size()) {
                log_error("send flv tag failed");
                return -2;
            }
        }

        mMSleep(5);
    }

    return E_SUCCESS;
}

void BlsHttpClient::clean()
{
    m_socket->close();
}
