
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MHTTPREQUESTHEADER_HPP
#define MHTTPREQUESTHEADER_HPP

#include <MHttpHeader>

class MHttpRequestHeader : public MHttpHeader
{
public:
    MHttpRequestHeader();

    virtual int    majorVersion() const;
    virtual int    minorVersion() const;
    MString    toString() const;

    void setRequest(const MString &method, const MString &path, int majorVer = 1, int minorVer = 1);

private:
    MString m_method;
    MString m_path;
};

#endif // MHTTPREQUESTHEADER_HPP
