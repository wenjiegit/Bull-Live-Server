#ifndef BLSHTTPSERVICE_HPP
#define BLSHTTPSERVICE_HPP

#include <MTcpServer>
#include <MHttpReactor>

class BlsHttpService : public MTcpServer
{
public:
    BlsHttpService(MObject * parent = 0);
    ~BlsHttpService();

    int newConnection(MTcpSocket *socket);
};

class BlsHttpClient : public MHttpReactor
{
public:
    BlsHttpClient(MTcpSocket *socket, MObject *parent = 0);
    ~BlsHttpClient();

    int run();

private:
    int sendHttpLiveFlv(const MString & url);
    void clean();

private:
    MTcpSocket *m_socket;
};

#endif // BLSHTTPSERVICE_HPP
