
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MRTMPURL_H
#define MRTMPURL_H

#include <MUrl>

class MRtmpUrl : public MUrl
{
public:
    MRtmpUrl();
    MRtmpUrl(const MString &url);

    inline MString app() { return m_app; }
    inline MString vhost() { return m_vhost; }
    inline MString stream() { return m_stream; }

    void setRtmpUrl(const MString &url);

private:
    void parse();

private:
    MString m_app;
    MString m_vhost;
    MString m_stream;
};

#endif // MRTMPURL_H
