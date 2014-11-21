#ifndef BLSCHILDCHANNEL_HPP
#define BLSCHILDCHANNEL_HPP

#include <MThread>
#include <list>

using namespace std;

class MTcpSocket;
class MTimer;

class BlsChildChannel : public MThread
{
public:
    BlsChildChannel(MObject *parent = 0);
    virtual ~BlsChildChannel();

    int start();
    virtual int run();

    int sendLineAndWaitResponse(const MString &commad, const MString &data, MString &response);
    int sendLine(const MString &commad, const MString &data);
    int send(const MString &data);

    // for write info to file
    virtual void timerA();

private:
    int clean();
    int processLine(const MString &line);

private:
    MTcpSocket *m_socket;
    list<MString> m_cmds;
    MString m_lineCache;
    MTimer *m_timer;
};

extern BlsChildChannel *g_cchannel;

#endif // BLSCHILDCHANNEL_HPP
