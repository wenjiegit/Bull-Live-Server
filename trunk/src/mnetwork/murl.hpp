
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MURL_HPP
#define MURL_HPP

#include <MString>
#include <map>

// TODO : add url encoded supported.

/*!
    http protocol

    protocol :// hostname[:port] / path / [;parameters][?query]#fragment
*/

class MUrl
{
public:
    MUrl();
    MUrl(const MString &url);

    void setUrl(const MString &url);
    MString url();
    MString fullUrl();

    MString scheme();
    MString host();
    muint16 port();
    MString path();

    bool hasQuery();
    MString query();
    MString queryByKey(const MString &key);

    bool hasFragment();
    MString fragment();
    MString fragmentByKey(const MString &key);

    MString errorString();

    static bool stringIsIp(const MString &str);

public:
    int parse();
    int parseQuery();

private:
    MString m_url;

    MString m_scheme;
    MString m_host;
    muint16 m_port;
    MString m_path;
    MString m_query;
    MString m_fragment;
    MString m_errorString;
    map<MString, MString> m_queryField;
};

#endif // MURL_HPP
