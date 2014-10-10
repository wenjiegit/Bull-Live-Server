
/*
 * Copyright (C) wenjie.zhao
 */


#include "mrtmpurl.hpp"

#include <MStringList>

MRtmpUrl::MRtmpUrl()
{
}

MRtmpUrl::MRtmpUrl(const MString &url)
    : MUrl(url)
{
    parse();
}

void MRtmpUrl::setRtmpUrl(const MString &url)
{
    setUrl(url);
    parse();
}

MString MRtmpUrl::url()
{
    // url = scheme + path + query
    MString ret = MString().sprintf("%s:%d%s", host().c_str(), port(), path().c_str());
    ret.replace("//", "/");
    ret = scheme() + "://" + ret;

    return ret;
}

void MRtmpUrl::parse()
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
        m_vhost = vhost.empty() ? "default" : vhost;
    }

    m_tcUrl = scheme() + "://" + vhost() + ":" + MString::number(port()) + "/" + m_app;
}
