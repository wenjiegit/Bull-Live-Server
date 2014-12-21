#ifndef BLSSERVERSELECTOR_HPP
#define BLSSERVERSELECTOR_HPP

#include <MString>
#include <list>

#include "conhash.h"

class BlsServerSelector
{
public:
    BlsServerSelector();
    ~BlsServerSelector();

    static BlsServerSelector *instance();

    void addServer(const MString &host, muint16 port);
    void removeServer(const MString &host, muint16 port);
    muint16 lookUp(const MString &info);

private:
    void init();

private:
    struct conhash_s *m_conhash;
    list<struct node_s*> m_nodes;
};

#endif // BLSSERVERSELECTOR_HPP
