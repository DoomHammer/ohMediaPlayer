#ifndef HEADER_OPENHOME_MEDIAPLAYER_PLAYLIST
#define HEADER_OPENHOME_MEDIAPLAYER_PLAYLIST

#include <OhNetTypes.h>
#include <Buffer.h>
#include <Core/DvDevice.h>
#include <Thread.h>
#include <Timer.h>

#include <Core/DvAvOpenhomeOrgPlaylist1.h>

namespace OpenHome {
namespace MediaPlayer {

class Track
{
public:
    static const TUint kMaxUriBytes = 1024;
    static const TUint kMaxMetadataBytes = 5 * 1024;

public:
    Track(TUint aId, const Brx& aUri, const Brx& aMetadata);
    TBool IsId(TUint aId) const;
    TUint Id() const;
    const Brx& Uri() const;
    const Brx& Metadata() const;

private:
    TUint iId;
    Bws<kMaxUriBytes> iUri;
    Bws<kMaxMetadataBytes> iMetadata;
};

class PlaylistImpl : public Net::DvProviderAvOpenhomeOrgPlaylist1
{
public:
	PlaylistImpl(Net::DvDevice& aDevice, TUint aMaxTracks, const Brx& aProtocolInfo);

    //From DvProviderAvOpenhomeOrgPlaylist1
private:
    virtual void Play(Net::IInvocationResponse& aResponse, TUint aVersion);
    virtual void Pause(Net::IInvocationResponse& aResponse, TUint aVersion);
    virtual void Stop(Net::IInvocationResponse& aResponse, TUint aVersion);
    virtual void Next(Net::IInvocationResponse& aResponse, TUint aVersion);
    virtual void Previous(Net::IInvocationResponse& aResponse, TUint aVersion);
    virtual void SetRepeat(Net::IInvocationResponse& aResponse, TUint aVersion, TBool aValue);
    virtual void Repeat(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseBool& aValue);
    virtual void SetShuffle(Net::IInvocationResponse& aResponse, TUint aVersion, TBool aValue);
    virtual void Shuffle(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseBool& aValue);
    virtual void SeekSecondAbsolute(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue);
    virtual void SeekSecondRelative(Net::IInvocationResponse& aResponse, TUint aVersion, TInt aValue);
    virtual void SeekId(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue);
    virtual void SeekIndex(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue);
    virtual void TransportState(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aValue);
    virtual void Id(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue);
    virtual void Read(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aId, Net::IInvocationResponseString& aUri, Net::IInvocationResponseString& aMetadata);
    virtual void ReadList(Net::IInvocationResponse& aResponse, TUint aVersion, const Brx& aIdList, Net::IInvocationResponseString& aTrackList);
    virtual void Insert(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aAfterId, const Brx& aUri, const Brx& aMetadata, Net::IInvocationResponseUint& aNewId);
    virtual void DeleteId(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue);
    virtual void DeleteAll(Net::IInvocationResponse& aResponse, TUint aVersion);
    virtual void TracksMax(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue);
    virtual void IdArray(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aToken, Net::IInvocationResponseBinary& aArray);
    virtual void IdArrayChanged(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aToken, Net::IInvocationResponseBool& aValue);
    virtual void ProtocolInfo(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aValue);

private:
    void UpdateIdArray();

private:
    std::list<Track*> iList;

    Bws<20> iTransportState;
    TUint iId;
    TUint iNextId;
    TUint iToken;
    Bwh iIdArray;
    TUint iTracksMax;
    Mutex iMutex;
};

} // namespace MediaPlayer
} // namespace OpenHome

#endif // HEADER_OPENHOME_PLAYLIST

