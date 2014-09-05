
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MPROCESS_HPP
#define MPROCESS_HPP

#include "mthread.hpp"
#include <unistd.h>

typedef void (*stdoutHander)(const MString& msg);
typedef void (*stderrHander)(const MString& msg);

class MProcess : public MThread
{
public:
    MProcess(MObject *parent = 0);
    ~MProcess();

    enum {
        NotRunning,
        Running,
        Stopped,
        Killed
    };

    /*!
        use this thread to read program's stdout and stdin msg.
    */
    int run();

    virtual void onStdoutMessage(const MString &msg);
    virtual void onFinished() {}
    virtual void onStart() {}

public:
    /*!
        start a \a program with \a rguments
        \code eg:
        MProcess a;
        MStringList args;
        args << "-l" << "-a";
        a.start("/bin/ls", args);
    */
    void start(const MString & program, const MStringList & arguments);
    void kill();

    /*!
        write msg to program.
    */
    int write(const MString &msg);
    int write(const char *msg);
    int write(const char *msg, int msgSize);

    inline int status() { return m_status; }

protected:
    pid_t m_pid;

private:
    int openPipe();

private:
    int m_stdoutPipe[2];
    int m_stderrPipe[2];
    int m_stdinPipe[2];

    int m_master;
    int m_slave;
    int m_status;
};

#endif // MPROCESS_HPP
