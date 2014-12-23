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

#define WORKER_COUNT_MIN    (1)
#define WORKER_COUNT_MAX    (32)

// default chunk-size
#define DEFAULT_CHUNK_SIZE      (40960)
#define CHUNK_SIZE_MAX          (65535)
#define CHUNK_SIZE_MIN          (128)

struct BlsHostInfo
{
    MString addr;
    mint16 port;
};

struct DvrInfo
{
    DvrInfo()
    {
        enabled = false;
    }

    bool enabled;
    MString path;
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
    DvrInfo dvrInfo;
};

class BlsConf : public MObject
{
public:
    int m_rtmpInternalPort;
    int m_processRole;

public:
    BlsConf(const MString &confFile, MObject *parent = 0);

    static BlsConf *instance(const MString &confName = "");

    inline void setProcessRole(int role) { m_processRole = role; }
    inline int processRole() const { return m_processRole; }

    /*!
        check whether contains this @vhost in conf.
    */
    bool containsVhost(const MString &vhost);

    /*!
        check whether exist default vhost in conf
    */
    bool useDefaultVhost();

    /*!
        get chunk size
    */
    int chunkSize();

    vector<BlsHostInfo> getRtmpListenInfo();
    MString getGopType(const MString &vhost);
    MString getMode(const MString &vhost);
    vector<BlsHostInfo> getOriginInfo(const MString &vhost);

    inline const int getWorkerCount() const { return  m_workerCount;}
    vector<BlsHostInfo> getBackSourceInfo();

    // http flv live enabledl
    bool httpLiveFlvEnabled(const MString &vhost);

    vector<BlsHostInfo> getHttpLiveFlvListenInfo();

    /*!
        get @vhost dvr info
        @ret dvr info struct.
    */
    DvrInfo getDvrInfo(const MString &vhost);

private:
    bool init(const MString &confName);

private:
    MHash<MString, BlsVhost> m_vhosts;
    vector<BlsHostInfo> m_listenerInfo;
    vector<BlsHostInfo> m_backSourceAddr;
    vector<BlsHostInfo> m_httpLiveFlvHosts;

    int m_workerCount;
    int m_chunkSize;
};

extern "C" {
bool processIsWorker();
bool processIsBackSource();
bool processIsMaster();
}

#endif // BLSCONF_HPP
