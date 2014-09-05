
/*
 * Copyright (C) wenjie.zhao
 */



#include "melapsedtimer.hpp"

#include <sys/time.h>

MElapsedTimer::MElapsedTimer()
    : m_hasStarted(false)
{
}

muint64 MElapsedTimer::elapsed() const
{
    muint64 ret = 0;
    if (m_hasStarted) {
        struct timeval currentTv;
        gettimeofday(&currentTv, NULL);

       muint64 timeCurrent = ((muint64)currentTv.tv_sec) * 1000 +  currentTv.tv_usec/1000;
       muint64 timeTv = ((muint64)m_tv.tv_sec) * 1000 +  m_tv.tv_usec/1000;

        ret = timeCurrent - timeTv;
    }

    return ret;
}

bool MElapsedTimer::hasExpired(muint64 timeout ) const
{
    return elapsed() >= timeout;
}

muint64 MElapsedTimer::restart()
{
    muint64 lastElapsed = elapsed();
    start();

    return lastElapsed;
}

void MElapsedTimer::start()
{
    gettimeofday(&m_tv, NULL);
    m_hasStarted = true;
}
