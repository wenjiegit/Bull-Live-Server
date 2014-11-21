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

int BlsChildChannel::start()
{
    mSleep(1);
    int ret = E_SUCCESS;
    m_socket = new MTcpSocket(this);
    if ((ret = m_socket->initSocket()) != E_SUCCESS) {
        return ret;
    }

    if ((ret = m_socket->connectToHost("127.0.0.1", 1940)) != E_SUCCESS) {
        return ret;
    }

    return MThread::start();
}

int BlsChildChannel::run()
{
    if (sendLine(Internal_CMD_PID, MString::number(getpid())) != E_SUCCESS) {
        log_error("send to server failed.");
        clean();
    }

    int port = BlsConf::instance()->m_rtmpInternalPort;
    if (sendLine(Internal_CMD_InternalPort, MString::number(port)) != E_SUCCESS) {
        log_error("send to server failed.");
        clean();
    }
    log_info("child %d run success.", (int)getpid());

    while (!RequestStop) {
        int ret = m_socket->testFeature(MTcpSocket::ReadReady, ST_UTIME_NO_TIMEOUT);
        if ((ret = m_socket->readToLineCache()) != E_SUCCESS) {
            clean();
        }

        mMSleep(500);
    }

    return E_SUCCESS;
}

int BlsChildChannel::sendLineAndWaitResponse(const MString &commad, const MString &data, MString &response)
{
    if (sendLine(commad, data) != E_SUCCESS) {
        clean();
        return -1;
    }
    mSleep(1);

    if (m_socket->readLine(response) != E_SUCCESS) {
        clean();
        return E_SUCCESS;
    }
}

int BlsChildChannel::sendLine(const MString &commad, const MString &data)
{
    MString line = commad + Internal_CMD_Delimer + data + "\n";

    return send(line);
}

int BlsChildChannel::send(const MString &data)
{
    if ((m_socket->write(data)) != data.size()) {
        log_error("write to master failed");
        return -1;
    }

    return E_SUCCESS;
}

void BlsChildChannel::timerA()
{
    // TODO write connection info to file
    // eg. bitrate fps ...
    static int pid = (int)getpid();
    MString name = MString().sprintf("PID_%d_Listen_%d.info", pid, BlsConf::instance()->m_rtmpInternalPort);
    MFile file(name);
    if (file.open("w")) {
        file.write("this is a text");
        file.close();
    }
}

int BlsChildChannel::clean()
{
    _exit(0);
}

int BlsChildChannel::processLine(const MString &line)
{

}
