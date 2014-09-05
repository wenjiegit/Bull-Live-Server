
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MThread_HPP
#define MThread_HPP

#include <st.h>
#include "mobject.hpp"

class MThread : public MObject
{
public:
    MThread(MObject *parent = 0);
    virtual ~MThread();

    int start();
    void stop();
    bool isRunning();
    virtual int run() = 0;

    enum {
        Unjoinable = 0,
        Joinable,
    };
    bool wait();

private:
    static void *entry(void *arg);

protected:
    st_thread_t stid;
    bool RequestStop;
    bool m_isRunning;
};

#endif // MThread_HPP
