#ifndef HEADER_OPENHOME_MEDIA_SOURCE_PLAYLIST
#define HEADER_OPENHOME_MEDIA_SOURCE_PLAYLIST

#include <OpenHome/Net/Core/DvDevice.h>
#include <OpenHome/Media/Source.h>
#include <OpenHome/Media/Player.h>

namespace OpenHome {
namespace Media {

class ProviderPlaylist;

class SourcePlaylist : public Source
{
public:
    SourcePlaylist(Net::DvDevice& aDevice, uint32_t aTracksMax, const char* aProtocolInfo, Player& aPlayer);
    virtual ~SourcePlaylist();

//from Source
public:
    virtual const Track* GetTrack(uint32_t aId, int32_t aIndex);
    virtual void Buffering(uint32_t aId);
    virtual void Stopped(uint32_t aId);
    virtual void Paused(uint32_t aId);
    virtual void Playing(uint32_t aId);

private:
    ProviderPlaylist* iProviderPlaylist;
};

} // namespace Media
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIA_SOURCE_PLAYLIST

