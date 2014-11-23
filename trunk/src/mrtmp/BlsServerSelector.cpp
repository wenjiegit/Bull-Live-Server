#include "BlsServerSelector.hpp"

#include <MStringList>

#include "BlsConf.hpp"

BlsServerSelector::BlsServerSelector()
{
    m_conhash = conhash_init(NULL);
}

BlsServerSelector::~BlsServerSelector()
{
    conhash_fini(m_conhash);
}

BlsServerSelector *BlsServerSelector::instance()
{
    static BlsServerSelector *ss = NULL;
    if (!ss) {
        ss = new BlsServerSelector;
        ss->init();
    }

    return ss;
}

void BlsServerSelector::addServer(const MString &host, muint16 port)
{
    node_s *node = new node_s;

    // ip:port
    MString info = host;
    info.append(":");
    info.append(MString::number(port));

    conhash_set_node(node, info.c_str(), 4800);
    conhash_add_node(m_conhash, node);

    m_nodes.push_back(node);
}

void BlsServerSelector::removeServer(const MString &host, muint16 port)
{

}

muint16 BlsServerSelector::lookUp(const MString &info)
{
    const node_s *node = conhash_lookup(m_conhash, info.c_str());
    if(node) {
        MStringList ip_port = MString(node->iden).split(":");
        return (muint16)ip_port.at(1).toInt();
    }

    return -1;
}

void BlsServerSelector::init()
{
    vector<BlsHostInfo> back_source_infos = BlsConf::instance()->getBackSourceInfo();
    for (unsigned int i = 0; i < back_source_infos.size(); ++i) {
        BlsHostInfo &info = back_source_infos.at(i);
        addServer(info.addr, info.port);
    }
}
