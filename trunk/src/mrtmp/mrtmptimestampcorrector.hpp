#ifndef MRTMPTIMESTAMPCORRECTOR_HPP
#define MRTMPTIMESTAMPCORRECTOR_HPP

class MRtmpMessage;

#include <MObject>

class MRtmpTimestampCorrector : public MObject
{
public:
    MRtmpTimestampCorrector(MObject *parent = 0);

    int correct(MRtmpMessage &msg, int tba = 0, int tbv = 0);
    mint64 getCurrentTime();
    void reset();

private:
    mint64 m_lastPktCorrectTime;
    mint64 m_lastPktTime;
};

#endif // MRTMPTIMESTAMPCORRECTOR_HPP
