
/*
 * Copyright (C) wenjie.zhao
 */


#include "mhttprequestheader.hpp"

MHttpRequestHeader::MHttpRequestHeader()
{
}

int MHttpRequestHeader::majorVersion() const
{
    return m_majorVersion;
}

int MHttpRequestHeader::minorVersion() const
{
    return m_minorVersion;
}

MString MHttpRequestHeader::toString() const
{
    // GET /static/nav/img/menu_bg_110e96d0.png HTTP/1.1
    MString ret;
    ret.sprintf("%s %s HTTP/%d.%d\r\n",
                m_method.c_str(), m_path.c_str(), m_majorVersion, m_minorVersion);
    ret.append(MHttpHeader::toString());

    return ret;
}

void MHttpRequestHeader::setRequest(const MString &method, const MString &path, int majorVer, int minorVer)
{
    m_method = method;
    m_path = path;
    m_majorVersion = majorVer;
    m_minorVersion = minorVer;
}
