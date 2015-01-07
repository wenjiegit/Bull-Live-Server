#include "BlsHDS.hpp"

#include "BlsRtmpProtocol.hpp"

#include <MLoger>
#include <MStream>
#include <MFile>

static void update_box(MStream &stream)
{
    char *start = (char*)stream.data();
    int size = stream.size();

    char *p_size = (char*)&size;
    start[0] = p_size[3];
    start[1] = p_size[2];
    start[2] = p_size[1];
    start[3] = p_size[0];
}

class BlsSegment
{
public:
    BlsSegment();
    ~BlsSegment();

    void onVideo(const BlsRtmpMessage &msg);
    void onAudio(const BlsRtmpMessage &msg);

    /*!
        flush data to disk.
    */
    int flush();

    /*!
        calc the segment duration in milliseconds.
        @return 0 if no msgs
                or the last msg dts minus the first msg dts.
    */
    int duration();

    /*!
        set/get index
    */
    inline void setIndex(int index) { m_index = index; }
    inline int index() { return m_index; }

    /*!
        set/get start time
    */
    inline void setStartTime(mint64 startTime) { m_startTime = startTime; }
    inline mint64 startTime() { return m_startTime; }

private:
    list<BlsRtmpMessage> m_msgs;

    /*!
        the index of this fragment
    */
    int m_index;
    mint64 m_startTime;
};

BlsSegment::BlsSegment()
{

}

BlsSegment::~BlsSegment()
{

}

void BlsSegment::onVideo(const BlsRtmpMessage &msg)
{

}

void BlsSegment::onAudio(const BlsRtmpMessage &msg)
{

}

int BlsSegment::flush()
{
    return E_SUCCESS;
}

int BlsSegment::duration()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
BlsHDS::BlsHDS(MObject *parent)
    : MObject(parent)
    , m_invalid(false)
    , m_currentSegment(NULL)
    , m_fragIndex(1)
{

}

BlsHDS::~BlsHDS()
{

}

void BlsHDS::setCtx(const BlsHDS::BlsHDSCtx &ctx)
{
    m_ctx = ctx;
    flushMainfest();
}

void BlsHDS::onVideo(const BlsRtmpMessage &msg)
{
    if (!m_invalid) {
        log_warn("HDS can not start, ignore video msg.");
        return;
    }

    if (!m_currentSegment) {
        m_currentSegment = new BlsSegment;
        m_currentSegment->setIndex(m_fragIndex++);
    }

    m_currentSegment->onVideo(msg);

    if (m_currentSegment->duration() >= m_ctx.durationMs) {
        // flush segment
        if (m_currentSegment->flush() != E_SUCCESS) {
            log_error("flush segment failed.");
            m_invalid = false;
            return;
        }

        log_trace("flush Segment success.");

        // flush bootstrap
        if (flushBootStrap() != E_SUCCESS) {
            log_error("flush bootstrap failed.");
            m_invalid = false;
            return;
        }

        log_trace("flush BootStrap success.");
    }
}

void BlsHDS::onAudio(const BlsRtmpMessage &msg)
{
    if (!m_invalid) {
        log_warn("HDS can not start, ignore audio msg.");
        return;
    }

    if (!m_currentSegment) {
        m_currentSegment = new BlsSegment;
    }

    m_currentSegment->onAudio(msg);

    if (m_currentSegment->duration() >= m_ctx.durationMs) {
        // flush segment
        if (m_currentSegment->flush() != E_SUCCESS) {
            log_error("flush segment failed.");
            m_invalid = false;
            return;
        }

        log_trace("flush Segment success.");
        // reset the current segment
        m_segments.push_back(m_currentSegment);
        m_currentSegment = NULL;

        // flush bootstrap
        if (flushBootStrap() != E_SUCCESS) {
            log_error("flush bootstrap failed.");
            m_invalid = false;
            return;
        }

        log_trace("flush BootStrap success.");
    }
}

void BlsHDS::clean()
{

}

/*!
    Note, the .abst files need to be served with the "binary/octet"
    mime type, otherwise at least the OSMF player can easily fail
    with "stream not found" when polling for the next fragment.

    @see ffmpeg src   libavformat/hdsenc.c
*/
int BlsHDS::flushBootStrap()
{
    MStream abst;

    // @see video_file_format_spec_v10_1
    // page: 46
    abst.write4Bytes(0);
    abst.writeString("abst");
    abst.write1Bytes(0x00);           // Either 0 or 1
    abst.write3Bytes(0x00);           // Flags always 0
    /*!
        @BootstrapinfoVersion       UI32
        The version number of the bootstrap information.
        When the Update field is set, BootstrapinfoVersion
        indicates the version number that is being updated.
        we assume this is the last.
    */
    abst.write4Bytes(m_fragIndex - 1);            // BootstrapinfoVersion

    abst.write1Bytes(0x20);                       // profile, live, update
    abst.write4Bytes(1000);                       // TimeScale Typically, the value is 1000, for a unit of milliseconds

    /*!
        The timestamp in TimeScale units of the latest available Fragment in the media presentation.
        This timestamp is used to request the right fragment number.
        The CurrentMedia Time can be the total duration.
        For media presentations that are not live, CurrentMediaTime can be 0.
    */
    BlsSegment *st = m_segments.back();
    abst.write8Bytes(st->startTime());

    /*!
        @MovieIdentifier        STRING
        The identifier of this presentation.
        we write null string.
    */
    abst.write1Bytes(0);

    /*!
        @ServerEntryCount       UI8
        The number of ServerEntryTable entries.
        The minimum value is 0.
    */
    abst.write1Bytes(0);

    /*!
        @ServerEntryTable
        because we write 0 of ServerEntryCount, so this feild is ignored.
    */

    /*!
        @QualityEntryCount      UI8
        The number of QualityEntryTable entries, which is
        also the number of available quality levels. The
        minimum value is 0. Available quality levels are for,
        for example, multi bit rate files or trick files.
    */
    abst.write1Bytes(0);

    /*!
        @QualityEntryTable
        because we write 0 of QualityEntryCount, so this feild is ignored.
    */

    /*!
        @DrmData        STRING
        Null or null-terminated UTF-8 string.  This string
        holds Digital Rights Management metadata.
        Encrypted files use this metadata to get the
        necessary keys and licenses for decryption and play back.
        we write null string.
    */
    abst.write1Bytes(0);

    /*!
        @MetaData       STRING
        Null or null-terminated UTF - 8 string that holds metadata.
        we write null string.
    */
    abst.write1Bytes(0);

    /*!
        @SegmentRunTableCount       UI8
        The number of entries in SegmentRunTableEntries.
        The minimum value is 1. Typically, one table
        contains all segment runs. However, this count
        provides the flexibility to define the segment runs
        individually for each quality level (or trick file).
    */
    abst.write1Bytes(1);

    MStream asrt;
    // follows by asrt
    asrt.write4Bytes(0);
    asrt.writeString("asrt");

    /*!
        @Version        UI8
        @Flags          UI24
    */
    asrt.write4Bytes(0);

    /*!
        @QualityEntryCount      UI8
        The number of QualitySegmen tUrlModifiers
        (quality level references) that follow. If 0, this
        Segment Run Table applies to all quality levels,
        and there shall be only one Segment Run Table
        box in the Bootstrap Info box.
    */
    asrt.write1Bytes(0);

    /*!
        @QualitySegmentUrlModifiers
        ignored.
    */

    /*!
        @SegmentRunEntryCount
        The number of items in this
        SegmentRunEn tryTable. The minimum value is 1.
    */
    asrt.write1Bytes(1);

    /*!
        @SegmentRunEntryTable
    */
    for  (int i = 0; i < 1; ++i) {
        /*!
            @FirstSegment       UI32
            The identifying number of the first segment in the run of
            segments containing the same number of fragments.
            The segment corresponding to the FirstSegment in the next
            SEGMENTRUNEN TRY will terminate this run.
        */
        asrt.write4Bytes(1);

        /*!
            @FragmentsPerSegment        UI32
            The number of fragments in each segment in this run.
        */
        asrt.write4Bytes(m_fragIndex - 1);
    }

    update_box(asrt);
    abst.append(asrt);

    /*!
        @FragmentRunTableCount      UI8
        The number of entries in FragmentRunTable-Entries.
        The min i mum value is 1.
    */
    abst.write1Bytes(1);

    MStream afrt;
    // follows by afrt
    afrt.write4Bytes(0);
    afrt.writeString("afrt");

    /*!
        @Version        UI8
        @Flags          UI24
    */
    afrt.write4Bytes(0);

    /*!
        @TimeScale      UI32
        The number of time units per second, used in the FirstFragmentTime stamp and
        Fragment Duration fields.
        Typically, the value is 1000.
    */
    afrt.write4Bytes(1000);

    /*!
        @QualityEntryCount      UI8
        The number of QualitySegment Url Modifiers
        (quality level references) that follow.
        If 0, this Fragment Run Table applies to all quality levels,
        and there shall be only one Fragment Run Table
        box in the Bootstrap Info box.
    */
    afrt.write1Bytes(0);

    /*!
        @FragmentRunEntryCount      UI32
        The number of items in this FragmentRunEntryTable.
        The minimum value is 1.
    */
    afrt.write4Bytes(m_segments.size());

    list<BlsSegment *>::iterator iter;
    for (iter = m_segments.begin(); iter != m_segments.end(); ++iter) {
        BlsSegment *st = *iter;
        afrt.write4Bytes(st->index());
        afrt.write8Bytes(st->startTime());
        afrt.write4Bytes(st->duration());
    }

    update_box(afrt);
    abst.append(afrt);
    update_box(asrt);
}

int BlsHDS::flushMainfest()
{
    char buf[1024] = {0};
    sprintf(buf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
            "<manifest xmlns=\"http://ns.adobe.com/f4m/1.0\">\n\t"
            "<id>%s.f4m</id>\n\t"
            "<streamType>live</streamType>\n\t"
            "<deliveryType>streaming</deliveryType>\n\t"
            "<bootstrapInfo profile=\"named\" url=\"stream0.abst\" id=\"bootstrap0\" />\n\t"
            "<media bitrate=\"578\" url=\"stream0\" bootstrapInfoId=\"bootstrap0\"></media>\n"
            "</manifest>"
            , m_ctx.streamName.c_str()
            );

    MString f4mPath = m_ctx.segmentPath + "/" + m_ctx.streamName + ".f4m";
    MFile f4m(f4mPath);
    if (!f4m.open("w")) {
        log_error("open f4m %s failed.", f4mPath.c_str());
        m_invalid = false;
        return E_F4M_OPEN_FAILED;
    }

    if (f4m.write(buf) < 0) {
        log_error("write f4m failed.");
        m_invalid = false;
        return E_F4M_WRITE_FAILED;
    }
    log_trace("build success %s", f4mPath.c_str());

    return E_SUCCESS;
}
