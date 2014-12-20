
/*
 * Copyright (C) wenjie.zhao
 */


#include "mcoreapplication.hpp"

#include <string.h>
#include <sys/prctl.h>
#include <libgen.h>

static MCoreApplication *g_mApp = 0;
static bool loop_falg = true;

MCoreApplication::MCoreApplication(int argc, char *argv[])
    : m_argc(argc)
    , m_argv(argv)
    , m_userData(NULL)
{
    if (st_set_eventsys(ST_EVENTSYS_ALT)) {
        _err_exit(-1, "st_set_eventsys error.");
    }

    if (st_init()) {
        _err_exit(-1, "st_init error.");
    }

    if (g_mApp) {
        _err_exit(-1, "only one MCoreApplication instance can be build.");
    }

    g_mApp = this;

    argvLast = m_argv[0];
    for(int i = 0; i < argc; i++)
    {
        argvLast += strlen(argvLast) + 1;
    }
}

int MCoreApplication::exec()
{
    loop_falg = true;

    int sleepInterval = 200 * 1000; // 1000 * 200 = 200ms
    int sleepIntervalMs = sleepInterval / 1000;
    while (loop_falg) {
        // delete objs in queue
        // if objs is not empty
        while (!deleteLaterObjs.empty()) {
            map<MObject*, bool>::iterator iter = deleteLaterObjs.begin();
            MObject *obj = iter->first;
            deleteLaterObjs.erase(iter);
            delete obj;
        }

        // travel timer list
        map<MTimer*, bool>::iterator iter = timerList.begin();
        while (iter != timerList.end()) {
            MTimer *timer = iter->first;
            timer->elapsed(sleepIntervalMs);
            ++iter;
        }

        // TODO add other code here
        st_usleep(sleepInterval);
    }

    return 0;
}

void MCoreApplication::quit()
{
    loop_falg = false;
}

void MCoreApplication::deleteLater(MObject *obj)
{
    map<MObject*, bool> &objs = mApp()->deleteLaterObjs;
    map<MObject*, bool>::iterator iter = objs.find(obj);
    if (iter == objs.end()) {
        objs[obj] = true;
    }
}

void MCoreApplication::regTimer(MTimer *timer)
{
    mApp()->timerList[timer] = true;
}

void MCoreApplication::removeTimer(MTimer *timer)
{
    mApp()->timerList.erase(timer);
}

void MCoreApplication::setProcTitle(const MString &title)
{
    // for ps aux
    char* p = m_argv[0];

    int size = title.length();
    if(argvLast - p < size){
        size = argvLast - p;
    }

    memcpy(p, title.data(), size);

    p += title.length();;
    if(argvLast - p > 0){
        memset(p, ' ', argvLast - p);
    }
    *(argvLast - 1) = 0;

    // for ps -el
    // prctl(PR_SET_NAME, title.c_str());
}

void MCoreApplication::setUserData(void *data)
{
    m_userData = data;
}

void *MCoreApplication::userData()
{
    return m_userData;
}

MString MCoreApplication::applicationDirPath()
{
    MString filePath = applicationFilePath();
    if (filePath.empty()) {
        return "";
    }

    char *path = const_cast<char*>(filePath.c_str());
    return MString(dirname(path));
}

MString MCoreApplication::applicationFilePath()
{
#ifndef PATH_MAX_LEN
#define PATH_MAX_LEN 2048
#endif

    char path[PATH_MAX_LEN];
    memset(path, 0, PATH_MAX_LEN);

    MString exePath = MString().sprintf("/proc/%d/exe", (int)getpid());

    int len = readlink(exePath.c_str(), path, PATH_MAX_LEN);
    if (len < 0 || len >= PATH_MAX_LEN) {
        return "";
    }

    return MString(path);
}

MCoreApplication *mApp()
{
    if (!g_mApp) {
        _err_return(NULL, "need a MCoreApplication instance.");
    }

    return g_mApp;
}
