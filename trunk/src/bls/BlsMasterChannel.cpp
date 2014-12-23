#include "BlsMasterChannel.hpp"

#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <vector>

#include <MHash>
#include <MLoger>
#include <MStringList>

#include "BlsUtils.hpp"
#include "BlsRtmpUrl.hpp"

static MHash<int, BlsChild *> gs_childs;
static MHash<MString, BlsChild *> gs_sources;

class BlsStreamManager : public MObject
{
public:
    BlsStreamManager();
    ~BlsStreamManager();

    static BlsStreamManager *instance();

    void addStream(const MString &url);
    bool contains(const MString &info) const;
    void removeStream(const MString &url);

private:
    MHash<MString, bool> m_pushedStreams;
};

BlsStreamManager::BlsStreamManager()
{

}

BlsStreamManager::~BlsStreamManager()
{

}

BlsStreamManager *BlsStreamManager::instance()
{
    static BlsStreamManager *ret = NULL;
    if (!ret) {
        ret = new BlsStreamManager;
    }

    return ret;
}

void BlsStreamManager::addStream(const MString &url)
{
    m_pushedStreams[url] = true;
}

bool BlsStreamManager::contains(const MString &info) const
{
    return m_pushedStreams.contains(info);
}

void BlsStreamManager::removeStream(const MString &url)
{
    m_pushedStreams.erase(url);
}

BlsMasterChannel::BlsMasterChannel(MObject *parent)
    : MTcpServer(parent)
{
}

BlsMasterChannel::~BlsMasterChannel()
{

}

int BlsMasterChannel::newConnection(MTcpSocket *socket)
{
    BlsChild *child = new BlsChild(socket);
    socket->setParent(child);

    return child->start();
}

BlsChild::BlsChild(MTcpSocket *socket, MObject *parent)
    : MThread(parent)
    , m_socket(socket)
    , m_pid(-1)
    , m_internalPort(-1)
{
}

BlsChild::~BlsChild()
{

}

int BlsChild::run()
{
    int ret = E_SUCCESS;

    while (!RequestStop) {
        BlsInternalMsg msg;
        if ((ret = readInternalMsg(msg, m_socket)) != E_SUCCESS) {
            log_error("read msg from master channel error");
            break;
        }

        if ((ret = processMsg(msg)) != E_SUCCESS) {
            log_error("process msg error");
            break;
        }

        mMSleep(100);
    }

    log_trace("BlsChild exit.");

    this->deleteLater();

    return E_SUCCESS;
}

int BlsChild::processMsg(const BlsInternalMsg &msg)
{
    int ret = E_SUCCESS;

    MString header = msg.header();
    BlsInternalMsg response;

    msg.dump();

    if (header == MSG_IF_EXIST_SAME_STREAM) {
        MString url = msg.body();

        if (BlsStreamManager::instance()->contains(url)) {
            response.setBody(MSG_RESULT_SUCESS);
        } else {
            response.setBody(MSG_RESULT_FAILED);
        }
        response.setHeader(MSG_RESULT);

        response.dump();

        return writeInternalMsg(response, m_socket);
    } else if (header == MSG_STREAM_PUBLISHED){
        MString url = msg.body();

        BlsStreamManager::instance()->addStream(url);
    } else if (header == MSG_STREAM_UNPUBLISHED) {
        MString url = msg.body();
        BlsStreamManager::instance()->removeStream(url);
    }

    return ret;
}
