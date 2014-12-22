#ifndef BLSUTILS_HPP
#define BLSUTILS_HPP

#include <MString>
#include <MTcpSocket>
#include <MStream>

#define BLS_RTMP_DEFAULT_PORT           (1935)
#define BLS_DEFAULT_VHOST               ("__defaultVhost__")

// process role
#define Process_Role_Worker             (0x01)
#define Process_Role_Master             (0x02)
#define Process_Role_BackSource         (0x03)

/*!
    Internal Msg format:
    msg header size (4 bytes) + msg header +  msg body size (4 bytes) + msg body

    @note key must be string type
    value could be any type, which is binary data.
*/

// msg types
#define MSG_IF_EXIST_SAME_STREAM                ("_ExistSameStream")
#define MSG_STREAM_PUBLISHED                    ("_StreamPublished")
#define MSG_STREAM_UNPUBLISHED                  ("_StreamUnPublished")

// ret msg type and result
#define MSG_RESULT                              ("_Result")

// respons from master, which is body data.
#define MSG_RESULT_SUCESS                       ("_Sucess")
#define MSG_RESULT_FAILED                       ("_Failed")

class BlsInternalMsg
{
public:
    BlsInternalMsg();

    MString header() const;
    void setHeader(const MString &header);

    MString body() const;
    void setBody(const MString &body);

    /*!
        encode header and body to stream.
    */
    MStream encode() const;

    /*!
        print msg by format.
        key:value
    */
    void dump() const;

private:
    int m_headerSize;
    int m_bodySize;
    MString m_header;
    MString m_body;
};

extern "C" {
int readInternalMsg(BlsInternalMsg &msg, MTcpSocket *socket);
int writeInternalMsg(const BlsInternalMsg &msg, MTcpSocket *socket);
}

#endif // BLSUTILS_HPP
