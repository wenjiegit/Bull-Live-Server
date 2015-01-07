#ifndef BLSHDS_HPP
#define BLSHDS_HPP

#include <MObject>
#include <MString>
#include <list>

using namespace std;

class BlsRtmpMessage;
class BlsSegment;

/*!
    @brief used to produce segment and f4m and bootstrap of HDS.
*/
class BlsHDS : public MObject
{
public:
    struct BlsHDSCtx
    {
        MString streamName;
        MString segmentPath;

        /*!
            the segment duration in milliseconds.
        */
        muint32 durationMs;

        /*!
            windowSize equal to the sum of all segment's time.
        */
        muint32 windowSize;
    };

public:
    BlsHDS(MObject *parent = 0);
    ~BlsHDS();

    void setCtx(const BlsHDSCtx &ctx);

    void onVideo(const BlsRtmpMessage &msg);
    void onAudio(const BlsRtmpMessage &msg);

    /*!
        release all segments and do some clean action.
    */
    void clean();

private:
    int flushBootStrap();
    int flushMainfest();

private:
    BlsHDSCtx m_ctx;
    bool m_invalid;
    list<BlsSegment *> m_segments;
    BlsSegment *m_currentSegment;
    int m_fragIndex;
};

#endif // BLSHDS_HPP
