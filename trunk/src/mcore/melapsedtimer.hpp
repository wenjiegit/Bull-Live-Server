
/*
 * Copyright (C) wenjie.zhao
 */



#ifndef MELAPSEDTIMER_HPP
#define MELAPSEDTIMER_HPP

#include "mglobal.hpp"

struct timeval;

class MElapsedTimer
{
public:
    MElapsedTimer();

    /*!
        return the elapsed time in ms.
    */
    muint64 elapsed() const;

    /*!
        if the elapsed time exceed the timeout in ms.
    */
    bool hasExpired(muint64 timeout) const;

    /*!
        restart the timer.
        @return last elapsed time in ms.
    */
    muint64 restart();

    /*!
        start the timer.
    */
    void start();

private:
    struct timeval m_tv;
    bool m_hasStarted;
};

#endif // MELAPSEDTIMER_HPP
