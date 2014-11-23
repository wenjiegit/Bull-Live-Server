#ifndef BLSCONF_HPP
#define BLSCONF_HPP

#include <MObject>
#include <vector>
#include <MHash>

using namespace std;

#define Gop_No_Gop          "no_gop"
#define Gop_Fast_Gop        "fast_gop"
#define Gop_Normal_Gop      "normal_gop"

#define Mode_Remote         "remote"
#define Mode_Local          "local"

#define Min_Worker_Count    (1)
#define Max_Worker_Count    (32)

struct BlsHostInfo
{
    MString addr;
    mint16 port;
};

struct BlsVhost
{
    BlsVhost()
    {
        gopCache = Gop_Fast_Gop;
        httpLiveFlvEnabled = false;
    }

    bool httpLiveFlvEnabled;
    MString gopCache;
    MString mode;
    vector<BlsHostInfo> origins;
};

class BlsConf : public MObject
{
public:
    int m_rtmpInternalPort;
    int m_processRole;

public:
    BlsConf(const MString &confFile, MObject *parent = 0);

    static BlsConf *instance(const MString &confName = "");

    vector<BlsHostInfo> getRtmpListenInfo();
    MString getGopType(const MString &vhost);
    MString getMode(const MString &vhost);
    vector<BlsHostInfo> getOriginInfo(const MString &vhost);

    inline const int getWorkerCount() const { return  m_workerCount;}
    vector<BlsHostInfo> getBackSourceInfo();

    // http flv live enabledl
    bool httpLiveFlvEnabled(const MString &vhost);

    vector<BlsHostInfo> getHttpLiveFlvListenInfo();

private:
    bool init(const MString &confName);

private:
    MHash<MString, BlsVhost> m_vhosts;
    vector<BlsHostInfo> m_listenerInfo;
    int m_workerCount;
    vector<BlsHostInfo> m_backSourceAddr;

    vector<BlsHostInfo> m_httpLiveFlvHosts;
};

#endif // BLSCONF_HPP
