
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MDATE_HPP
#define MDATE_HPP

#include "mstring.hpp"

struct timeval;

class MDateTime
{
public:
    MDateTime();
    ~MDateTime();

    // currently only supported yyyy MM dd hh mm ss ms
    // eg. 2014-03-30 12:12:34.560
    MString toString(const MString &format);
    muint64 toMS();
    muint64 toSecond();

public:
    static mint64 currentSecond();

    //yyyyMMddhhmmss
    static MString formatCurrentDate();

    static MDateTime currentDate();

private:
    struct timeval m_tv;
};

#endif //MDATE_HPP
