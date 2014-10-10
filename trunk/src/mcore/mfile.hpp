
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MFILE_HPP
#define MFILE_HPP

#include "mstring.hpp"
#include "mglobal.hpp"

class MFile
{
public:
    MFile(const MString &_fileName);
    ~MFile();

public:
    bool atEnd();
    MString fileName();
    MString filePath();
    static MString filePath(const MString &path);

    bool open(const MString &mode);
    bool isOpen();
    void close();

    mint64 size();
    int seek(mint64 pos);
    mint64 pos();

    void setAutoFlush(bool enabled);
    bool flush();
    MString errorString();

    mint64 read(char *data, mint64 maxSize);
    MString read(mint64 maxSize);
    MString readAll();

    mint64 readLine(char *data, mint64 maxSize);
    MString readLine(mint64 maxSize = 0);

    /**
    * @brief write data to file
    * @param @a data with @a size
    * @retval the actual size of writed.
    */
    mint64 write(const char *data, mint64 size);
    /**
    * @overload write()
    * @brief write string data to file
    */
    mint64 write(const char *data);
    /**
    * @overload write()
    * @brief write @a byteArray with byteArray.size() to file
    */
    mint64 write(const MString &byteArray);

    bool isFile();
    static bool isFile(const MString &filePath);

    bool exists();

    static bool exists(const MString &filePath);

    bool remove();
    static bool remove(const MString &filePath);

    /*!
        Returns the suffix of the file.
        The suffix consists of all characters in the file after (but not including) the last '.'.
        Example:

        "/tmp/archive.tar.gz" --->  gz;
     */
    MString suffix();
    static MString suffix(const MString &fileName);

    MString baseName();
    static MString baseName(const MString &fileName);

    /*!
        return the file handle.
    */
    inline FILE *handle() { return fp; }

private:
    MString m_fileName;
    FILE *fp;
    bool m_autoFlush;
};

#endif // MFILE_HPP
