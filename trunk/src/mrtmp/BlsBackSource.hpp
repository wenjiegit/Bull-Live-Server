#ifndef BLSBACKSOURCE_HPP
#define BLSBACKSOURCE_HPP

#include <MObject>
#include <MHash>

#include "BlsRtmpUrl.hpp"

class BlsRtmpPlayer;

class BlsBackSource : public MObject
{
public:
    BlsBackSource();

    static BlsBackSource * instance();

    void add(const MString &host, mint16 port, const MString &app, const MString &url);
    void remove(const BlsRtmpUrl &url);
    bool hasBackSource(const MString &url);
    void setHasBackSource(const MString &url);

private:
    struct SourcePair
    {
        BlsRtmpUrl url;
        BlsRtmpPlayer *player;
    };

    MHash<MString, SourcePair> m_sources;
};

#endif // BLSBACKSOURCE_HPP
