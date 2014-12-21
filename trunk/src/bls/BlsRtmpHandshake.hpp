#ifndef MRTMPHANDSHKAE_HPP
#define MRTMPHANDSHKAE_HPP

class MTcpSocket;

class BlsRtmpHandshake
{
public:
    BlsRtmpHandshake();
    virtual ~BlsRtmpHandshake();
public:
    virtual int handshake_with_client(MTcpSocket& skt);
    /*
    * if c0 is 0x03 and c1[4] is not 0, use use_complex_response
    * if c0 is 0x06 or 0x08, use use_complex_response
    */
    virtual int use_complex_response(MTcpSocket& skt, char* _c1,bool encrypted);
    /*
    * if c0 is only 0x03 and c1[4] is 0, use simple handshark response
    */
    virtual int use_simple_response(MTcpSocket& skt,char* c0c1);
    /*
    * when use edge mode connect server use the two function
    */
    virtual int handshake_with_server_use_complex(MTcpSocket& skt,bool rtmpe,bool swf);
    virtual int handshake_with_server_use_simple(MTcpSocket& skt);
};

#endif  // MRTMPHANDSHKAE_HPP
