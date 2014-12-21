#ifndef MFLASHVIDEOINFO_HPP
#define MFLASHVIDEOINFO_HPP

#include <MObject>

enum FlashVideoFrameType
{
    FlashVideo_Invalid                  = -1,
    FlashVideo_KeyFrame                 = 1,
    FlashVideo_InterFrame               = 2,
    FlashVideo_Disposable_InterFrame    = 3,        // H263 only
    FlashVideo_Generated_KeyFrame       = 4,        // reserved for server use only
    FlashVideo_VideoInfo                = 5,
};

enum FlashVideoCodecID
{
    FlashVideoCodecID_Invalid           = -1,
    FlashVideoCodecID_Sorenson_H263     = 2,
    FlashVideoCodecID_ScreenVideo       = 3,
    FlashVideoCodecID_On2_VP6           = 4,
    FlashVideoCodecID_On2VP6_With_Alpha = 5,
    FlashVideoCodecID_Screen_Video      = 6,
    FlashVideoCodecID_AVC               = 7,
};

enum FlashVideoAVCPacketType
{
    FlashVideoAVC_Invalid               = -1,
    FlashVideoAVC_Sequence_Header       = 0,
    FlashVideoAVC_NALU                  = 1,
    FlashVideoAVC_End_Sequence          = 2,
};

enum FlashSoundFormat
{
    FlashSoundFormat_Invalid                = -1,
    FlashSoundFormat_Linear_PCM             = 0,
    FlashSoundFormat_ADPCM                  = 1,
    FlashSoundFormat_MP3                    = 2,
    FlashSoundFormat_Linear_PCM_LE          = 3,
    FlashSoundFormat_Nellymoser_16K_Mono    = 4,
    FlashSoundFormat_Nellymoser_8K_Mono     = 5,
    FlashSoundFormat_Nellymoser             = 6,
    FlashSoundFormat_G711_A_LAW_PCM         = 7,        // reserved
    FlashSoundFormat_G711_MU_LAW_PCM        = 8,        // reserved
    FlashSoundFormat_Reserved               = 9,
    FlashSoundFormat_AAC                    = 10,       // supported in Flash Player 9,0,115,0 and higher
    FlashSoundFormat_Speex                  = 11,       // supported in Flash Player 10 and higher
    FlashSoundFormat_MP3_8K                 = 14,       // reserved
    FlashSoundFormat_Device_Specific        = 15,       // reserved
};

enum FlashSoundRate
{
    FlashSoundRate_Invalid                 = -1,
    FlashSoundRate_5_5KHZ                  = 0,
    FlashSoundRate_11_KHZ                  = 1,
    FlashSoundRate_22_KHZ                  = 2,
    FlashSoundRate_44_KHZ                  = 3,
};

enum FlashSoundSize
{
    FlashSoundSize_Invalid                 = -1,
    FlashSoundSize_8bit_Sample             = 0,
    FlashSoundSize_16bit_Sample            = 1,
};

enum FlashSoundChannel
{
    FlashSoundChannel_Invalid               = -1,
    FlashSoundChannel_Mono                  = 0,
    FlashSoundChannel_Stereo                = 1,
};

enum FlashSoundAACPacketType
{
    FlashSoundAAC_Invalid                   = -1,
    FlashSoundAAC_Sequence_Header           = 0,
    FlashSoundAAC_Raw                       = 1,
};

class BlsFlashVideoInfo : public MObject
{
public:
    static FlashVideoFrameType      getFrameType(const MString &data);
    static FlashVideoCodecID        getCodecID(const MString &data);
    static FlashVideoAVCPacketType  getAVCPacketType(const MString &data);
    static FlashSoundFormat         getSoundFormat(const MString &data);
    static FlashSoundRate           getSoundRate(const MString &data);
    static FlashSoundSize           getSoundSize(const MString &data);
    static FlashSoundChannel        getSoundChannel(const MString &data);
    static FlashSoundAACPacketType  getAACPacketType(const MString &data);

    // utils
    static bool videoIsKeyFrame(const MString &data);
    static bool videoIsSequenceHeader(const MString &data);
    static bool audioIsSequenceHeader(const MString &data);
    static bool videoIsH264(const MString &data);
    static bool audioIsAAC(const MString &data);
    static bool audioIsNellymoser(const MString &data);
};

#endif // MFLASHVIDEOINFO_HPP
