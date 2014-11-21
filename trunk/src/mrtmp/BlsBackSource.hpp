#ifndef BLSBACKSOURCE_HPP
#define BLSBACKSOURCE_HPP

#include <MObject>
#include <MHash>

#include "mrtmpurl.hpp"

class MRtmpPlayer;

class BlsBackSource : public MObject
{
public:
    BlsBackSource();

    static BlsBackSource * instance();

    void add(const MString &host, mint16 port, const MString &app, const MString &url);
    void remove(const MRtmpUrl &url);
    bool hasBackSource(const MString &url);
    void setHasBackSource(const MString &url);

private:
    struct SourcePair
    {
        MRtmpUrl url;
        MRtmpPlayer *player;
    };

    MHash<MString, SourcePair> m_sources;
};

#endif // BLSBACKSOURCE_HPP
