
/*
 * Copyright (C) wenjie.zhao
 */


#include "mhttpheader.hpp"

map<MString, MString> MHttpHeader::m_mime;

MHttpHeader::MHttpHeader()
    : m_majorVersion(1)             // default is HTTP 1.1
    , m_minorVersion(1)
{

    if (m_mime.empty()) {
        m_mime["html"]      = "text/html";
        m_mime["htm"]       = "text/html";
        m_mime["shtml"]     = "text/html";
        m_mime["css"]       = "text/css";
        m_mime["xml"]       = "text/xml";
        m_mime["gif"]       = "image/gif";
        m_mime["jpeg"]      = "image/jpeg";
        m_mime["jpg"]       = "image/jpeg";
        m_mime["js"]        = "application/javascript";
        m_mime["atom"]      = "application/atom+xml";
        m_mime["rss"]       = "application/rss+xml";

        m_mime["mml"]       = "text/mathml";
        m_mime["txt"]       = "text/plain";
        m_mime["jad"]       = "text/vnd.sun.j2me.app-descriptor";
        m_mime["wml"]       = "text/vnd.wap.wml";
        m_mime["htc"]       = "text/x-component";

        m_mime["png"]       = "image/png";
        m_mime["tif"]       = "image/tiff";
        m_mime["tiff"]      = "image/vnd.wap.wbmp";
        m_mime["wbmp"]      = "image/x-icon";
        m_mime["ico"]       = "image/x-jng";
        m_mime["jng"]       = "image/x-ms-bmp";
        m_mime["bmp"]       = "text/x-component";
        m_mime["svg"]       = "image/svg+xml";
        m_mime["svgz"]      = "image/svg+xml";
        m_mime["webp"]      = "image/webp";

        m_mime["woff"]      = "application/font-woff";
        m_mime["jar"]       = "application/java-archive";
        m_mime["war"]       = "application/java-archive";
        m_mime["ear"]       = "application/java-archive";
        m_mime["json"]      = "application/json";
        m_mime["hqx"]       = "application/mac-binhex40";
        m_mime["doc"]       = "application/msword";
        m_mime["pdf"]       = "application/pdf";
        m_mime["ps"]        = "application/postscript";
        m_mime["eps"]       = "application/postscript";
        m_mime["ai"]        = "application/postscript";


        m_mime["rtf"]       = "application/rtf";
        m_mime["xls"]       = "application/vnd.ms-excel";
        m_mime["eot"]       = "application/vnd.ms-fontobject";
        m_mime["ppt"]       = "application/vnd.ms-powerpoint";
        m_mime["wmlc"]      = "application/vnd.wap.wmlc";
        m_mime["kml"]       = "application/vnd.google-earth.kml+xml";
        m_mime["kmz"]       = "application/vnd.google-earth.kmz";
        m_mime["7z"]        = "application/x-7z-compressed";
        m_mime["cco"]       = "application/x-cocoa";
        m_mime["jardiff"]   = "application/x-java-archive-diff";

        m_mime["jnlp"]      = "application/x-java-jnlp-file";
        m_mime["run"]       = "application/x-makeself";
        m_mime["pl"]        = "application/x-perl";
        m_mime["pm"]        = "application/x-perl";
        m_mime["prc"]       = "application/x-pilot";
        m_mime["pdb"]       = "application/x-pilot";
        m_mime["rar"]       = "application/x-rar-compressed";
        m_mime["rpm"]       = "application/x-redhat-package-manager";
        m_mime["sea"]       = "application/x-sea";
        m_mime["swf"]       = "application/x-shockwave-flash";


        m_mime["sit"]       = "application/x-stuffit";
        m_mime["tcl"]       = "application/x-tcl";
        m_mime["tk"]        = "application/x-tcl";
        m_mime["der"]       = "application/x-x509-ca-certl";
        m_mime["pem"]       = "application/x-x509-ca-cert";
        m_mime["crt"]       = "application/x-x509-ca-cert";
        m_mime["xpi"]       = "application/x-xpinstall";
        m_mime["xhtml"]     = "application/xhtml+xml";
        m_mime["zip"]       = "application/zip";

        m_mime["bin"]       = "application/octet-stream";
        m_mime["exe"]       = "application/octet-stream";
        m_mime["dll"]       = "application/octet-stream";
        m_mime["deb"]       = "application/octet-stream";
        m_mime["dmg"]       = "application/octet-stream";
        m_mime["iso"]       = "application/octet-stream";
        m_mime["img"]       = "application/octet-stream";
        m_mime["msi"]       = "application/octet-stream";
        m_mime["msp"]       = "application/octet-stream";
        m_mime["msm"]       = "application/octet-stream";

        m_mime["docx"]      = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        m_mime["xlsx"]      = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        m_mime["pptx"]      = "application/vnd.openxmlformats-officedocument.presentationml.presentation";

        m_mime["mid"]       = "audio/midi";
        m_mime["midi"]      = "audio/midi";
        m_mime["kar"]       = "audio/midi";
        m_mime["mp3"]       = "audio/mpeg";
        m_mime["ogg"]       = "audio/ogg";
        m_mime["m4a"]       = "audio/x-m4a ";
        m_mime["ra"]        = "audio/x-realaudio";
        m_mime["3gpp"]      = "video/3gpp";
        m_mime["3gp"]       = "video/3gpp";
        m_mime["mp4"]       = "video/mp4";
        m_mime["mpeg"]      = "video/mpeg";
        m_mime["mpg"]       = "video/mpeg";
        m_mime["mov"]       = "video/quicktime";
        m_mime["webm"]      = "video/webm";
        m_mime["flv"]       = "video/x-flv";
        m_mime["m4v"]       = "video/x-m4v";
        m_mime["mng"]       = "video/x-mng";
        m_mime["asx"]       = "video/x-ms-asf";
        m_mime["asf"]       = "video/x-ms-asf";
        m_mime["wmv"]       = "video/x-ms-wmv";
        m_mime["avi"]       = "video/x-msvideo";
        m_mime["ts"]        = "video/MP2T";
        m_mime["m3u8"]      = "application/x-mpegURL";

        m_mime["default"]   = "application/octet-stream";
    }
}

void MHttpHeader::addValue(const MString &key, const MString &value)
{
    m_header.push_back(make_pair<MString, MString>(key, value));
    m_headerIndex[key] = m_header.size() - 1;
}

void MHttpHeader::removeValue(const MString &key)
{
    // TODO : imp
//    list<pair<MString, MString> >::const_iterator iter;
//    for (iter = m_header.begin(); iter != m_header.end(); ++iter) {
//        pair<MString, MString> &value = *iter;
//        if (value.first == key) {
//            return value.second;
//        }
//    }
}

MString MHttpHeader::toString() const
{
    MString ret;
    list<pair<MString, MString> >::const_iterator iter;
    for (iter = m_header.begin(); iter != m_header.end(); ++iter) {
        const pair<MString, MString> &value = *iter;
        ret.append(value.first).append(": ").append(value.second).append("\r\n");
    }
    ret.append("\r\n");

    if (!m_content.empty()) {
        const_cast<MHttpHeader&>(*this).setContentLength((int)m_content.size());
        ret.append(m_content);
    }

    return ret;
}

MString MHttpHeader::value(const MString &key) const
{
    list<pair<MString, MString> >::const_iterator iter;
    for (iter = m_header.begin(); iter != m_header.end(); ++iter) {
        const pair<MString, MString> &value = *iter;
        if (value.first == key) {
            return value.second;
        }
    }

    return "";
}

void MHttpHeader::setContentLength(int len)
{
    addValue("Content-Length", MString::number(len));
}

void MHttpHeader::setContentType(const MString &type)
{
    addValue("Content-Type", type);
}

void MHttpHeader::setHost(const MString &host, int port)
{
    MString hp = host;
    if (port != 80) {
        hp.sprintf(":%d", port);
    }
    addValue("Host", hp);
}

void MHttpHeader::setServer(const MString &server)
{
    addValue("Server", server);
}

void MHttpHeader::setConnectionClose()
{
    addValue("Connection", "close");
}

void MHttpHeader::setConnectionKeepAlive()
{
    addValue("Connection", "Keep-Alive");
}

void MHttpHeader::setDate()
{
    // TODO : add imp
}

void MHttpHeader::setContent(const MString &content)
{
    m_content = content;
}

void MHttpHeader::appendContent(const MString &append)
{
    m_content.append(append);
}

MString MHttpHeader::contentType(const MString &fileType)
{
    if (m_mime.find(fileType) != m_mime.end()) {
        return m_mime[fileType];
    }

    return m_mime["default"];
}
