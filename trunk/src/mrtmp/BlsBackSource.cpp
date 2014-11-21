#include "BlsBackSource.hpp"

#include "mrtmpurl.hpp"
#include "mrtmpplayer.hpp"

BlsBackSource::BlsBackSource()
{
}

BlsBackSource *BlsBackSource::instance()
{
    static BlsBackSource *ret = NULL;
    if (!ret) {
        ret = new BlsBackSource;
    }

    return ret;
}

void BlsBackSource::add(const MString &host, mint16 port, const MString &app, const MString &url)
{
    MRtmpUrl rtmpUrl(url);
    if (m_sources.contains(rtmpUrl.url())) {
        return ;
    }

    MRtmpPlayer *player = new MRtmpPlayer;
    player->setUrl(host, port, app, url);
    player->start();

    SourcePair pair;
    pair.player = player;
    pair.url = rtmpUrl;

    m_sources[rtmpUrl.url()] = pair;
}

void BlsBackSource::remove(const MRtmpUrl &url)
{
    MRtmpUrl rtmpUrl(url);
    m_sources.erase(rtmpUrl.url());
}

bool BlsBackSource::hasBackSource(const MString &url)
{
    MRtmpUrl rtmpUrl(url);

    return m_sources.contains(rtmpUrl.url());
}

void BlsBackSource::setHasBackSource(const MString &url)
{
    MRtmpUrl rtmpUrl(url);
    SourcePair pair;
    pair.player = NULL;
    pair.url = rtmpUrl;

    m_sources[rtmpUrl.url()] = pair;
}
