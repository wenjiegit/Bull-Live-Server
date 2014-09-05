
/*
 * Copyright (C) wenjie.zhao
 * Copyright (C) www.17173.com
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

    MString vhost = queryByKey("vhost");
    m_vhost = vhost.empty() ? "__defaultVhost__" : vhost;
}
