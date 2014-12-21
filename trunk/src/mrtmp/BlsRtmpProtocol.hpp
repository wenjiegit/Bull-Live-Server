#ifndef MRTMPPROTOCOL_HPP
#define MRTMPPROTOCOL_HPP

#include <MStream>
#include <MHash>
#include <MObject>

#include "BlsAMF0Serializer.hpp"

// Protocol Control Messages

#define RTMP_MSG_SetChunkSize                   0x01
#define RTMP_MSG_AbortMessage                   0x02
#define RTMP_MSG_Acknowledgement                0x03
#define RTMP_MSG_UserControlMessage             0x04
#define RTMP_MSG_WindowAcknowledgementSize      0x05
#define RTMP_MSG_SetPeerBandwidth               0x06
#define RTMP_MSG_EdgeAndOriginServerCommand     0x07

enum
{
    UCM_StreamBegin             = 0x00,
    UCM_StreamEOF               = 0x01,
    UCM_StreamDry               = 0x02,
    UCM_SetBufferLength         = 0x03,     // 8bytes event-data = 4 bytes' stream id + 4 bytes'
    UCM_StreamIsRecorded        = 0x04,
    UCM_PingRequest             = 0x06,
    UCM_PingResponse            = 0x07,
    UCM_Request_Verfify         = 0x1a,
    UCM_Response_Verfify        = 0x1b,
    UCM_Buffer_Empty            = 0x1f,
    UCM_Buffer_Ready            = 0x20
};

enum {
    SetPeerBandwidth_Type_Hard       = 0,
    SetPeerBandwidth_Type_Soft       = 1,
    SetPeerBandwidth_Type_Dynamic    = 2
};

// Command message
#define RTMP_MSG_AMF3CommandMessage             17  // 0x11
#define RTMP_MSG_AMF0CommandMessage             20  // 0x14

// Data message
#define RTMP_MSG_AMF0DataMessage                18  // 0x12
#define RTMP_MSG_AMF3DataMessage                15  // 0x0F

// Shared object message
#define RTMP_MSG_AMF3SharedObject               16  // 0x10
#define RTMP_MSG_AMF0SharedObject               19  // 0x13

// audio message
#define RTMP_MSG_AudioMessage                   8   // 0x08

// Video message
#define RTMP_MSG_VideoMessage                   9   // 0x09

// Aggregate message
#define RTMP_MSG_AggregateMessage               22  // 0x16

// chunk type
#define RTMP_FMT_TYPE0                          0
#define RTMP_FMT_TYPE1                          1
#define RTMP_FMT_TYPE2                          2
#define RTMP_FMT_TYPE3                          3

// chunk size
#define RTMP_DEFAULT_CHUNK_SIZE                 128
#define RTMP_MIN_CHUNK_SIZE                     128
#define RTMP_MAX_CHUNK_SIZE                     65536

// ext ts
#define RTMP_EXTENDED_TIMESTAMP                 0xFFFFFF

// amf0 command message
#define RTMP_AMF0_COMMAND_CONNECT               "connect"
#define RTMP_AMF0_COMMAND_CREATE_STREAM         "createStream"
#define RTMP_AMF0_COMMAND_CLOSE_STREAM          "closeStream"
#define RTMP_AMF0_COMMAND_PLAY                  "play"
#define RTMP_AMF0_COMMAND_PAUSE                 "pause"
#define RTMP_AMF0_COMMAND_ON_BW_DONE            "onBWDone"
#define RTMP_AMF0_COMMAND_ON_STATUS             "onStatus"
#define RTMP_AMF0_COMMAND_RESULT                "_result"
#define RTMP_AMF0_COMMAND_ERROR                 "_error"
#define RTMP_AMF0_COMMAND_RELEASE_STREAM        "releaseStream"
#define RTMP_AMF0_COMMAND_FC_PUBLISH            "FCPublish"
#define RTMP_AMF0_COMMAND_UNPUBLISH             "FCUnpublish"
#define RTMP_AMF0_COMMAND_PUBLISH               "publish"
#define RTMP_AMF0_DATA_SAMPLE_ACCESS            "|RtmpSampleAccess"
#define RTMP_AMF0_DATA_SET_DATAFRAME            "@setDataFrame"
#define RTMP_AMF0_DATA_ON_METADATA              "onMetaData"

/**
* the chunk stream id used for some under-layer message,
* for example, the PC(protocol control) message.
*/
#define RTMP_CID_ProtocolControl 0x02

/**
* the AMF0/AMF3 command message, invoke method and return the result, over NetConnection.
* generally use 0x03.
*/
#define RTMP_CID_OverConnection 0x03

/**
* the AMF0/AMF3 command message, invoke method and return the result, over NetConnection,
* the midst state(we guess).
* rarely used, e.g. onStatus(NetStream.Play.Reset).
*/
#define RTMP_CID_OverConnection2 0x04

/**
* the stream message(amf0/amf3), over NetStream.
* generally use 0x05.
*/
#define RTMP_CID_OverStream 0x05

/**
* the stream message(amf0/amf3), over NetStream, the midst state(we guess).
* rarely used, e.g. play("mp4:mystram.f4v")
*/
#define RTMP_CID_OverStream2 0x08

/**
* the stream message(video), over NetStream
* generally use 0x06.
*/
#define RTMP_CID_Video 0x06

/**
* the stream message(audio), over NetStream.
* generally use 0x07.
*/
#define RTMP_CID_Audio 0x07

#define RTMP_SIG_FMS_VER                            "3,5,3,888"
#define RTMP_SIG_AMF0_VER                           0
#define RTMP_SIG_CLIENT_ID                          "ASAICiss"

#define STATUS_LEVEL                                "level"
#define STATUS_CODE                                 "code"
#define STATUS_DESC                                 "description"
#define STATUS_DETAILS                              "details"
#define STATUS_CLIENT_ID                            "clientid"

// status value
#define STATUS_LEVEL_STATUS                         "status"
#define STATUS_LEVEL_WARNING                        "warning"
#define STATUS_LEVEL_ERROR                          "error"

// code value
#define NetConnection_Connect_Success               "NetConnection.Connect.Success"
#define NetConnection_Connect_Rejected              "NetConnection.Connect.Rejected"

#define NetStream_Play_Reset                        "NetStream.Play.Reset"
#define NetStream_Play_Start                        "NetStream.Play.Start"
#define NetStream_Pause_Notify                      "NetStream.Pause.Notify"
#define NetStream_Unpause_Notify                    "NetStream.Unpause.Notify"
#define NetStream_Publish_Start                     "NetStream.Publish.Start"
#define NetStream_Data_Start                        "NetStream.Data.Start"
#define NetStream_Unpublish_Success                 "NetStream.Unpublish.Success"

#define NetStream_Play_StreamNotFound               "NetStream.Play.StreamNotFound"

// FMLE
#define RTMP_AMF0_COMMAND_ON_FC_PUBLISH             "onFCPublish"
#define RTMP_AMF0_COMMAND_ON_FC_UNPUBLISH           "onFCUnpublish"

class MTcpSocket;
class BlsRtmpUrl;

struct BlsRtmpMessageHeader
{
    BlsRtmpMessageHeader()
    {
        type = 0;
        payloadLength = 0;
        timestampDelta = 0;
        streamID = 0;
        timestamp = 0;
        perfer_cid = 0;
    }

    BlsRtmpMessageHeader(mint8 tp, mint32 cid)
        : type(tp)
        , perfer_cid(cid)
    {
        payloadLength = 0;
        timestampDelta = 0;
        streamID = 0;
        timestamp = 0;
    }

    mint8   type;
    mint32  payloadLength;
    mint32  timestampDelta;
    mint32  streamID;
    muint32 timestamp;
    mint32  perfer_cid;
};

class BlsRtmpMessage
{
public:
    BlsRtmpMessage();
    virtual ~BlsRtmpMessage();

    inline bool isAudio() {return header.type == RTMP_MSG_AudioMessage;}
    inline bool isVideo() {return header.type == RTMP_MSG_VideoMessage;}
    inline bool isAmf0Command() {return header.type == RTMP_MSG_AMF0CommandMessage;}
    inline bool isAmf0Data() {return header.type == RTMP_MSG_AMF0DataMessage;}
    inline bool isAmf3Command() {return header.type == RTMP_MSG_AMF3CommandMessage;}
    inline bool isAmf3Data() {return header.type == RTMP_MSG_AMF3DataMessage;}
    inline bool isWindowAckledgementSize() {return header.type == RTMP_MSG_WindowAcknowledgementSize;}
    inline bool isAckledgement() {return header.type == RTMP_MSG_Acknowledgement;}
    inline bool isSetChunkSize() {return header.type == RTMP_MSG_SetChunkSize;}
    inline bool isUserControl() {return header.type == RTMP_MSG_UserControlMessage;}
    inline bool isSetPeerBandwidth() {return header.type == RTMP_MSG_SetPeerBandwidth;}
    inline bool isAggregate() {return header.type == RTMP_MSG_AggregateMessage;}

public:
    BlsRtmpMessageHeader header;
    MStream payload;
};

struct BlsRtmpChunkStream
{
    char                fmt;
    int                 cid;
    BlsRtmpMessageHeader  header;
    bool                hasExtendedTimestamp;
    BlsRtmpMessage*       msg;
    muint32             msg_count;

    BlsRtmpChunkStream(int _cid)
    {
        fmt = -1;
        cid = _cid;
        hasExtendedTimestamp = false;
        msg = NULL;
        msg_count = 0;
    }
};

class BlsRtmpNetStatusEvent : public BlsAMF0Object
{
public:
    BlsRtmpNetStatusEvent(const MString &code = "", const MString &level = "");
    ~BlsRtmpNetStatusEvent();
};

class BlsRtmpProtocolAbstract
{
public:
    virtual ~BlsRtmpProtocolAbstract() {}

    virtual int onCommand(BlsRtmpMessage *msg, const MString &name, double transactionID, BlsAMF0Any *arg1
                          , BlsAMF0Any *arg2 = NULL, BlsAMF0Any *arg3 = NULL, BlsAMF0Any *arg4 = NULL)
    {
        M_UNUSED(msg);
        M_UNUSED(name);
        M_UNUSED(transactionID);
        M_UNUSED(arg1);
        M_UNUSED(arg2);
        M_UNUSED(arg3);
        M_UNUSED(arg4);
        return E_SUCCESS;
    }

    virtual int onAudio(BlsRtmpMessage *msg)
    {
        M_UNUSED(msg);
        return E_SUCCESS;
    }

    virtual int onVideo(BlsRtmpMessage *msg)
    {
        M_UNUSED(msg);
        return E_SUCCESS;
    }

    virtual int onMetadata(BlsRtmpMessage *msg)
    {
        M_UNUSED(msg);
        return E_SUCCESS;
    }
};

class BlsRtmpContext
{
public:
    BlsRtmpContext();
    ~BlsRtmpContext();

    void setTcUrl(const MString &tu);
    void setStreamName(const MString &name);
    MString url();

    int windowAcknowledgementSize;
    int streamID;
    int outChunkSize;
    int inChunkSize;
    int bufferLength;
    mint64 ackedSize;

    BlsRtmpUrl *rtmpUrl;
    MString tcUrl;
};

class BlsRtmpProtocol : public MObject
{
public:
    BlsRtmpProtocol(MTcpSocket *socket, MObject *parent = 0);

    int recv_message(BlsRtmpMessage **pmsg);
    int send_message(BlsRtmpMessage *msg);

    void setSession(BlsRtmpProtocolAbstract *session);

    // rtmp functions
    int onConnect(double id);
    int setChunkSize(int chunkSize);
    int setPeerBandwidth(mint32 bandwidth, mint8 type = SetPeerBandwidth_Type_Dynamic);
    int onBWDone();
    int setAckSize(mint32 size);
    int setUCM(int type, int eventData1 = -1, int eventData2 = -1);

    /*!
        client method to tell server to create stream id.
        the stream id will be retured in _result.
    */
    int createStream();

    /*!
        client method to tell server begin to publish @streamName
        with @transactionId
    */
    int publishStream(double transactionId, const MString &streamName);

    int sendAny(const BlsRtmpMessageHeader &header, BlsAMF0Any *arg1, BlsAMF0Any *arg2 = NULL, BlsAMF0Any *arg3 = NULL
            , BlsAMF0Any *arg4 = NULL, BlsAMF0Any *arg5 = NULL, BlsAMF0Any *arg6 = NULL);
    int sendNetStatusEvent(double transactionID, BlsRtmpNetStatusEvent *event);

    BlsRtmpContext *getRtmpCtx();

    int handshakeWithClient();
    int handshakeWithServer(bool useComplex = false);

private:
    int recv_interlaced_message(BlsRtmpMessage** pmsg);
    int read_basic_header(char& fmt, int& cid, int& bh_size);
    int read_message_header(BlsRtmpChunkStream* chunk, char fmt, int bh_size, int& mh_size);
    int read_message_payload(BlsRtmpChunkStream* chunk, int bh_size, int mh_size, int& payload_size, BlsRtmpMessage** pmsg);
    int on_recv_message(BlsRtmpMessage* msg);

private:
    int fillStream(int size, MStream &stream);
    int responeAck();
    BlsRtmpMessage *reEncodeMetadata(BlsRtmpMessage *metadata);

private:
    static int decodeAny(MStream &stream, BlsAMF0Any **arg1, BlsAMF0Any **arg2, BlsAMF0Any **arg3, BlsAMF0Any **arg4, BlsAMF0Any **arg5, BlsAMF0Any **arg6);
    static int encodeAny(MStream &stream, BlsAMF0Any *arg1, BlsAMF0Any *arg2, BlsAMF0Any *arg3, BlsAMF0Any *arg4, BlsAMF0Any *arg5, BlsAMF0Any *arg6);

private:
    MHash<int, BlsRtmpChunkStream*> m_chunks;
    MStream m_chunkHeader;
    MTcpSocket *m_socket;
    BlsRtmpContext m_rtmpCtx;

    BlsRtmpProtocolAbstract *m_session;
};

#endif // MRTMPPROTOCOL_HPP
