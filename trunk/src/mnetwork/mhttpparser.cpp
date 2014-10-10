
/*
 * Copyright (C) wenjie.zhao
 */


#include "mhttpparser.hpp"

#include <string.h>

MHttpParser::MHttpParser(int parserType)
    : m_parserType(parserType)
{
}

int MHttpParser::parse(const char *data, unsigned int length)
{
    memset(&m_httpSettings, 0, sizeof(m_httpSettings));
    m_httpSettings.on_message_begin = _onMessageBegin;
    m_httpSettings.on_url = _onUrl;
    m_httpSettings.on_header_field = _onHeaderField;
    m_httpSettings.on_header_value = _onHeaderValue;
    m_httpSettings.on_headers_complete = _onHeaderscComplete;
    m_httpSettings.on_body = _onBody;
    m_httpSettings.on_message_complete = _onMessageComplete;

    http_parser_init(&m_parser, (http_parser_type)m_parserType);
    m_parser.data = reinterpret_cast<void *>(this);

    size_t parsedCount = http_parser_execute(&m_parser, &m_httpSettings, data, length);

    return parsedCount;
}

int MHttpParser::parse(const MString &data)
{
    const char *da = data.data();
    unsigned int length = data.length();

    return parse(da, length);
}

MString MHttpParser::feild(const MString &key)
{
    map<MString, MString>::iterator iter;
    iter = m_fields.find(key);
    if (iter != m_fields.end())
    {
        return m_fields[key];
    }

    return "";
}

MString MHttpParser::url()
{
    return m_url;
}

MString MHttpParser::body()
{
    return m_body;
}

MString MHttpParser::method()
{
    return http_method_str((enum http_method)m_parser.method);
}

MString MHttpParser::host()
{
    return feild("Host");
}

bool MHttpParser::headerParseComplete()
{
    return m_headerParseComplete;
}

bool MHttpParser::bodyParseComplete()
{
    //return http_body_is_final(&m_parser);
    return m_bodyParseComplete;
}

bool MHttpParser::error()
{
    return m_error;
}

int MHttpParser::_onMessageBegin(http_parser* parser)
{
    MHttpParser *object = reinterpret_cast<MHttpParser *>(parser->data);
    object->m_headerParseComplete = false;
    object->m_bodyParseComplete = false;
    object->m_error = false;

    return object->onMessageBegin(parser);
}

int MHttpParser::_onHeaderscComplete(http_parser* parser)
{
    MHttpParser *object = reinterpret_cast<MHttpParser *>(parser->data);
    object->m_headerParseComplete = true;

    return object->onHeaderscComplete(parser);
}

int MHttpParser::_onMessageComplete(http_parser* parser)
{
    MHttpParser *object = reinterpret_cast<MHttpParser *>(parser->data);
    object->m_bodyParseComplete = true;

    return object->onMessageComplete(parser);
}

int MHttpParser::_onUrl(http_parser* parser, const char* at, size_t length)
{
    MHttpParser *object = reinterpret_cast<MHttpParser *>(parser->data);
    object->m_url = MString(at, (int)length);

    return object->onUrl(parser, at, length);
}

int MHttpParser::_onHeaderField(http_parser* parser, const char* at, size_t length)
{
    MHttpParser *object = reinterpret_cast<MHttpParser *>(parser->data);
    object->m_currentFieldKey = MString(at, (int)length);

    return object->onHeaderField(parser, at, length);
}

int MHttpParser::_onHeaderValue(http_parser* parser, const char* at, size_t length)
{
    MHttpParser *object = reinterpret_cast<MHttpParser *>(parser->data);
    object->m_fields[object->m_currentFieldKey] = MString(at, (int)length);

    return object->onHeaderField(parser, at, length);
}

int MHttpParser::_onBody(http_parser* parser, const char* at, size_t length)
{
    MHttpParser *object = reinterpret_cast<MHttpParser *>(parser->data);
    object->m_body = MString(at, (int)length);

    return object->onBody(parser, at, length);
}

int MHttpParser::onMessageBegin(http_parser* parser)
{
    return 0;
}

int MHttpParser::onHeaderscComplete(http_parser* parser)
{
    return 0;
}

int MHttpParser::onMessageComplete(http_parser* parser)
{
    return 0;
}

int MHttpParser::onUrl(http_parser* parser, const char* at, size_t length)
{
    return 0;
}

int MHttpParser::onHeaderField(http_parser* parser, const char* at, size_t length)
{
    return 0;
}

int MHttpParser::onHeaderValue(http_parser* parser, const char* at, size_t length)
{
    return 0;
}

int MHttpParser::onBody(http_parser* parser, const char* at, size_t length)
{
    return 0;
}
