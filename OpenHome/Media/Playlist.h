#ifndef HEADER_OPENHOME_MEDIA_PLAYLIST
#define HEADER_OPENHOME_MEDIA_PLAYLIST

#include <OpenHome/OhNetTypes.h>
#include <OpenHome/Buffer.h>
#include <OpenHome/Net/Core/DvDevice.h>
#include <OpenHome/Private/Thread.h>
#include <OpenHome/Private/Timer.h>

#include <Generated/DvAvOpenhomeOrgPlaylist1.h>

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
    virtual void Play(Net::IDvInvocation& aResponse);
    virtual void Pause(Net::IDvInvocation& aResponse);
    virtual void Stop(Net::IDvInvocation& aResponse);
    virtual void Next(Net::IDvInvocation& aResponse);
    virtual void Previous(Net::IDvInvocation& aResponse);
    virtual void SetRepeat(Net::IDvInvocation& aResponse, TBool aValue);
    virtual void Repeat(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseBool& aValue);
    virtual void SetShuffle(Net::IDvInvocation& aResponse, TBool aValue);
    virtual void Shuffle(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseBool& aValue);
    virtual void SeekSecondAbsolute(Net::IDvInvocation& aResponse, TUint aValue);
    virtual void SeekSecondRelative(Net::IDvInvocation& aResponse, TInt aValue);
    virtual void SeekId(Net::IDvInvocation& aResponse, TUint aValue);
    virtual void SeekIndex(Net::IDvInvocation& aResponse, TUint aValue);
    virtual void TransportState(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseString& aValue);
    virtual void Id(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aValue);
    virtual void Read(Net::IDvInvocation& aResponse, TUint aId, Net::IDvInvocationResponseString& aUri, Net::IDvInvocationResponseString& aMetadata);
    virtual void ReadList(Net::IDvInvocation& aResponse, const Brx& aIdList, Net::IDvInvocationResponseString& aTrackList);
    virtual void Insert(Net::IDvInvocation& aResponse, TUint aAfterId, const Brx& aUri, const Brx& aMetadata, Net::IDvInvocationResponseUint& aNewId);
    virtual void DeleteId(Net::IDvInvocation& aResponse, TUint aValue);
    virtual void DeleteAll(Net::IDvInvocation& aResponse);
    virtual void TracksMax(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aValue);
    virtual void IdArray(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aToken, Net::IDvInvocationResponseBinary& aArray);
    virtual void IdArrayChanged(Net::IDvInvocation& aResponse, TUint aToken, Net::IDvInvocationResponseBool& aValue);
    virtual void ProtocolInfo(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseString& aValue);

private:
    void UpdateIdArray();
    bool IsRepeat();
    bool IsShuffle(); 
    const Track* IterateForwards(std::list<Track*>::const_iterator aIter, uint32_t aCount);
    const Track* IterateBackwards(std::list<Track*>::const_iterator aIter, uint32_t aCount);
private:
    std::list<Track*> iList;

    TUint iToken;
    Bwh iIdArray;
    Mutex iMutex;
    ETransportState iState;
    Source& iSource;
};

} // namespace Media
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIA_PLAYLIST

