
/*
 * Copyright (C) wenjie.zhao
 */


#include "murl.hpp"

#include <MStringList>
#include <http_parser.h>

#define HTTP_DEFAULT_PORT   80
#define FTP_DEFAULT_PORT    20
#define RTMP_DEFAULT_PORT   1935

static MString getUrlField(const MString &url, enum http_parser_url_fields field, const http_parser_url &hpu)
{
    int offset  = hpu.field_data[field].off;
    int len     = hpu.field_data[field].len;

    if ( (offset + len) > url.size()) {
        return "";
    }

    return url.substr(offset, len);
}

MUrl::MUrl()
{
}

MUrl::MUrl(const MString &url)
    : m_url(url)
{
    parse();
}

void MUrl::setUrl(const MString &url)
{
    m_url = url;

    if (!url.isEmpty()) {
        parse();
    }
}

MString MUrl::url()
{
    // url = scheme + path + query
    MString ret = MString().sprintf("%s:%d%s", host().c_str(), port(), path().c_str());
    ret.replace("//", "/");
    ret = scheme() + "://" + ret;

    return ret;
}

MString MUrl::fullUrl()
{
    return m_url;
}

MString MUrl::scheme()
{
    return m_scheme;
}

MString MUrl::host()
{
    return m_host;
}

muint16 MUrl::port()
{
    if (m_port == 0) {
        if (m_scheme == "http")
        {
            m_port = HTTP_DEFAULT_PORT;
        } else if (m_scheme == "rtmp") {
            m_port = RTMP_DEFAULT_PORT;
        } else if (m_scheme == "ftp") {
            m_port = FTP_DEFAULT_PORT;
        }
    }

    return m_port;
}

MString MUrl::path()
{
    return m_path;
}

bool MUrl::hasQuery()
{
    return !m_query.isEmpty();
}

MString MUrl::query()
{
    return m_query;
}

MString MUrl::queryByKey(const MString &key)
{
    // TODO :  imp
    // what is query ?
    // eg : http://zhidao.baidu.com/link?url=test.com&url1=test1&....
    // and the "url=test.com" is query field.
    // split the string with & =
    MString value;
    if (m_queryField.find(key) != m_queryField.end()) {
        value = m_queryField[key];
    }

    return value;
}

bool MUrl::hasFragment()
{
    return !m_fragment.isEmpty();
}

MString MUrl::fragment()
{
    return m_fragment;
}

MString MUrl::fragmentByKey(const MString &key)
{
    if (m_queryField.find(key) != m_queryField.end()) {
        return m_queryField[key];
    }

    return "";
}

MString MUrl::errorString()
{
    return m_errorString;
}

bool MUrl::stringIsIp(const MString &str)
{
    int a,b,c,d;

    if (str.empty()) return false;

    int ret = sscanf(str.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d);
    if (ret == 4
            && (a>=0 && a<=255)
            && (b>=0 && b<=255)
            && (c>=0 && c<=255)
            && (d>=0 && d<=255))
    {
        return true;
    }

    return false;
}

int MUrl::parse()
{
    http_parser_url hpu;
    if (http_parser_parse_url(m_url.c_str(), m_url.length(), 0, &hpu) != 0) {
        return -1;
    }

    m_scheme    = getUrlField(m_url, UF_SCHEMA, hpu);
    m_host      = getUrlField(m_url, UF_HOST, hpu);
    m_port      = getUrlField(m_url, UF_PORT, hpu).toShort();
    m_path      = getUrlField(m_url, UF_PATH, hpu);
    m_query     = getUrlField(m_url, UF_QUERY, hpu);
    m_fragment  = getUrlField(m_url, UF_FRAGMENT, hpu);

    if (hasQuery()) {
        parseQuery();
    }

    return 0;
}

int MUrl::parseQuery()
{
    MString queryStr = m_query;
    queryStr.replace("?", "&");
    MStringList query = queryStr.split("&");
    for (unsigned int i = 0; i < query.size(); ++i) {
        MString &frag = query[i];
        MStringList kv = frag.split("=");
        if (kv.size() > 1) {
            m_queryField[kv.at(0)] = kv.at(1);
        }
    }

    return 0;
}
