#ifndef MFLVRECODER_HPP
#define MFLVRECODER_HPP

#include <MObject>

#include "BlsRtmpProtocol.hpp"

class MFile;

class MFlvRecoder : public MObject
{
public:
    MFlvRecoder(MObject *parent = 0);

    void setFileName(const MString &name);
    int start();

    virtual int onMessage(BlsRtmpMessage *msg);

private:
    int onVideo(BlsRtmpMessage *msg);
    int onAudio(BlsRtmpMessage *msg);
    int onMetadata(BlsRtmpMessage *msg);
    int onOther(BlsRtmpMessage *msg);

private:
    MString m_fileName;
    MFile *m_file;
};

#endif // MFLVRECODER_HPP
