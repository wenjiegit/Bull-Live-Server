
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MCOREAPPLICATION_H
#define MCOREAPPLICATION_H

#include "mglobal.hpp"
#include "mobject.hpp"
#include "mtimer.hpp"

#include <list>
#include <map>

using namespace std;
class MTimer;
typedef list<MTimer *> TimerList;
typedef list<MTimer *>::iterator TimerListIter;

class MCoreApplication : public MObject
{
public:
    MCoreApplication(int argc, char *argv[]);

    int exec();
    static void quit();
    static void deleteLater(MObject *obj);
    static void regTimer(MTimer *timer);
    static void removeTimer(MTimer *timer);
    void setProcTitle(const MString &title);
    void setUserData(void *data);
    void * userData();

    static MString applicationDirPath();
    static MString applicationFilePath();

private:
    map<MObject*, bool> deleteLaterObjs;
    map<MTimer*, bool> timerList;

    int m_argc;
    char **m_argv;
    char *argvLast;
    void *m_userData;
};

extern "C" {
    MCoreApplication *mApp();
}

#endif // MCOREAPPLICATION_H
