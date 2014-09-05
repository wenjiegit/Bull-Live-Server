
/*
 * Copyright (C) wenjie.zhao
 */


#include "mtimer.hpp"

#include "mcoreapplication.hpp"

MTimer::MTimer(MObject *obj, TimerHandle h, MObject *parent)
    : MObject(parent)
    , interval(0)
    , singleShot(false)
    , elapsedTime(0)
    , handle(h)
    , timedObj(obj)
    , m_elapsed(0)
{
}

MTimer::~MTimer()
{
    stop();
}

void MTimer::start()
{
    elapsedTime = interval;
    if (interval > 0) {
        mApp()->regTimer(this);
    }
}

void MTimer::start(int msec)
{
    elapsedTime = interval = msec;

    if (interval > 0) {
        mApp()->regTimer(this);
    }
}

void MTimer::stop()
{
    mApp()->removeTimer(this);
}

void MTimer::setInterval(int msec)
{
    interval = msec;
}

void MTimer::setSingleShot(bool _singleShot)
{
    singleShot = _singleShot;
}

int MTimer::elapsedMsecond()
{
    return m_elapsed;
}

void MTimer::timeout()
{
    m_elapsed += interval;

    (timedObj->*handle)();
}

void MTimer::elapsed(int msec)
{
    elapsedTime -= msec;
    if (elapsedTime <= 0) {
        timeout();
        // singleShot is true , then stop timer
        if (singleShot) {
            stop();
        } else {
            // singleShot is false, then restart timer
            elapsedTime = interval;
        }
    }
}
