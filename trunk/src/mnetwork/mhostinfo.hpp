
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MHOSTINFO_HPP
#define MHOSTINFO_HPP

#include <string>
#include <MStringList>

using namespace std;
class MTcpSocket;

/**
* @class MHostInfo
* @brief class of getting host info
* eg: get peer ip, get local ip, get ns...
*/
class MHostInfo
{
public:
    MHostInfo();
    ~MHostInfo();

    /**
    * @brief get the address of this class
    * @retval if MStringList is empty
    * then retrieve the merrno
    */
    MStringList address();
    int port();

public:
    /**
    * @brief get fd's peer info
    */
    static MHostInfo getPeerInfo(int fd);
    /**
    * @brief get fd's local info
    */
    static MHostInfo getLocalInfo(int fd);
    /**
    * @brief dns lookup
    * @return MHostInfo which has series of ips
    * if the MHostInfo.address() is empty,
    * then should retrieve the merrno
    */
    static MHostInfo lookupHost(const MString &host);

private:
    MStringList _address;
    int _port;
};

#endif // MHOSTINFO_HPP
