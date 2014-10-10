
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MHTTPPARSER_HPP
#define MHTTPPARSER_HPP

#include <http_parser.h>

#include <map>
#include <MString>

#define HTTP_METHOD_GET  "GET"
#define HTTP_METHOD_POST "POST"

class MHttpParser
{
public:
    // parserType  { HTTP_REQUEST, HTTP_RESPONSE, HTTP_BOTH };
    MHttpParser(int parserType);

    int parse(const char *data, unsigned int length);
    int parse(const MString &data);

    MString feild(const MString &key);
    MString url();
    MString body();
    MString method();       // request only
    MString host();

    bool headerParseComplete();
    bool bodyParseComplete();
    bool error();

public:
    // functions start with _ should not be called in outside.
    static int _onMessageBegin(http_parser* parser);
    static int _onHeaderscComplete(http_parser* parser);
    static int _onMessageComplete(http_parser* parser);
    static int _onUrl(http_parser* parser, const char* at, size_t length);
    static int _onHeaderField(http_parser* parser, const char* at, size_t length);
    static int _onHeaderValue(http_parser* parser, const char* at, size_t length);
    static int _onBody(http_parser* parser, const char* at, size_t length);

public:
    virtual int onMessageBegin(http_parser* parser);
    virtual int onHeaderscComplete(http_parser* parser);
    virtual int onMessageComplete(http_parser* parser);
    virtual int onUrl(http_parser* parser, const char* at, size_t length);
    virtual int onHeaderField(http_parser* parser, const char* at, size_t length);
    virtual int onHeaderValue(http_parser* parser, const char* at, size_t length);
    virtual int onBody(http_parser* parser, const char* at, size_t length);

private:
    http_parser_settings m_httpSettings;
    http_parser m_parser;
    map<MString, MString> m_fields;
    MString m_url;
    int m_parserType;
    bool m_headerParseComplete;
    bool m_bodyParseComplete;
    bool m_error;

private:
    MString m_currentFieldKey;
    MString m_body;
};

#endif // MHTTPPARSER_HPP
