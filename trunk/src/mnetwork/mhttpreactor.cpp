
/*
 * Copyright (C) wenjie.zhao
 */


#include "mhttpreactor.hpp"

#include <MTcpSocket>
#include <MHttpParser>
#include <MHttpResponseHeader>
#include <mhttprequestheader.hpp>
#include <MElapsedTimer>
#include <MUrl>
#include <MStringList>
#include <MLoger>

#define MAX_HTTP_HEADER_LENGTH  4096

MHttpReactor::MHttpReactor(MObject *parent)
    : MThread(parent)
    , m_socket(new MTcpSocket(this))
    , m_parser(new MHttpParser(HTTP_REQUEST))
{
    m_socket->setRecvTimeout(1000*1000*60);
}

MHttpReactor::MHttpReactor(MTcpSocket *socket, MObject *parent)
    : MThread(parent)
    , m_socket(socket)
    , m_parser(new MHttpParser(HTTP_REQUEST))
{
    m_socket->setParent(this);
    m_socket->setRecvTimeout(1000*1000*60);
}

MHttpReactor::~MHttpReactor()
{
    mFree(m_parser);
}

int MHttpReactor::run()
{
    MString httpContent;
    MElapsedTimer timer;
    while (true) {
        char buf[2048];
        int nread = m_socket->read(buf, 2048);
        if (nread <= 0) {
            return -1;
        }

        httpContent.append(buf, nread);
        if (m_parser->parse(httpContent.data(), httpContent.length()) < 0) {
            return -2;
        }

        if (m_parser->headerParseComplete()) {
            // currently, only method GET is supported
            if (m_parser->method() != HTTP_METHOD_GET) {
                return -3;
            }
            break;
        }

        if (timer.hasExpired(1000 * 30)) {     // timer elapsed 10 s
            return -4;
        }

        if (httpContent.size() > MAX_HTTP_HEADER_LENGTH) {
            return -5;
        }
    }

    return E_SUCCESS;
}

int MHttpReactor::response(const MHttpResponseHeader &header,
                           const char *data /*= 0*/, muint32 length /*= 0*/)
{
    // header
    MString headerString = header.toString();

    if (data)
        headerString.append(data, length);

    if (m_socket->write(headerString.data(), headerString.length()) < 0) {
        merrno = errno;
        return -1;
    }

    return 0;
}

int MHttpReactor::response(const MHttpResponseHeader &header, const MString &data)
{
    return response(header, data.data(), data.length());
}

int MHttpReactor::writeResponse(const MHttpHeader &header)
{
    MString data = header.toString();
    if (m_socket->write(data.data(), data.size()) < 0) {
        merrno = errno;
        return -1;
    }

    return 0;
}

MString MHttpReactor::readHeader()
{
    MString httpContent;
    while (true) {
        char buf[2048];
        int nread = m_socket->read(buf, 2048);
        if (nread <= 0) {
            return "";
        }

        httpContent.append(buf, nread);
        if (httpContent.contains("\r\n\r\n")) {
            MStringList lines = httpContent.split("\r\n\r\n");
            if (lines.size() >=2) {
                lines.pop_front();
                return lines.join("\r\n\r\n");
            }
        }
    }

    return "";
}

int MHttpReactor::post(const MString &path, const MString &data)
{
    MUrl url(path);
    if (m_socket->connectToHost(url.host(), url.port()) != 0) {
        return -1;
    }

    MHttpRequestHeader header;
    header.setContent(data);
    header.setRequest(HTTP_METHOD_POST, url.path());

    return writeResponse(header);
}

// not support HTTP 1.1 CHUNKED
MString MHttpReactor::get(const MString &urlStr)
{
    MUrl url(urlStr);
    if (m_socket->connectToHost(url.host(), url.port()) != 0) {
        return "";
    }

    MHttpRequestHeader header;
    MString path = url.path();
    if (url.hasQuery()) {
        path.sprintf("?%s", url.query().c_str());
    }
    if (url.hasFragment()) {
        path.sprintf("#%s", url.fragment().c_str());
    }
    header.setRequest(HTTP_METHOD_GET, path, 1, 0);
    header.setHost(url.host(), url.port());
    header.addValue("Connection", "close");

    if (writeResponse(header) != 0)
    {
        return "";
    }

    MString body = readHeader();
    int content_length = m_parser->feild("Content-Length").toInt();
    if (content_length < 0) {
        return "";
    }

    // recv until peer disconnected.
    if (content_length == 0) {
        char *left_body = new char[2048];
        mAutoFreeArray(char, left_body);
        while (true) {
            int ret = m_socket->read(left_body, 2047);
            if (ret <= 0) {
                break;
            }

            body.append(left_body, ret);
        }

        return body;
    }

    int leftBytes = content_length - body.size();
    mAssert(leftBytes >= 0);
    if (leftBytes == 0) {
        return body;
    }

    char *left_body = new char[leftBytes];
    mAutoFreeArray(char, left_body);

    if (m_socket->readFully(left_body, leftBytes) <= 0) {
        log_error("http get readFully error.");
        return "";
    }
    body.append(left_body, leftBytes);

    return body;
}

int MHttpReactor::readHttpHeader(MHttpParser &parser, MString &body, MTcpSocket &socket)
{
    MString httpContent;
    MElapsedTimer timer;

    while (true) {
        int bufSize = 2048;
        char buf[bufSize];

        int nread = socket.read(buf, bufSize);
        if (nread <= 0) {
            return -1;
        }

        httpContent.append(buf, nread);

        int hasParsedCount = parser.parse(httpContent.data(), httpContent.length());
        if (hasParsedCount < 0) {
            return -2;
        }

        if (parser.headerParseComplete()) {
            break;
        }

        if (timer.hasExpired(1000 * 10)) {     // timer elapsed 10 s
            return -4;
        }

        if (httpContent.size() > MAX_HTTP_HEADER_LENGTH) {
            return -5;
        }
    }

    MString delim("\r\n\r\n");
    size_t index = httpContent.find(delim);
    if (index != MString::npos) {
        body = httpContent.substr(index + delim.size());
    }

    return E_SUCCESS;
}
