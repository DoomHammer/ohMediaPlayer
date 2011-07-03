#ifndef HEADER_OPENHOME_MEDIAPLAYER_SOURCE_PLAYLIST
#define HEADER_OPENHOME_MEDIAPLAYER_SOURCE_PLAYLIST

#include <Core/DvDevice.h>
#include "Source.h"
#include "Player.h"

namespace OpenHome {
namespace MediaPlayer {

class ProviderPlaylist;

class SourcePlaylist : public Source
{
public:
    SourcePlaylist(Net::DvDevice& aDevice, uint32_t aTracksMax, const char* aProtocolInfo, Player& aPlayer);
    ~SourcePlaylist();

//from Source
public:
    virtual void Finished(uint32_t aId);
    virtual void Next(uint32_t aAfterId, uint32_t& aId, Bwx& aUri);
    virtual void Buffering(uint32_t aId);
    virtual void Playing(uint32_t aId);

private:
    ProviderPlaylist* iProviderPlaylist;
};

} // namespace MediaPlayer
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIAPLAYER_SOURCE_PLAYLIST

