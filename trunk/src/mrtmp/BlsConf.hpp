#ifndef BLSCONF_HPP
#define BLSCONF_HPP

#include <MObject>
#include <vector>
#include <MHash>

using namespace std;

struct BlsHostInfo
{
    MString addr;
    mint16 port;
};

#define Gop_No_Gop          "no_gop"
#define Gop_Fast_Gop        "fast_gop"
#define Gop_Normal_Gop      "normal_gop"

struct BlsVhost
{
    BlsVhost()
    {
        gopCache = Gop_Fast_Gop;
    }

    MString gopCache;
    MString mode;
    vector<BlsHostInfo> origins;
};

#define Mode_Remote "remote"
#define Mode_Local  "local"

class BlsConf : public MObject
{
public:
    int m_rtmpInternalPort;
    int m_processRole;

public:
    BlsConf(const MString &confFile, MObject *parent = 0);

    static BlsConf *instance(const MString &confName = "");

    vector<BlsHostInfo> getListenInfo();
    MString getGopType(const MString &vhost);
    MString getMode(const MString &vhost);
    vector<BlsHostInfo> getOriginInfo(const MString &vhost);

private:
    bool init(const MString &confName);

private:
    MHash<MString, BlsVhost> m_vhosts;
    vector<BlsHostInfo> m_listenerInfo;
};

#endif // BLSCONF_HPP
