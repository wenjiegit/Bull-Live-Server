#ifndef MFLVRECODER_HPP
#define MFLVRECODER_HPP

#include <MObject>

#include "mrtmpprotocol.hpp"

class MFile;

class MFlvRecoder : public MObject
{
public:
    MFlvRecoder(MObject *parent = 0);

    void setFileName(const MString &name);
    int start();

    virtual int onMessage(MRtmpMessage *msg);

private:
    int onVideo(MRtmpMessage *msg);
    int onAudio(MRtmpMessage *msg);
    int onMetadata(MRtmpMessage *msg);
    int onOther(MRtmpMessage *msg);

private:
    MString m_fileName;
    MFile *m_file;
};

#endif // MFLVRECODER_HPP
