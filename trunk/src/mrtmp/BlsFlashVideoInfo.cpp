#include "BlsFlashVideoInfo.hpp"

#include <MLoger>

FlashVideoFrameType BlsFlashVideoInfo::getFrameType(const MString &data)
{
    const char *dt = data.data();
    unsigned char type = dt[0] >> 4 & 0x0F;

    switch (type) {
    case 1:
        return FlashVideo_KeyFrame;
    case 2:
        return FlashVideo_InterFrame;
    case 3:
        return FlashVideo_Disposable_InterFrame;
    case 4:
        return FlashVideo_Generated_KeyFrame;
    case 5:
        return FlashVideo_VideoInfo;
    default:
        break;
    }

    return FlashVideo_Invalid;
}

FlashVideoCodecID BlsFlashVideoInfo::getCodecID(const MString &data)
{
    const char *dt = data.data();
    unsigned char type = dt[0] & 0x0F;
    switch (type) {
    case 2:
        return FlashVideoCodecID_Sorenson_H263;
    case 3:
        return FlashVideoCodecID_ScreenVideo;
    case 4:
        return FlashVideoCodecID_On2_VP6;
    case 5:
        return FlashVideoCodecID_On2VP6_With_Alpha;
    case 6:
        return FlashVideoCodecID_Screen_Video;
    case 7:
        return FlashVideoCodecID_AVC;
    default:
        break;
    }

    return FlashVideoCodecID_Invalid;
}

FlashVideoAVCPacketType BlsFlashVideoInfo::getAVCPacketType(const MString &data)
{
    FlashVideoCodecID codecID = getCodecID(data);
    if (codecID != FlashVideoCodecID_AVC) {
        return FlashVideoAVC_Invalid;
    }
    const char *dt = data.data();
    unsigned char type = dt[1];
    switch (type) {
    case 0:
        return FlashVideoAVC_Sequence_Header;
    case 1:
        return FlashVideoAVC_NALU;
    case 2:
        return FlashVideoAVC_End_Sequence;
    default:
        break;
    }

    return FlashVideoAVC_Invalid;
}

FlashSoundFormat BlsFlashVideoInfo::getSoundFormat(const MString &data)
{
    const char *dt = data.data();
    unsigned char type = dt[0] >> 4 & 0x0F;

    switch (type) {
    case 0:
        return FlashSoundFormat_Linear_PCM;
    case 1:
        return FlashSoundFormat_ADPCM;
    case 2:
        return FlashSoundFormat_MP3;
    case 3:
        return FlashSoundFormat_Linear_PCM_LE;
    case 4:
        return FlashSoundFormat_Nellymoser_16K_Mono;
    case 5:
        return FlashSoundFormat_Nellymoser_8K_Mono;
    case 6:
        return FlashSoundFormat_Nellymoser;
    case 7:
        return FlashSoundFormat_G711_A_LAW_PCM;
    case 8:
        return FlashSoundFormat_G711_MU_LAW_PCM;
    case 9:
        return FlashSoundFormat_Reserved;
    case 10:
        return FlashSoundFormat_AAC;
    case 11:
        return FlashSoundFormat_Speex;
    case 14:
        return FlashSoundFormat_MP3_8K;
    case 15:
        return FlashSoundFormat_Device_Specific;
    default:
        break;
    }

    return FlashSoundFormat_Invalid;
}

FlashSoundRate BlsFlashVideoInfo::getSoundRate(const MString &data)
{
    const char *dt = data.data();
    unsigned char type = dt[0] >> 2 & 0x03;

    switch (type) {
    case 0:
        return FlashSoundRate_5_5KHZ;
    case 1:
        return FlashSoundRate_11_KHZ;
    case 2:
        return FlashSoundRate_22_KHZ;
    case 3:
        return FlashSoundRate_44_KHZ;
    default:
        break;
    }

    return FlashSoundRate_Invalid;
}

FlashSoundSize BlsFlashVideoInfo::getSoundSize(const MString &data)
{
    const char *dt = data.data();
    unsigned char type = dt[0] >> 1 & 0x01;

    switch (type) {
    case 0:
        return FlashSoundSize_8bit_Sample;
    case 1:
        return FlashSoundSize_16bit_Sample;
    default:
        break;
    }

    return FlashSoundSize_Invalid;
}

FlashSoundChannel BlsFlashVideoInfo::getSoundChannel(const MString &data)
{
    const char *dt = data.data();
    unsigned char type = dt[0] & 0x01;

    switch (type) {
    case 0:
        return FlashSoundChannel_Mono;
    case 1:
        return FlashSoundChannel_Stereo;
    default:
        break;
    }

    return FlashSoundChannel_Invalid;
}

FlashSoundAACPacketType BlsFlashVideoInfo::getAACPacketType(const MString &data)
{
    FlashSoundFormat sf = getSoundFormat(data);
    if (sf != FlashSoundFormat_AAC) {
        return FlashSoundAAC_Invalid;
    }

    const char *dt = data.data();
    unsigned char type = dt[1];

    switch (type) {
    case 0:
        return FlashSoundAAC_Sequence_Header;
    case 1:
        return FlashSoundAAC_Raw;
    default:
        break;
    }

    return FlashSoundAAC_Invalid;
}

bool BlsFlashVideoInfo::videoIsKeyFrame(const MString &data)
{
    return getFrameType(data) == FlashVideo_KeyFrame;
}

bool BlsFlashVideoInfo::videoIsSequenceHeader(const MString &data)
{
    return getAVCPacketType(data) == FlashVideoAVC_Sequence_Header;
}

bool BlsFlashVideoInfo::audioIsSequenceHeader(const MString &data)
{
    return getAACPacketType(data) == FlashSoundAAC_Sequence_Header;
}

bool BlsFlashVideoInfo::videoIsH264(const MString &data)
{
    return getCodecID(data) == FlashVideoCodecID_AVC;
}

bool BlsFlashVideoInfo::audioIsAAC(const MString &data)
{
    return getSoundFormat(data) == FlashSoundFormat_AAC;
}

bool BlsFlashVideoInfo::audioIsNellymoser(const MString &data)
{
    return getSoundFormat(data) == FlashSoundFormat_Nellymoser;
}
