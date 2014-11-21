#include "BlsStatistics.hpp"

#include <MHttpParser>
#include <MUrl>
#include <MLoger>
#include <MHttpResponseHeader>

BlsStatistics::BlsStatistics(MObject *parent)
    : MTcpServer(parent)
{
}

BlsStatistics::~BlsStatistics()
{

}

int BlsStatistics::newConnection(MTcpSocket *socket)
{
    BlsStatisticsClient *client = new BlsStatisticsClient(socket);
    return client->start();
}

BlsStatisticsClient::BlsStatisticsClient(MTcpSocket *socket, MObject *parent)
    : MHttpReactor(socket, parent)
{

}

BlsStatisticsClient::~BlsStatisticsClient()
{
    if (m_socket) m_socket->close();
}

int BlsStatisticsClient::run()
{
    if (MHttpReactor::run() != E_SUCCESS) {
        goto end;
    }

    {
        MString urlStr = "HTTP://" + m_parser->feild("Host") + m_parser->url();
        log_trace("--------------> %s ", urlStr.c_str());

        MHttpResponseHeader header;

    }

end:
    deleteLater();
    return E_SUCCESS;
}
