#ifndef BLSSTATISTICS_HPP
#define BLSSTATISTICS_HPP

#include <MTcpServer>
#include <MHttpReactor>

class BlsStatistics : public MTcpServer
{
public:
    BlsStatistics(MObject *parent = 0);
    virtual ~BlsStatistics();

    virtual int newConnection(MTcpSocket *socket);
};

class BlsStatisticsClient : public MHttpReactor
{
public:
    BlsStatisticsClient(MTcpSocket *socket, MObject *parent = 0);
    virtual ~BlsStatisticsClient();

    virtual int run();
};

#endif // BLSSTATISTICS_HPP
