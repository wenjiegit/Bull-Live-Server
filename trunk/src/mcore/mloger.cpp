
/*
 * Copyright (C) wenjie.zhao
 */


#include "mloger.hpp"

#include "mdate.hpp"
#include "mfile.hpp"

#include <iostream>

using namespace std;

MLogerInterface::MLogerInterface()
{
}

MLogerInterface::~MLogerInterface()
{
}

void MLogerInterface::verbose(const char */*file*/, muint16 /*line*/, const char */*function*/
                              , const char */*tag*/, const char */*fmt*/, ...)
{
}

void MLogerInterface::info(const char */*file*/, muint16 /*line*/, const char */*function*/
                           , const char */*tag*/, const char */*fmt*/, ...)
{
}

void MLogerInterface::trace(const char */*file*/, muint16 /*line*/, const char */*function*/
                            , const char */*tag*/, const char */*fmt*/, ...)
{
}

void MLogerInterface::warn(const char */*file*/, muint16 /*line*/, const char */*function*/
                           , const char */*tag*/, const char */*fmt*/, ...)
{
}

void MLogerInterface::error(const char */*file*/, muint16 /*line*/, const char */*function*/
                            , const char */*tag*/, const char */*fmt*/, ...)
{
}


MLoger::MLoger()
    : m_logLevel(MLogLevel::Trace)
    , m_enableCache(false)
    , m_log2Console(true)
    , m_log2File(false)
    , m_enablFILE(false)
    , m_enableLINE(false)
    , m_enableFUNTION(true)
    , m_enableColorPrint(true)
    , m_file(NULL)
    , m_buffer(NULL)
    , m_bufferLength(4096)
    , m_timeFormat("MM-dd hh:mm")
{
    m_filePath.sprintf("/var/log/zwj_%s.log"
                       , MDateTime::currentDate().toString("MM_dd_hh_mm_ss_ms").c_str());
    m_buffer = new char[m_bufferLength];
}

MLoger::~MLoger()
{
    mFree(m_file);
    mFreeArray(m_buffer);
}

void MLoger::setLogLevel(int level)
{
    m_logLevel = level;
}

void MLoger::setEnableCache(bool enabled)
{
    m_enableCache = enabled;
}

void MLoger::setLog2Console(bool enabled)
{
    m_log2Console = enabled;
}

void MLoger::setLog2File(bool enabled)
{
    m_log2File = enabled;
    m_file = new MFile(m_filePath);
    if (!m_file->open("w")) {
        cout << "open log failed : " << m_filePath << endl;
        return ;
    }

    m_file->setAutoFlush(m_enableCache);
}

void MLoger::setEnableFILE(bool enabled)
{
    m_enablFILE = enabled;
}

void MLoger::setEnableLINE(bool enabled)
{
    m_enableLINE = enabled;
}

void MLoger::setEnableFUNCTION(bool enabled)
{
    m_enableFUNTION = enabled;
}

void MLoger::setEnableColorPrint(bool enabled)
{
    m_enableColorPrint = enabled;
}

void MLoger::setTimeFormat(const MString &fmt)
{
    m_timeFormat = fmt;
}

void MLoger::setFilePath(const MString &path)
{
    m_filePath = path;
}

void MLoger::verbose(const char *file, muint16 line, const char *function
                     , const char *tag, const char *fmt, ...)
{
    if (m_logLevel > MLogLevel::Verbose) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    log(MLogLevel::Verbose, file, line, function, tag, fmt, ap);
    va_end(ap);
}

void MLoger::info(const char *file, muint16 line, const char *function
                  , const char *tag, const char *fmt, ...)
{
    if (m_logLevel > MLogLevel::Info) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    log(MLogLevel::Info, file, line, function, tag, fmt, ap);
    va_end(ap);
}

void MLoger::trace(const char *file, muint16 line, const char *function
                   , const char *tag, const char *fmt, ...)
{
    if (m_logLevel > MLogLevel::Trace) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    log(MLogLevel::Trace, file, line, function, tag, fmt, ap);
    va_end(ap);
}

void MLoger::warn(const char *file, muint16 line, const char *function
                  , const char *tag, const char *fmt, ...)
{
    if (m_logLevel > MLogLevel::Warn) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    log(MLogLevel::Warn, file, line, function, tag, fmt, ap);
    va_end(ap);
}

void MLoger::error(const char *file, muint16 line, const char *function
                   , const char *tag, const char *fmt, ...)
{
    if (m_logLevel > MLogLevel::Error) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    log(MLogLevel::Error, file, line, function, tag, fmt, ap);
    va_end(ap);
}

void MLoger::log(int level, const char *file, muint16 line, const char *function, const char *tag, const char *fmt, va_list ap)
{
    const char *p;

    switch (level) {
    case MLogLevel::Verbose:
        p = "verbose";
        break;
    case MLogLevel::Info:
        p = "info";
        break;
    case MLogLevel::Trace:
        p = "trace";
        break;
    case MLogLevel::Warn:
        p = "warn";
        break;
    case MLogLevel::Error:
        p = "error";
        break;
    default:
        p = "default";
        break;
    }

    MString time = MDateTime::currentDate().toString(m_timeFormat);
    int size = 0;
    size += snprintf(m_buffer+size, m_bufferLength-size, "[%s][%d]", time.c_str(), 0);
    size += snprintf(m_buffer+size, m_bufferLength-size, "[%s]", p);

    if (m_enablFILE) {
        size += snprintf(m_buffer+size, m_bufferLength-size, "[%s]", file);
    }

    if (m_enableLINE) {
        size += snprintf(m_buffer+size, m_bufferLength-size, "[%d]", line);
    }

    if (m_enableFUNTION) {
        size += snprintf(m_buffer+size, m_bufferLength-size, "[%s] ", function);
    }

    if (tag) {
        size += snprintf(m_buffer+size, m_bufferLength-size, "[%s] ", tag);
    }

    size += vsnprintf(m_buffer+size, m_bufferLength-size, fmt, ap);

    if (level == MLogLevel::Error) {
        size += snprintf(m_buffer+size, m_bufferLength-size, "(%s)", mstrerror(merrno));
    }

    size += snprintf(m_buffer+size, m_bufferLength-size, "\n");

    // log to console
    if (m_log2Console && m_enableColorPrint) {
        if (level <= MLogLevel::Trace) {
                  printf("\033[0m%s", m_buffer);
              } else if (level == MLogLevel::Warn) {
                  printf("\033[33m%s\033[0m", m_buffer);
              } else if (level == MLogLevel::Error){
                  printf("\033[31m%s\033[0m", m_buffer);
              }
    } else if (m_log2Console && !m_enableColorPrint) {
        printf("\033[0m%s", m_buffer);
    }

    // log to file
    if (m_log2File) {
        m_file->write(m_buffer, size);
    }
}

MLogerInterface *g_logCtx = new MLoger;
