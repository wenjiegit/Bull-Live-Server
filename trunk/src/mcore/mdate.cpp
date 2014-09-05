
/*
 * Copyright (C) wenjie.zhao
 */


#include "mdate.hpp"

#include <time.h>
#include <sys/time.h>
#include <string.h>

MDateTime::MDateTime()
{
    //memset(&m_tv, 0, sizeof(timeval));
}

MDateTime::~MDateTime()
{
}

//yyyy MM dd hh mm ss ms
MString MDateTime::toString(const MString &format)
{
    struct tm *tm;
    if ((tm = localtime(&m_tv.tv_sec)) == NULL) {
        merrno = errno;
        return "";
    }

    MString yyyy = MString().sprintf("%d",   1900 + tm->tm_year);
    MString MM   = MString().sprintf("%02d", 1 + tm->tm_mon);
    MString dd   = MString().sprintf("%02d", tm->tm_mday);
    MString hh   = MString().sprintf("%02d", tm->tm_hour);
    MString mm   = MString().sprintf("%02d", tm->tm_min);
    MString ss   = MString().sprintf("%02d", tm->tm_sec);
    MString ms   = MString().sprintf("%03d", (int)(m_tv.tv_usec/1000));

    MString ret = format;
    ret.replace("yyyy", yyyy);
    ret.replace("MM", MM);
    ret.replace("dd", dd);
    ret.replace("hh", hh);
    ret.replace("mm", mm);
    ret.replace("ss", ss);
    ret.replace("ms", ms);

    return ret;
}

muint64 MDateTime::toMS()
{
    long long time_us = ((long long)m_tv.tv_sec) * 1000 *1000 +  m_tv.tv_usec;

    return time_us / 1000;
}

muint64 MDateTime::toSecond()
{
    return (muint64)m_tv.tv_sec;
}

mint64 MDateTime::currentSecond()
{
    time_t t = time(NULL);
    return (mint64)t;
}

MString MDateTime::formatCurrentDate()
{
    // clock time
    timeval tv;

    char temp[32];

    if (gettimeofday(&tv, NULL) == -1) {
        return MString(temp);
    }
    // to calendar time
    struct tm *tm;
    if ((tm = localtime(&tv.tv_sec)) == NULL) {
        return MString(temp);
    }
    sprintf(temp, "%d%02d%02d%02d%02d%02d",
             1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

    return MString(temp);
}

MDateTime MDateTime::currentDate()
{
    MDateTime dt;
    gettimeofday(&dt.m_tv, NULL);

    return dt;
}

