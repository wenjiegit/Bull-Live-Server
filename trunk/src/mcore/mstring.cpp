
/*
 * Copyright (C) wenjie.zhao
 */


#include "mstring.hpp"

#include <merrno.hpp>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "mstringlist.hpp"

MString::MString()
{
}

MString::MString(const char *str)
    : string(str)
{

}

MString::MString(const char *str, int n)
    : string(str, n)
{

}

MString::MString(const string &str)
    : string(str)
{

}

//MString::MString(const string &str, size_t pos, size_t len)
//    : string(str, pos, len)
//{

//}

MString::MString(size_t n, char c)
    : string(n, c)
{

}

MString::~MString()
{
}

string MString::toStdString()
{
    return string(c_str());
}

void MString::chop(int n)
{
    if ((muint32)n >= size()) {
        clear();

        return;
    }

    erase(size() - n, n);
}

// '\t', '\n', '\v', '\f', '\r', and ' '
MString MString::trimmed() const
{
    int beginIndex = 0;
    int endIndex = 0;

    // find beginIndex
    for (unsigned int i = 0; i < size(); ++i) {
        char c = this->at(i);
        if (c == '\t'
                || c == '\n'
                || c == '\v'
                || c == '\f'
                || c == '\r'
                || c == ' ') {
            ++beginIndex;
        } else {
            break;
        }
    }

    // find endIndex
    for (int i = size() - 1; i >= 0; --i) {
        char c = this->at(i);
        if (c == '\t'
                || c == '\n'
                || c == '\v'
                || c == '\f'
                || c == '\r'
                || c == ' ') {
            ++endIndex;
        } else {
            break;
        }
    }

    return substr(beginIndex, size() - beginIndex - endIndex);
}

void MString::truncate(int position)
{
    MStringIterator first = begin() + position;
    erase(first, end());
}

MString &MString::sprintf(const char *cformat, ...)
{
    va_list vp;
    va_start(vp, cformat);

    char buffer[1024];
    vsprintf(buffer, cformat, vp);
    va_end(vp);
    this->append(buffer);

    return *this;
}

bool MString::contains(const MString &str) const
{
    return find(str) != npos;
}

bool MString::contains(const string &str) const
{
    return find(str) != npos;
}

bool MString::contains(const char *str) const
{
    return find(str) != npos;
}

bool MString::startWith(const MString &str) const
{
    return find_first_of(str) == 0;
}

bool MString::startWith(const string &str) const
{
    return find_first_of(str) == 0;
}

bool MString::startWith(const char *str) const
{
    return find_first_of(str) == 0;
}

bool MString::endWith(const MString &str) const
{
    if (size() < str.size()) return false;

    MString ss = substr(size() - str.size(), str.size());

    return ss == str;
}

bool MString::endWith(const string &str) const
{
    return endWith(MString(str));
}

bool MString::endWith(const char *str) const
{
    return endWith(MString(str));
}

MString &MString::replace(const MString &before, const MString &after, bool replaceAll)
{
    size_t  pos = npos;
    while ((pos = find(before.c_str())) != npos) {
        string::replace(pos, before.size(), after);

        if (!replaceAll) {
            break;
        }
    }

    return *this;
}

int MString::toInt(bool *ok, int base)
{
    char *stop;
    int ret = strtol(c_str(), &stop, base);
    if (ret == LONG_MIN
            || ret == LONG_MAX) {
        if (ok) {
            *ok = false;
        }
        merrno = errno;
    } else {
        if (ok) {
            *ok = true;
        }
    }

    return ret;
}

short MString::toShort(bool *ok, int base)
{
    char *stop;
    int ret = strtol(c_str(), &stop, base);
    if (ret == LONG_MIN
            || ret == LONG_MAX) {
        if (ok) {
            *ok = false;
        }
        merrno = errno;
    } else {
        if (ok) {
            *ok = true;
        }
    }

    return (short)ret;
}

MString MString::toHex()
{
    MString ret;
    MStringIterator iter = begin();
    while (iter != end()) {
        char buf[2] = {'\0', '\0'};
        ::sprintf(buf, "%x", *iter);
        ret.append(buf);
        ++iter;
    }

    return ret;
}

muint64 MString::toInt64(bool *ok, int base)
{
    char *stop;
    muint64 ret = strtoll(c_str(), &stop, base);
    if (ret == LLONG_MIN
            || ret == LLONG_MAX) {
        if (ok) {
            *ok = false;
        }
        merrno = errno;
    } else {
        if (ok) {
            *ok = true;
        }
    }

    return ret;
}

MStringList MString::split(const MString &sep)
{
    MString temp(*this);
    MStringList ret;
    if (sep.isEmpty()) {
        return ret;
    }

    while (temp.contains(sep)) {
        size_type index = temp.find(sep);

        MString ss = temp.substr(0, index);
        if (!ss.isEmpty()) {
            ret << ss;
        }
        temp = temp.substr(index + sep.size(), temp.size() - 1);
    }
    if (!temp.isEmpty()) {
        ret << temp;
    }

    return ret;
}

MString &MString::prepend(const MString &str)
{
    MString temp = *this;
    *this = str + temp;
    return *this;
}

MString &MString::prepend(const char *str, int size)
{
    MString temp(str, size);
    return prepend(temp);
}

MString & MString::operator <<(mint32 value)
{
    append(number(value));
    return *this;
}

MString & MString::operator <<(muint64 value)
{
    append(number(value));
    return *this;
}

MString & MString::operator <<(const MString &str)
{
    append(str);
    return *this;
}

MString MString::number(mint32 n, int base)
{
    // TODO process base
    M_UNUSED(base);
    char buffer[32] = {0};
    ::sprintf(buffer, "%d", n);

    return buffer;
}

MString MString::number(muint64 n, int base)
{
    // TODO process base
    M_UNUSED(base);
    char buffer[64] = {0};
    ::sprintf(buffer, "%lld", n);

    return buffer;
}

MString MString::number(size_t n, int base)
{
    // TODO process base
    M_UNUSED(base);
    char buffer[32] = {0};
    ::sprintf(buffer, "%d", n);

    return buffer;
}
