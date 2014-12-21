#include "BlsConf.hpp"

#include "BlsUtils.hpp"

#include <MConf>
#include <MLoger>

static BlsConf *th = NULL;

BlsConf::BlsConf(const MString &confFile, MObject *parent)
    : MObject(parent)
{
    init(confFile);
}

BlsConf *BlsConf::instance(const MString &confName)
{
    if (!th) {
        th = new BlsConf(confName);
    }

    return th;
}

vector<BlsHostInfo> BlsConf::getRtmpListenInfo()
{
    return m_listenerInfo;
}

MString BlsConf::getGopType(const MString &vhost)
{
    if (m_vhosts.contains(vhost)) {
        BlsVhost &host = m_vhosts[vhost];
        return host.gopCache;
    }

    return "";
}

MString BlsConf::getMode(const MString &vhost)
{
    if (m_vhosts.contains(vhost)) {
        BlsVhost &host = m_vhosts[vhost];
        return host.mode;
    }

    return "";
}

vector<BlsHostInfo> BlsConf::getOriginInfo(const MString &vhost)
{
    if (m_vhosts.contains(vhost)) {
        BlsVhost &host = m_vhosts[vhost];
        return host.origins;
    }

    vector<BlsHostInfo> ret;
    return ret;
}

vector<BlsHostInfo> BlsConf::getBackSourceInfo()
{
    return m_backSourceAddr;
}

bool BlsConf::httpLiveFlvEnabled(const MString &vhost)
{
    if (m_vhosts.contains(vhost)) {
        BlsVhost &host = m_vhosts[vhost];
        return host.httpLiveFlvEnabled;
    }

    return false;
}

vector<BlsHostInfo> BlsConf::getHttpLiveFlvListenInfo()
{
    return m_httpLiveFlvHosts;
}

bool BlsConf::init(const MString &confName)
{
    MConf *cf = new MConf(confName);

    MEE *root = cf->root();

    m_rtmpInternalPort = 1961;

    // rtmp listen
    MStringList args = root->get("rtmp_listen")->arguments();
    for (MStringList::iterator iter = args.begin(); iter != args.end(); ++iter) {
        MString &io = *iter;
        MStringList ip_ports = io.split(":");
        BlsHostInfo info;
        info.addr =  ip_ports.at(0);
        info.port =  ip_ports.at(1).toInt();

        m_listenerInfo.push_back(info);
    }

    // http listen
    MEE *httpLiveFlv = root->get("http_live_flv_listen");
    if (httpLiveFlv) {
        MStringList args = httpLiveFlv->arguments();
        for (MStringList::iterator iter = args.begin(); iter != args.end(); ++iter) {
            MString &io = *iter;
            MStringList ip_ports = io.split(":");
            BlsHostInfo info;
            info.addr =  ip_ports.at(0);
            info.port =  ip_ports.at(1).toInt();

            m_httpLiveFlvHosts.push_back(info);
        }
    } else {
        log_warn("can not find the http info, so disable the http flv mode.");
    }

    // worker count
    MEE *rtmp_listen = root->get("worker_count");
    if (!rtmp_listen) {
        m_workerCount = Min_Worker_Count;
        log_warn("No worker_count feild in conf file, reset to default: %d", Min_Worker_Count);
    } else {
        m_workerCount = rtmp_listen->arg0().toInt();

        if (m_workerCount < Min_Worker_Count) {
            log_warn("worker count range [%d-%d], but actual is %d, reset to %d", Min_Worker_Count, Max_Worker_Count, m_workerCount, Min_Worker_Count);
            m_workerCount = Min_Worker_Count;
        } else if (m_workerCount > Max_Worker_Count) {
            log_warn("worker count range [%d-%d], but actual is %d, reset to %d", Min_Worker_Count, Max_Worker_Count, m_workerCount, Max_Worker_Count);
            m_workerCount = Max_Worker_Count;
        }
    }

    // back source addr
    MEE *back_source = root->get("back_source_ports");
    if (!back_source) {
        log_error("back source ports are mandatory.");
        mAssert(back_source);
    } else {
        MStringList args = back_source->arguments();
        for (MStringList::iterator iter = args.begin(); iter != args.end(); ++iter) {
            MString &port = *iter;
            BlsHostInfo info;
            info.addr =  "127.0.0.1";
            info.port =  (muint16)port.toInt();

            m_backSourceAddr.push_back(info);
        }
    }

    // all vhost
    vector<MEE*> vhosts = root->getVector("vhost");
    for (int i = 0; i < vhosts.size(); ++i) {
        MEE *ee = vhosts.at(i);

        BlsVhost vh;

        // gop cache
        MEE *gop = ee->get("gop_cache");
        if (gop) {
            vh.gopCache = gop->arg0();
        }

        // mode
        MEE *mode = ee->get("mode");
        if (mode) {
            vh.mode = mode->arg0();
        }

        // origin
        MEE *origin = ee->get("origin");
        if (origin) {
            MStringList args = origin->arguments();
            for (MStringList::iterator iter = args.begin(); iter != args.end(); ++iter) {
                MString &temp = *iter;
                MStringList tempList = temp.split(":");
                if (tempList.size() == 2) {
                    BlsHostInfo host;
                    host.addr = tempList.at(0);
                    host.port = tempList.at(1).toInt();

                    vh.origins.push_back(host);
                }
            }
        }

        if (vh.mode == Mode_Remote && vh.origins.empty()) {
            log_warn("when vhost mode is remote, origins must not be empty.");
        }

        // http live flv
        MEE *liveFlv = ee->get("http_flv_live");
        if (liveFlv) {
            MEE *enabled = liveFlv->get("enabled");
            if (enabled && enabled->arg0() == "on") {
                vh.httpLiveFlvEnabled = true;
            }
        }

        // other

        m_vhosts[ee->arg0()] = vh;
    }

    return true;
}

bool processIsWorker()
{
    return BlsConf::instance()->processRole() == Process_Role_Worker;
}

bool processIsBackSource()
{
    return BlsConf::instance()->processRole() == Process_Role_BackSource;
}

bool processIsMaster()
{
    return BlsConf::instance()->processRole() == Process_Role_Master;
}
