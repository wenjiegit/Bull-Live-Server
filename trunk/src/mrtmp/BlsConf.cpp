#include "BlsConf.hpp"

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

vector<BlsHostInfo> BlsConf::getListenInfo()
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

bool BlsConf::init(const MString &confName)
{
    MConf *cf = new MConf(confName);

    MEE *root = cf->root();

    m_rtmpInternalPort = 1961;

    // listen
    MStringList args = root->get("rtmp_listen")->arguments();
    for (MStringList::iterator iter = args.begin(); iter != args.end(); ++iter) {
        MString &io = *iter;
        MStringList ip_ports = io.split(":");
        BlsHostInfo info;
        info.addr =  ip_ports.at(0);
        info.port =  ip_ports.at(1).toInt();
        m_listenerInfo.push_back(info);
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

        // other

        m_vhosts[ee->arg0()] = vh;
    }

    return true;
}
