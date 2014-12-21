#ifndef MRTMPTIMESTAMPCORRECTOR_HPP
#define MRTMPTIMESTAMPCORRECTOR_HPP

class BlsRtmpMessage;

#include <MObject>

class BlsTimestampCorrector : public MObject
{
public:
    BlsTimestampCorrector(MObject *parent = 0);

    int correct(BlsRtmpMessage &msg, int tba = 0, int tbv = 0);
    mint64 getCurrentTime();
    void reset();

private:
    mint64 m_lastPktCorrectTime;
    mint64 m_lastPktTime;
};

#endif // MRTMPTIMESTAMPCORRECTOR_HPP
