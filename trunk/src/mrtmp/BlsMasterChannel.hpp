#ifndef BLSMASTERCHANNEL_HPP
#define BLSMASTERCHANNEL_HPP

#include <MTcpServer>
#include <MHash>

class BlsChild;

class BlsMasterChannel : public MTcpServer
{
public:
    BlsMasterChannel(MObject *parent = 0);
    virtual ~BlsMasterChannel();

    virtual int newConnection(MTcpSocket *socket);
};

class BlsChild : public MThread
{
public:
    BlsChild(MTcpSocket *socket, MObject *parent = 0);
    virtual ~BlsChild();

    virtual int run();

    inline int internalPort() { return m_internalPort; }

    int sendLine(const MString &commad, const MString &data);

    int send(const MString &data);

private:
    int processCommand(MString &line);

private:
    MTcpSocket *m_socket;
    int m_pid;
    int m_internalPort;
};

#endif // BLSMASTERCHANNEL_HPP
