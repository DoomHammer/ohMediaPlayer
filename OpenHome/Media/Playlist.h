#ifndef HEADER_OPENHOME_MEDIA_PLAYLIST
#define HEADER_OPENHOME_MEDIA_PLAYLIST

#include <OpenHome/OhNetTypes.h>
#include <OpenHome/Buffer.h>
#include <OpenHome/Net/Core/DvDevice.h>
#include <OpenHome/Private/Thread.h>
#include <OpenHome/Private/Timer.h>

#include <OpenHome/Net/Core/DvAvOpenhomeOrgPlaylist1.h>

#include <OpenHome/Media/SourcePlaylist.h>

namespace OpenHome {
namespace Media {

class ProviderPlaylist : public Net::DvProviderAvOpenhomeOrgPlaylist1
{
public:
    static const Brn kProvider;

    enum ETransportState 
    {
        ePlaying = 0,
        ePaused = 1,
        eStopped = 2,
        eBuffering = 3
    }; 

	ProviderPlaylist(Net::DvDevice& aDevice, TUint aMaxTracks, const Brx& aProtocolInfo, Source& aSource); 

    const Track* GetTrack(TUint aId, TInt aOffset);
    void SetTransportState(ETransportState aState);
    void SetId(TUint aId);

private:

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
    bool IsRepeat();
    bool IsShuffle(); 
    const Track* IterateForwards(std::list<Track*>::const_iterator aIter, uint32_t aCount);
    const Track* IterateBackwards(std::list<Track*>::const_iterator aIter, uint32_t aCount);
private:
    std::list<Track*> iList;

    TUint iId;
    TUint iToken;
    Bwh iIdArray;
    TUint iTracksMax;
    Mutex iMutex;
    ETransportState iState;
    Source& iSource;
};

} // namespace Media
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIA_PLAYLIST

