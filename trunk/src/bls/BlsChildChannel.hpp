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

    int init();
    virtual int run();

    // for write info to file
    virtual void timerA();

    int readMsg();

    /*!
        used to check if exist the url between multi-process.
        @url the url that will be checked.
    */
    int checkSameStream(const MString &url, bool &res);

    /*!
        inform master the @url will be published.
    */
    int informStreamPublished(const MString &url);

    /*!
        inform master the @url will be unpublished.
    */
    int informStreamUnPublished(const MString &url);

private:
    int clean();

private:
    MTcpSocket *m_socket;
    list<MString> m_cmds;
    MString m_lineCache;
    MTimer *m_timer;
};

extern BlsChildChannel *g_cchannel;

#endif // BLSCHILDCHANNEL_HPP
