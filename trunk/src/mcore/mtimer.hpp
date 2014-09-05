
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MTIMER_HPP
#define MTIMER_HPP

#include "mobject.hpp"
#include "mcoreapplication.hpp"

class MTimer : public MObject
{
    DECLARE_FRIEND_CLASS(MCoreApplication)
public:
    MTimer(MObject *obj, TimerHandle h, MObject *parent = 0);
    ~MTimer();

    void start();
    void start(int msec);
    void stop();
    void setInterval(int msec);
    void setSingleShot(bool _singleShot);
    int  elapsedMsecond();

private:
    void timeout();
    void elapsed(int msec);

private:
    int interval;
    bool singleShot;
    int elapsedTime;
    TimerHandle handle;
    MObject *timedObj;
    int m_elapsed;
};

#endif // MTIMER_HPP
