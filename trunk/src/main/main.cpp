#include <iostream>
#include <MCoreApplication>

#include "mrtmpserver.hpp"

int main(int argc, char *argv[])
{
    MCoreApplication app(argc, argv);

    MRtmpServer server;
    server.listen("", 1935);

    return app.exec();
}
