
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MLOGER_HPP
#define MLOGER_HPP

#include "mloger.hpp"

#include "mstring.hpp"

#include <stdarg.h>

class MLogerInterface
{
public:
    MLogerInterface();
    virtual ~MLogerInterface();

    virtual void verbose(const char *file, muint16 line, const char *function, const char *tag, const char* fmt, ...);
    virtual void info(const char *file, muint16 line, const char *function, const char *tag,  const char* fmt, ...);
    virtual void trace(const char *file, muint16 line, const char *function, const char *tag,  const char* fmt, ...);
    virtual void warn(const char *file, muint16 line, const char *function, const char *tag,  const char* fmt, ...);
    virtual void error(const char *file, muint16 line, const char *function, const char *tag,  const char* fmt, ...);
};

class MLogLevel
{
public:
    static const int Verbose    = 0x01;
    static const int Info       = 0x02;
    static const int Trace      = 0x04;
    static const int Warn       = 0x08;
    static const int Error      = 0x0F;
};

class MFile;
class MLoger : public MLogerInterface
{
public:
    MLoger();
    ~MLoger();

    void setLogLevel(int level);

    void setEnableCache(bool enabled);
    void setLog2Console(bool enabled);
    void setLog2File(bool enabled);
    void setEnableFILE(bool enabled);
    void setEnableLINE(bool enabled);
    void setEnableFUNCTION(bool enabled);
    void setEnableColorPrint(bool enabled);

    void setTimeFormat(const MString &fmt);
    void setFilePath(const MString &path);

    virtual void verbose(const char *file, muint16 line, const char *function, const char *tag, const char* fmt, ...);
    virtual void info(const char *file, muint16 line, const char *function, const char *tag,  const char* fmt, ...);
    virtual void trace(const char *file, muint16 line, const char *function, const char *tag,  const char* fmt, ...);
    virtual void warn(const char *file, muint16 line, const char *function, const char *tag,  const char* fmt, ...);
    virtual void error(const char *file, muint16 line, const char *function, const char *tag,  const char* fmt, ...);

private:
    void log(int level, const char *file, muint16 line, const char *function, const char *tag, const char* fmt, va_list ap);

private:
    int m_logLevel;
    bool m_enableCache;
    bool m_log2Console;
    bool m_log2File;
    bool m_enablFILE;
    bool m_enableLINE;
    bool m_enableFUNTION;
    bool m_enableColorPrint;
    MFile *m_file;
    char *m_buffer;
    muint16 m_bufferLength;

    MString m_timeFormat;
    MString m_filePath;
};

extern MLogerInterface *g_logCtx;

#define log_verbose(msg, ...) \
    g_logCtx->verbose(__FILE__, __LINE__, __FUNCTION__, NULL, msg, ##__VA_ARGS__)

#define log_info(msg, ...) \
    g_logCtx->info(__FILE__, __LINE__, __FUNCTION__, NULL, msg, ##__VA_ARGS__)

#define log_trace(msg, ...) \
    g_logCtx->trace(__FILE__, __LINE__, __FUNCTION__, NULL, msg, ##__VA_ARGS__)

#define log_warn(msg, ...) \
    g_logCtx->warn(__FILE__, __LINE__, __FUNCTION__, NULL, msg, ##__VA_ARGS__)

#define log_error(msg, ...) \
    g_logCtx->error(__FILE__, __LINE__, __FUNCTION__, NULL, msg, ##__VA_ARGS__)

#define log_error_with_errno(err, msg, ...) \
    setError(err); \
    g_logCtx->error(__FILE__, __LINE__, __FUNCTION__, NULL, msg, ##__VA_ARGS__)

#endif // MLOGER_HPP
