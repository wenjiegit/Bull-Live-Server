
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MRTMPURL_H
#define MRTMPURL_H

#include <MUrl>

#include "BlsConf.hpp"

class BlsRtmpUrl : public MUrl
{
public:
    BlsRtmpUrl();
    BlsRtmpUrl(const MString &url);

    inline MString app() { return m_app; }
    inline MString vhost() { return m_vhost; }
    inline MString stream() { return m_stream; }
    inline MString tcUrl() { return m_tcUrl; }

    void setRtmpUrl(const MString &url);
    MString url();

private:
    void parse();

private:
    MString m_app;
    MString m_vhost;
    MString m_stream;
    MString m_tcUrl;
};

#endif // MRTMPURL_H
