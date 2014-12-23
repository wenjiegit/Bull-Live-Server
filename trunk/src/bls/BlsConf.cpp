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

bool BlsConf::containsVhost(const MString &vhost)
{
    return m_vhosts.contains(vhost);
}

bool BlsConf::useDefaultVhost()
{
    return containsVhost(BLS_DEFAULT_VHOST);
}

int BlsConf::chunkSize()
{
    return m_chunkSize;
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

DvrInfo BlsConf::getDvrInfo(const MString &vhost)
{
    if (m_vhosts.contains(vhost)) {
        BlsVhost &host = m_vhosts[vhost];
        return host.dvrInfo;
    }

    // empty one
    return DvrInfo();
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
        m_workerCount = WORKER_COUNT_MIN;
        log_warn("No worker_count feild in conf file, reset to default: %d", WORKER_COUNT_MIN);
    } else {
        m_workerCount = rtmp_listen->arg0().toInt();

        if (m_workerCount < WORKER_COUNT_MIN) {
            log_warn("worker count range [%d-%d], but actual is %d, reset to %d", WORKER_COUNT_MIN, WORKER_COUNT_MAX, m_workerCount, WORKER_COUNT_MIN);
            m_workerCount = WORKER_COUNT_MIN;
        } else if (m_workerCount > WORKER_COUNT_MAX) {
            log_warn("worker count range [%d-%d], but actual is %d, reset to %d", WORKER_COUNT_MIN, WORKER_COUNT_MAX, m_workerCount, WORKER_COUNT_MAX);
            m_workerCount = WORKER_COUNT_MAX;
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

    // chunk size
    MEE *chunk_size = root->get("chunk_size");
    if (!chunk_size) {
        // set default
        m_chunkSize = DEFAULT_CHUNK_SIZE;
        log_warn("No chunk_size feild in conf file, reset to default: %d", DEFAULT_CHUNK_SIZE);
    } else {
        int chunkSize = chunk_size->arg0().toInt();
        if (chunkSize < CHUNK_SIZE_MIN) {
            log_warn("chunk size range [%d-%d], but actual is %d, reset to %d", CHUNK_SIZE_MIN, CHUNK_SIZE_MAX, chunkSize, DEFAULT_CHUNK_SIZE);
            m_chunkSize = DEFAULT_CHUNK_SIZE;
        } else if (chunkSize > CHUNK_SIZE_MAX) {
            log_warn("chunk size range [%d-%d], but actual is %d, reset to %d", CHUNK_SIZE_MIN, CHUNK_SIZE_MAX, chunkSize, DEFAULT_CHUNK_SIZE);
            m_chunkSize = DEFAULT_CHUNK_SIZE;
        }
    }

    // all vhost
    vector<MEE*> vhosts = root->getVector("vhost");
    for (unsigned int i = 0; i < vhosts.size(); ++i) {
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

        MEE *dvr = ee->get("dvr");
        if (dvr) {
            MEE *pathPtr = dvr->get("path");
            mAssert(pathPtr);
            MString path = pathPtr->arg0();

            vh.dvrInfo.enabled = true;
            vh.dvrInfo.path = path;
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
