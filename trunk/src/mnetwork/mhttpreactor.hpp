
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MHTTPREACTOR_HPP
#define MHTTPREACTOR_HPP

#include <MThread>

class MTcpSocket;
class MHttpResponseHeader;
class MHttpParser;
class MHttpHeader;

class MHttpReactor : public MThread
{
public:
    MHttpReactor(MObject *parent = 0);
    MHttpReactor(MTcpSocket *socket, MObject *parent = 0);

    virtual ~MHttpReactor();

    virtual int run();
    int response(const MHttpResponseHeader &header, const char *data = 0, muint32 length = 0);
    int response(const MHttpResponseHeader &header, const MString &data);
    int writeResponse(const MHttpHeader &header);
    MString readHeader();

    int post(const MString& path, const MString &data);
    MString get(const MString &urlStr);

    static int readHttpHeader(MHttpParser &parser, MString &body, MTcpSocket &socket);

protected:
    MTcpSocket *m_socket;
    MHttpParser *m_parser;
};

#endif // MHTTPREACTOR_HPP
