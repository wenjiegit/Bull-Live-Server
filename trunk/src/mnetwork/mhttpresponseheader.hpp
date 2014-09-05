
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MHTTPRESPONSEHEADER_HPP
#define MHTTPRESPONSEHEADER_HPP

#include <MHttpHeader>

class MHttpResponseHeader : public MHttpHeader
{
public:
    MHttpResponseHeader();

    virtual int    majorVersion() const;
    virtual int    minorVersion() const;
    MString    toString() const;

    void setStatusLine(int code, const MString & text = MString(), int majorVer = 1, int minorVer = 1);
    int    statusCode() const;

    void setServer(const MString &server);

private:
    int m_code;
    MString m_codeText;
};

#endif // MHTTPRESPONSEHEADER_HPP
