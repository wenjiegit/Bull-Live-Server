
/*
 * Copyright (C) wenjie.zhao
 */


#include "mrtmp.hpp"

#include "mrtmpprotocol.hpp"

#define MAP_FUNCTION(x) \
    m_methods[#x] = (&MRtmp::x)

MRtmp::MRtmp(MTcpSocket *socket, MObject *parent)
    : MObject(parent)
    , m_protocol(new MRtmpProtocol(socket, this))
{
    MAP_FUNCTION(connectApp);
}

int MRtmp::invoke(const MString &method, const MAMFVariant &v1, const MAMFVariant &v2, const MAMFVariant &v3, const MAMFVariant &v4, const MAMFVariant &v5, const MAMFVariant &v6)
{
    if (m_methods.contains(method)) {
        RtmpFunction func = m_methods[method];
        return (this->*func)(v1, v2, v3, v4, v5, v6);
    } else {
        return defaultMethod(v1, v2, v3, v4, v5, v6);
    }
}

int MRtmp::defaultMethod(const MAMFVariant &v1, const MAMFVariant &v2, const MAMFVariant &v3, const MAMFVariant &v4, const MAMFVariant &v5, const MAMFVariant &v6)
{
}

int MRtmp::connectApp(const MAMFVariant &v1, const MAMFVariant &v2, const MAMFVariant &v3, const MAMFVariant &v4, const MAMFVariant &v5, const MAMFVariant &v6)
{
    return 0;
}

