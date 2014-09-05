
/*
 * Copyright (C) wenjie.zhao
 */


#include "mfile.hpp"

#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "mstringlist.hpp"

#include "merrno.hpp"

MFile::MFile(const MString &_m_fileName)
    : m_fileName(_m_fileName)
    , fp(NULL)
    , m_autoFlush(false)
{
}

MFile::~MFile()
{
    if (isOpen()) {
        close();
    }
}

bool MFile::atEnd()
{
    return pos() == size();
}

MString MFile::fileName()
{
    return m_fileName;
}

MString MFile::filePath()
{
    return filePath(m_fileName);
}

MString MFile::filePath(const MString &path)
{
    MString temp = path;
    MStringList args = temp.split("/");
    if (args.size() > 0) {
        args.remove(args.back());
    }

    MString ret = args.join("/");
    if (temp.startWith("/")) {
        ret.prepend("/");
    }

    return ret;
}

bool MFile::open(const MString &mode)
{
    if (fp) return true;

    fp = fopen(m_fileName.c_str(), mode.c_str());
    if (!fp) {
        merrno = errno;
        return false;
    }

    return true;
}

bool MFile::isOpen()
{
    return (fp);
}

void MFile::close()
{
    fclose(fp);
    fp = NULL;
}

mint64 MFile::size()
{
    struct stat buf;
    if (stat(m_fileName.c_str(), &buf) == -1) {
        merrno = errno;
        return -1;
    }

    return (mint64)buf.st_size;
}

int MFile::seek(mint64 pos)
{
    // see lseek  which is a 32 bit version
//    off64_t res = lseek(fd, (off64_t)pos, SEEK_SET);
//    if (res < 0) {
//        merrno = errno;
//        return -1;
//    }

    if (pos > size()) {
        merrno = errno;
        return -1;
    }

    fseek(fp, pos, SEEK_SET);

    return ftell(fp);
}

mint64 MFile::pos()
{
    mint64 currentPos = ftell(fp);
    if (currentPos == -1) {
        merrno = errno;
        return -1;
    }

    return currentPos;
}

void MFile::setAutoFlush(bool enabled)
{
    m_autoFlush = enabled;
}

bool MFile::flush()
{
    // fd is not buffered device
    int ret = fflush(fp);
    if (ret == EOF) {
        merrno = errno;
        return false;
    }

    return  true;
}

MString MFile::errorString()
{
    return mstrerror(merrno);
}

mint64 MFile::read(char *data, mint64 maxSize)
{
    muint64 lastPos = pos();

    int readCnt = ::fread((void *)data, maxSize, 1, fp);
    if (readCnt < 1) {
        // check if reach end-of-file
        if (feof(fp)) {
            return size() - lastPos;
        }
        if (ferror(fp)) {
            clearerr(fp);
            return -1;
        }
    }

    return maxSize;
}

MString MFile::read(mint64 maxSize)
{
    MString array;
    char *buffer = new char[maxSize];
    mAutoFreeArray(char, buffer);

    int bytes = read(buffer, maxSize);
    if (bytes == -1) {
        return array;
    }

    if (bytes > 0) {
        array.append(buffer, bytes);
    }

    return array;
}

MString MFile::readAll()
{
    MString array;
    while (true) {
        MString bytes = read(1024 * 8);
        if (!bytes.empty()) {
            array.append(bytes);
            if (bytes.size() < 1024 * 8) {  // reach end-of-file
                break;
            }
        } else {
            break;
        }
    }

    return array;
}

mint64 MFile::readLine(char *data, mint64 maxSize)
{
    char *ret = fgets(data, maxSize, fp);
    if (!ret) {
        if (feof(fp)) {
            return 0;
        } else {
            merrno = errno;
            return -1;
        }
    }

    return strlen(data);
}

MString MFile::readLine(mint64 maxSize)
{
    MString array;
    char *buffer = new char[maxSize];
    mAutoFreeArray(char, buffer);
    if (readLine(buffer, maxSize) == -1) {
        return array;
    }
    array.append(buffer);

    return array;
}

mint64 MFile::write(const char *data, mint64 maxSize)
{
//    ssize_t writeCount = ::write(fd, data, maxSize);
//    if (writeCount == -1) {
//        merrno = errno;
//        return -1;
//    }
    mint64 ret = 0;
    int nitem = fwrite(data, maxSize, 1, fp);
    if (nitem < 1) {
        merrno = errno;
        return -1;
    }

    if (m_autoFlush) {
        bool ret = flush();
        if (!ret) {
            return -2;
        }
    }

    if (nitem == 1) {
        ret = maxSize;
    }

    return ret;
}

mint64 MFile::write(const char *data)
{
    return write(data, strlen(data));
}

mint64 MFile::write(const MString &byteArray)
{
    return write(byteArray.data(), byteArray.size());
}

bool MFile::isFile()
{
    struct stat buf;
    if (stat(m_fileName.c_str(), &buf) == -1) {
        merrno = errno;
        return false;
    }

    if (S_ISREG(buf.st_mode)) {
        return true;
    }

    return false;
}

bool MFile::isFile(const MString &filePath)
{
    MFile f(filePath);

    return f.isFile();
}

bool MFile::exists()
{
    return isFile();
}

bool MFile::exists(const MString &filePath)
{
    MFile f(filePath);

    return f.exists();
}

bool MFile::remove()
{
    if (unlink(m_fileName.c_str()) == -1) {
        merrno = errno;
        return false;
    }

    return true;
}

bool MFile::remove(const MString &filePath)
{
    MFile f(filePath);

    return f.remove();
}

MString MFile::suffix()
{
    return MFile::suffix(fileName());
}

MString MFile::suffix(const MString &fileName)
{
    // find the last "."
    MString temp = fileName;
    MStringList all = temp.split(".");

    if (all.size() == 0) {
        return "";
    }

    if (all.size() == 1) {
        return all.at(0);
    }

    if (all.size() > 1) {
        return all.at(all.size() -1);
    }

    return "";
}

MString MFile::baseName()
{
    return MFile::baseName(m_fileName);
}

MString MFile::baseName(const MString &fileName)
{
    // find the last "."
    MString temp = fileName;
    MStringList all = temp.split("/");

    if (all.size() == 0) {
        return "";
    }

    if (all.size() == 1) {
        return all.at(0);
    }

    if (all.size() > 1) {
        return all.at(all.size() -1);
    }

    return "";
}
