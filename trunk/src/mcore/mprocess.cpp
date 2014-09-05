
/*
 * Copyright (C) wenjie.zhao
 */


#include "mprocess.hpp"

#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pty.h>
#include <utmp.h>

#include "mstringlist.hpp"
#include <mcoreapplication.hpp>

#include <iostream>

static const int MProcess_Buffer_Size = 1025;
static const int MProcess_Read_Timeout = 1000000;   // 1000 * 1000 us = 1000 ms = 1s

MProcess::MProcess(MObject *parent)
    : MThread(parent)
    , m_pid(-1)
    , m_status(NotRunning)
{
    m_master = -1;
    m_slave = -1;
}

MProcess::~MProcess()
{
}

int MProcess::run()
{
    st_netfd_t stfd = st_netfd_open_socket(m_master);
    if (!stfd) {
        return -1;
    }

    while (true) {
        // read stdout
        char buffer[MProcess_Buffer_Size];
        memset(buffer, '\0', MProcess_Buffer_Size);

        int count = st_read(stfd, buffer, MProcess_Buffer_Size - 1, MProcess_Read_Timeout);
        if (count == 0) {
            break;
        } else if (count < 0) {
            if (errno == ETIME) {
                continue;
            } else {
                break;
            }
        }

        onStdoutMessage(buffer);
        mMSleep(100);
    }
    m_status = Stopped;
    st_netfd_close(stfd);
    ::wait(0);
    onFinished();

    return 0;
}

void MProcess::onStdoutMessage(const MString &msg)
{
    M_UNUSED(msg);
}

void MProcess::start(const MString &program, const MStringList &arguments)
{
    openpty(&m_master, &m_slave, NULL, NULL, NULL);
    m_pid = fork();
    if (m_pid < 0) {
        return;
    }

    MStringList args = arguments;
    args.push_front(program);

    if (m_pid == 0) {
            login_tty(m_slave);
            close(m_master);

            char *argv[args.size() + 1];
            for (unsigned int i = 0; i < args.size(); ++i) {
                argv[i] = const_cast<char*>(args.at(i).c_str());
            }
            // append \0 to argv last
            argv[args.size()] = (char*)0;
            sleep(2);
            if (execvp(program.data(), argv) < 0) {
                _exit(-1);
            }
    }

    m_status = Running;
    close(m_slave);
    onStart();
    MThread::start();
}

void MProcess::kill()
{
    if (m_pid > 0) {
        ::kill(m_pid, SIGKILL);
    }
    m_status = Killed;
}

int MProcess::write(const MString &msg)
{
    return write(msg.data(), msg.size());
}

int MProcess::write(const char *msg)
{
    return write(msg, strlen(msg));
}

int MProcess::write(const char *msg, int msgSize)
{
    // TODO : add imp
    return 0;
}
