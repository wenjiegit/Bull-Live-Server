
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MHTTPHEADER_HPP
#define MHTTPHEADER_HPP

#include <MString>
#include <list>
#include <set>
#include <map>

/*!
    HTTP MIME type

    超文本标记语言文本          .html,.html text/html
    普通文本                   .txt text/plain
    RTF文本                   .rtf application/rtf
    GIF图形                   .gif image/gif
    JPEG图形                  .ipeg,.jpg image/jpeg
    au声音文件                 .au audio/basic
    MIDI音乐文件               .mid,.midi audio/midi,audio/x-midi
    RealAudio音乐文件          .ra, .ram audio/x-pn-realaudio
    MPEG文件                  .mpg,.mpeg video/mpeg
    AVI文件                   .avi video/x-msvideo
    GZIP文件                  .gz application/x-gzip
    TAR文件                   .tar application/x-tar
*/

class MHttpHeader
{
public:
    MHttpHeader();

    void addValue(const MString &key, const MString &value);
    void removeValue(const MString &key);

    virtual int majorVersion() const = 0;
    virtual int minorVersion() const = 0;

    virtual MString toString() const;
    MString value(const MString &key) const;

    void setContentLength(int len);
    void setContentType(const MString &type);
    void setHost(const MString &host, int port);
    void setServer(const MString &server);
    void setConnectionClose();
    void setConnectionKeepAlive();
    void setDate();
    void setContent(const MString &content);
    void appendContent(const MString &append);

    static MString contentType(const MString &fileType);

protected:
    int m_majorVersion;
    int m_minorVersion;
    static map<MString, MString> m_mime;
    MString m_content;

private:
    list<pair<MString, MString> > m_header;
    map<MString, int> m_headerIndex;
};

#endif // MHTTPHEADER_HPP
