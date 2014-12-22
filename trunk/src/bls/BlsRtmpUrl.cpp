
/*
 * Copyright (C) wenjie.zhao
 */

#include "BlsRtmpUrl.hpp"

#include "BlsUtils.hpp"

#include <MStringList>

BlsRtmpUrl::BlsRtmpUrl()
{
}

BlsRtmpUrl::BlsRtmpUrl(const MString &url)
    : MUrl(url)
{
    parse();
}

void BlsRtmpUrl::setRtmpUrl(const MString &url)
{
    setUrl(url);
    parse();
}

MString BlsRtmpUrl::url()
{
    // url = scheme + path + query
    MString ret = MString().sprintf("%s:%d%s", host().c_str(), port(), path().c_str());
    ret.replace("//", "/");
    ret = scheme() + "://" + ret;

    return ret;
}

void BlsRtmpUrl::parse()
{
    MStringList as = path().split("/");
    if (as.size() >=2) {
        m_stream = as.back();
        as.pop_back();
        m_app = as.join("/");
    } else if (as.size() == 1) {
        m_stream = as.front();
        m_app = "/";
    }

    if (!stringIsIp(host())) {
        m_vhost = host();
    } else {
        MString vhost = queryByKey("vhost");
        m_vhost = vhost.empty() ? BLS_DEFAULT_VHOST : vhost;
    }

    m_tcUrl = scheme() + "://" + vhost() + ":" + MString::number(port()) + "/" + m_app;
}
