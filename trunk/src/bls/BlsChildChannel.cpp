#include "BlsChildChannel.hpp"

#include <unistd.h>
#include <MTcpSocket>
#include <MLoger>
#include <MCoreApplication>
#include <MFile>
#include <MTimer>

#include "BlsUtils.hpp"
#include "BlsConf.hpp"

BlsChildChannel *g_cchannel = NULL;

BlsChildChannel::BlsChildChannel(MObject *parent)
    : MThread(parent)
{
    g_cchannel = this;

    m_timer = new MTimer(TIMER_EVENT(timerA), this);
    m_timer->setInterval(1000 * 10);
    m_timer->start();
}

BlsChildChannel::~BlsChildChannel()
{
    m_timer->stop();
}

int BlsChildChannel::init()
{
    // wait for master process running.
    mSleep(3);

    int ret = E_SUCCESS;

    m_socket = new MTcpSocket(this);
    if ((ret = m_socket->initSocket()) != E_SUCCESS) {
        return ret;
    }

    if ((ret = m_socket->connectToHost("127.0.0.1", 1940)) != E_SUCCESS) {
        return ret;
    }
    log_trace("connected to master success.");

    return MThread::start();
}

int BlsChildChannel::run()
{
//    while (!RequestStop) {
//        int ret = m_socket->testFeature(MTcpSocket::ReadReady, ST_UTIME_NO_TIMEOUT);
//        if ((ret = m_socket->readToLineCache()) != E_SUCCESS) {
//            clean();
//        }

//        mMSleep(500);
//    }

    return E_SUCCESS;
}

void BlsChildChannel::timerA()
{
    // TODO write connection info to file
    // eg. bitrate fps ...
//    static int pid = (int)getpid();
//    MString name = MString().sprintf("PID_%d_Listen_%d.info", pid, BlsConf::instance()->m_rtmpInternalPort);
//    MFile file(name);
//    if (file.open("w")) {
//        file.write("this is a text");
//        file.close();
    //    }
}

int BlsChildChannel::checkSameStream(const MString &url, bool &res)
{
    int ret = E_SUCCESS;

    // init to false.
    res = false;

    BlsInternalMsg msg;
    msg.setHeader(MSG_IF_EXIST_SAME_STREAM);
    msg.setBody(url);

    if ((ret = writeInternalMsg(msg, m_socket)) != E_SUCCESS) {
        log_error("check if exist stream write error, process will exit.");
        clean();
        return ret;
    }

    BlsInternalMsg response;
    if ((ret = readInternalMsg(response, m_socket)) != E_SUCCESS) {
        log_error("check if exist stream read error, process will exit.");
        clean();
        return ret;
    }

    if (response.header() == MSG_RESULT && response.body() == MSG_RESULT_SUCESS) {
        res = true;
    }

    return ret;
}

int BlsChildChannel::informStreamPublished(const MString &url)
{
    BlsInternalMsg msg;
    msg.setHeader(MSG_STREAM_PUBLISHED);
    msg.setBody(url);

    return writeInternalMsg(msg, m_socket);
}

int BlsChildChannel::informStreamUnPublished(const MString &url)
{
    BlsInternalMsg msg;
    msg.setHeader(MSG_STREAM_UNPUBLISHED);
    msg.setBody(url);

    return writeInternalMsg(msg, m_socket);
}

int BlsChildChannel::clean()
{
    log_error("process will exit, may be master process is freezed.");
    _exit(0);
}
