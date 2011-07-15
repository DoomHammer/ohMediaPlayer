#include "SourcePlaylist.h"
#include <Debug.h>
#include "Playlist.h"

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;

static const Brn kProvider("av.openhome.org/Providers/Playlist");

SourcePlaylist::SourcePlaylist(Net::DvDevice& aDevice, uint32_t aTracksMax, const char* aProtocolInfo, Player& aPlayer)
    : Source(Brn("Playlist"), Brn("Playlist"), Brn("Playlist"), true, aPlayer)
{
    iProviderPlaylist = new ProviderPlaylist(aDevice, aTracksMax, Brn(aProtocolInfo), *this);
}

SourcePlaylist::~SourcePlaylist()
{
    delete iProviderPlaylist;
}

const Track* SourcePlaylist::GetTrack(uint32_t aId, int32_t aOffset)
{
    return iProviderPlaylist->GetTrack(aId, aOffset);
}

void SourcePlaylist::Buffering(uint32_t aId)
{
    iProviderPlaylist->SetTransportState(ProviderPlaylist::eBuffering);
    iProviderPlaylist->SetId(aId);
}

void SourcePlaylist::Stopped(uint32_t aId)
{
    iProviderPlaylist->SetTransportState(ProviderPlaylist::eStopped);
    iProviderPlaylist->SetId(aId);
}

void SourcePlaylist::Paused(uint32_t aId)
{
    iProviderPlaylist->SetTransportState(ProviderPlaylist::ePaused);
}

void SourcePlaylist::Playing(uint32_t aId)
{
    iProviderPlaylist->SetTransportState(ProviderPlaylist::ePlaying);
}
