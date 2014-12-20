
/*
 * Copyright (C) wenjie.zhao
 */


#include "mthread.hpp"

MThread::MThread(MObject *parent)
    : MObject(parent)
    , stid(NULL)
    , RequestStop(false)
    , m_isRunning(false)
{
}

MThread::~MThread()
{
    wait();
}

int MThread::start()
{
    if (!stid) {
        stid = st_thread_create(entry, this, Unjoinable, 0);
    }

    return E_SUCCESS;
}

void MThread::stop()
{
    RequestStop = true;
}

bool MThread::isRunning()
{
    return m_isRunning;
}

bool MThread::wait()
{
    while (isRunning()) {
        mMSleep(100);
    }

//    if (stid) {
//        st_thread_join(stid, NULL);
//    }

    return true;
}

void *MThread::entry(void *arg)
{
    MThread *this_ptr = (MThread *)arg;

    if (!this_ptr) return NULL;

    this_ptr->m_isRunning = true;
    this_ptr->run();
    this_ptr->m_isRunning = false;

    return NULL;
}
