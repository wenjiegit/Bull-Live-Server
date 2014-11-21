#include "BlsMasterChannel.hpp"

#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <vector>

#include <MHash>
#include <MLoger>
#include <MStringList>

#include "BlsUtils.hpp"
#include "mrtmpurl.hpp"


static MHash<int, BlsChild *> gs_childs;
static MHash<MString, BlsChild *> gs_sources;

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
    if (gs_childs.contains(m_pid)) {
        gs_childs.erase(m_pid);
    }

    if (m_pid > 0) {
        kill(m_pid, SIGKILL);
        ::wait(NULL);
    }

    // clean from gs_sources
    MHash<MString, BlsChild *>::iterator iter;
    std::vector<MString> deleteKeys;
    for (iter = gs_sources.begin(); iter != gs_sources.end(); ++iter) {
        if (iter->second == this) deleteKeys.push_back(iter->first);
    }

    for (int i = 0; i < deleteKeys.size(); ++i) {
        gs_sources.erase(deleteKeys.at(i));
    }
}

int BlsChild::run()
{
    while (!RequestStop) {
        MString line;
        if (m_socket->readLine(line) != E_SUCCESS) {
            // TODO child exit ? or crash ?
            // should kill child ?
            break;
        }

        if (processCommand(line) != E_SUCCESS) {
            // TODO child exit ? or crash ?
            // should kill child ?
            break;
        }

        mMSleep(100);
    }

    log_trace("BlsChild exit.");
    this->deleteLater();

    return E_SUCCESS;
}

int BlsChild::processCommand(MString &line)
{
    if (line.empty()) return E_SUCCESS;

    if (line.endWith("\n")) {
        line.erase(line.size()-1, 1);
    }
    log_trace("BlsMasterChannel get line %s", line.c_str());

    MStringList temp = line.split(Internal_CMD_Delimer);
    MString key = temp.at(0);
    MString value = temp.at(1);

    if (key == Internal_CMD_PID) {
        m_pid = value.toInt();
        gs_childs[m_pid] = this;
    } else if (key == Internal_CMD_InternalPort) {
        m_internalPort = value.toInt();
    } else if (key == Internal_CMD_WhoHasBackSource) {
        MRtmpUrl rtmpUrl(value);
        MString url = rtmpUrl.url();

        int port = 0;
        if (gs_sources.contains(url)) {
            port = gs_sources[url]->internalPort();
        }

        if (sendLine(Internal_CMD_WhoHasBackSourceRes, MString::number(port)) != E_SUCCESS) {
            return -1;
        }

        log_trace("--%s:%d", url.c_str(), port);

        if (port == 0) {
            gs_sources[url] = this;
        }
    } else if (key == Internal_CMD_IHasBackSourced) {
        log_warn("%s insert into backsource queue", value.c_str());
        gs_sources[value] = this;
    } else if (key == Internal_CMD_RemoveHasBackSourceRes) {
        log_warn("%s removed from backsource queue", value.c_str());
        gs_sources.erase(value);
    }

    return E_SUCCESS;
}

int BlsChild::sendLine(const MString &commad, const MString &data)
{
    MString line = commad + Internal_CMD_Delimer + data + "\n";

    return send(line);
}

int BlsChild::send(const MString &data)
{
    if ((m_socket->write(data)) != data.size()) {
        log_error("write to master failed");
        return -1;
    }

    return E_SUCCESS;
}
