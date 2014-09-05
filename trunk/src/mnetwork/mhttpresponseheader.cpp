
/*
 * Copyright (C) wenjie.zhao
 */


#include "mhttpresponseheader.hpp"

MHttpResponseHeader::MHttpResponseHeader()
{
}

int MHttpResponseHeader::majorVersion() const
{
    return m_majorVersion;
}

int MHttpResponseHeader::minorVersion() const
{
    return m_minorVersion;
}

MString MHttpResponseHeader::toString() const
{
    MString ret;
    ret.sprintf("HTTP/%d.%d %d %s\r\n",
                m_majorVersion, m_minorVersion, m_code, m_codeText.c_str());
    ret.append(MHttpHeader::toString());

    return ret;
}

void MHttpResponseHeader::setStatusLine(int code, const MString &text, int majorVer, int minorVer)
{
    m_majorVersion = majorVer;
    m_minorVersion = minorVer;

    MString codeText;
    switch (code) {
    case 200:
        codeText = "OK";
        break;
    case 404:
        codeText = "Not Found";
        break;
    default:
        code = 500;
        codeText = "server busy.";
        break;
    }

    m_code = code;
    m_codeText = codeText;
}

int MHttpResponseHeader::statusCode() const
{
    return m_code;
}

void MHttpResponseHeader::setServer(const MString &server)
{
    addValue("Server", server);
}
