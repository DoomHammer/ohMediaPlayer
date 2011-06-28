#include "SourcePlaylist.h"
#include <Debug.h>
#include "Playlist.h"

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;

static const Brn kProvider("av.openhome.org/Providers/Playlist");

SourcePlaylist::SourcePlaylist(Net::DvDevice& aDevice, uint32_t aTracksMax, const char* aProtocolInfo, Player& aPlayer)
    : Source(Brn("Playlist"), Brn("Playlist"), Brn("Playlist"), true)
    , iPlayer(aPlayer)
{
    iProviderPlaylist = new ProviderPlaylist(aDevice, aTracksMax, Brn(aProtocolInfo), aPlayer);
}

SourcePlaylist::~SourcePlaylist()
{
    delete iProviderPlaylist;
}

void SourcePlaylist::Finished(uint32_t aId)
{
    TUint nextId;
    Bws<Track::kMaxUriBytes> uri;
    iProviderPlaylist->Next(aId, nextId, uri);

    if(aId == 0) {
        iProviderPlaylist->SetId(0);
        iProviderPlaylist->SetTransportState(ProviderPlaylist::eStopped);
    }
    else {
        iPlayer.Play(nextId, uri, 0, kProvider);
    }
}

void SourcePlaylist::Next(uint32_t aAfterId, uint32_t& aId, Bwx& aUri, Bwx& aProvider)
{
    iProviderPlaylist->Next(aAfterId, aId, aUri);

    aProvider.Replace(kProvider);
}

void SourcePlaylist::Buffering(uint32_t aId)
{
    iProviderPlaylist->SetTransportState(ProviderPlaylist::eBuffering);
}

void SourcePlaylist::Playing(uint32_t aId)
{
    iProviderPlaylist->SetId(aId);
    iProviderPlaylist->SetTransportState(ProviderPlaylist::ePlaying);
}
