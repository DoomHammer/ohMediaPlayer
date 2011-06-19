#include <Debug.h>
#include "Playlist.h"
#include <functional>
#include <algorithm>

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;
using namespace std;

Track::Track(TUint aId, const Brx& aUri, const Brx& aMetadata)
    : iId(aId)
{
    iUri.Replace(aUri);
    iMetadata.Replace(aMetadata);
}

TBool Track::IsId(TUint aId) const
{
    return (aId == iId);
}

TUint Track::Id() const
{
    return iId;
}

const Brx& Track::Uri() const
{
    return iUri;
}

const Brx& Track::Metadata() const
{
    return iMetadata;
}

static const TInt kIdNotFound = 800;
static const Brn kIdNotFoundMsg("Id not found");
static const TInt kPlaylistFull = 801;
static const Brn kPlaylistFullMsg("Playlist full");
static const TInt kInvalidRequest = 802;
static const Brn kInvalidRequestMsg("Comma separated id request list invalid");

PlaylistImpl::PlaylistImpl(Net::DvDevice& aDevice, TUint aTracksMax, const Brx& aProtocolInfo)
    : DvProviderAvOpenhomeOrgPlaylist1(aDevice)
    , iTransportState("Stopped")
    , iRepeat(false)
    , iShuffle(false)
    , iId(0)
    , iNextId(1)
    , iToken(0)
    , iIdArray(sizeof(TUint)*aTracksMax)
    , iMutex("Play")
{

    EnableActionPlay();
    EnableActionPause();
    EnableActionStop();
    EnableActionNext();
    EnableActionPrevious();
    EnableActionSetRepeat();
    EnableActionRepeat();
    EnableActionSetShuffle();
    EnableActionShuffle();
    EnableActionSeekSecondAbsolute();
    EnableActionSeekSecondRelative();
    EnableActionSeekId();
    EnableActionSeekIndex();
    EnableActionTransportState();
    EnableActionId();
    EnableActionRead();
    EnableActionReadList();
    EnableActionInsert();
    EnableActionDeleteId();
    EnableActionDeleteAll();
    EnableActionTracksMax();
    EnableActionIdArray();
    EnableActionIdArrayChanged();
    EnableActionProtocolInfo();

    SetPropertyTransportState(iTransportState);
    SetPropertyRepeat(iRepeat);
    SetPropertyShuffle(iShuffle);
    SetPropertyId(iId);
    SetPropertyTracksMax(aTracksMax);
    SetPropertyProtocolInfo(aProtocolInfo);

    UpdateIdArray();
    SetPropertyIdArray(iIdArray);
}

void PlaylistImpl::Play(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();
}

void PlaylistImpl::Pause(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();
}

void PlaylistImpl::Stop(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();
}

void PlaylistImpl::Next(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();
}

void PlaylistImpl::Previous(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();
}

void PlaylistImpl::SetRepeat(Net::IInvocationResponse& aResponse, TUint aVersion, TBool aValue)
{
    iRepeat = aValue;
    aResponse.Start();
    aResponse.End();
}

void PlaylistImpl::Repeat(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseBool& aValue)
{
    TBool value = iRepeat;
    aResponse.Start();
    aValue.Write(value);
    aResponse.End();
}

void PlaylistImpl::SetShuffle(Net::IInvocationResponse& aResponse, TUint aVersion, TBool aValue)
{
    iShuffle = aValue;
    aResponse.Start();
    aResponse.End();
}

void PlaylistImpl::Shuffle(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseBool& aValue)
{
    TBool value = iShuffle;
    aResponse.Start();
    aValue.Write(value);
    aResponse.End();
}

void PlaylistImpl::SeekSecondAbsolute(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    Log::Print("SeekSecondAbsolute: %d\n", aValue);
    aResponse.Start();
    aResponse.End();
}

void PlaylistImpl::SeekSecondRelative(Net::IInvocationResponse& aResponse, TUint aVersion, TInt aValue)
{
    Log::Print("SeekSecondRelative: %d\n", aValue);
    aResponse.Start();
    aResponse.End();
}

void PlaylistImpl::SeekId(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    Log::Print("SeekId: %d\n", aValue);
    aResponse.Start();
    aResponse.End();
}

void PlaylistImpl::SeekIndex(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    Log::Print("SeekIndex: %d\n", aValue);
    aResponse.Start();
    aResponse.End();
}

void PlaylistImpl::TransportState(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aValue)
{
    Brhz transportState;
    GetPropertyTransportState(transportState);
    aResponse.Start();
    aValue.Write(transportState);
    aValue.WriteFlush();
    aResponse.End();
}

void PlaylistImpl::Id(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue)
{
    TUint id;
    GetPropertyId(id);
    aResponse.Start();
    aValue.Write(id);
    aResponse.End();
}

void PlaylistImpl::Read(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aId, Net::IInvocationResponseString& aUri, Net::IInvocationResponseString& aMetadata)
{
    if(aId == 0) {
        aResponse.Error(kIdNotFound, kIdNotFoundMsg);
    }   

    iMutex.Wait();

    list<Track*>::const_iterator i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),aId));

    if(i == iList.end()) {
        iMutex.Signal();
        aResponse.Error(kIdNotFound, kIdNotFoundMsg);
    }
    else {
        aResponse.Start();
        aUri.Write((*i)->Uri());
        aUri.WriteFlush();
        aMetadata.Write((*i)->Metadata());
        aMetadata.WriteFlush();
        aResponse.End();
        iMutex.Signal();
    }
}

void PlaylistImpl::ReadList(Net::IInvocationResponse& aResponse, TUint aVersion, const Brx& aIdList, Net::IInvocationResponseString& aTrackList)
{
    Log::Print("PlaylistImpl::Readlist\n");
}

void PlaylistImpl::Insert(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aAfterId, const Brx& aUri, const Brx& aMetadata, Net::IInvocationResponseUint& aNewId)
{
    iMutex.Wait();

    TUint tracksMax;
    GetPropertyTracksMax(tracksMax);
    if(iList.size() == tracksMax) {
        iMutex.Signal();
        aResponse.Error(kPlaylistFull, kPlaylistFullMsg);
    }

    list<Track*>::iterator i;
    if(aAfterId == 0) {
        i = iList.begin();
    }
    else {
        i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),aAfterId));
        if(i == iList.end()) {
            iMutex.Signal();
            aResponse.Error(kIdNotFound, kIdNotFoundMsg);
        }
        ++i;  //we insert after the id, not before
    }

    iList.insert(i, new Track(iNextId, aUri, aMetadata));
    aResponse.Start();
    aNewId.Write(iNextId++);
    aResponse.End();
    UpdateIdArray();

    iMutex.Signal();
}

void PlaylistImpl::DeleteId(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    iMutex.Wait();

    list<Track*>::iterator i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),aValue));

    if(i != iList.end()) {
        delete (*i);
        iList.erase(i);
        UpdateIdArray();
    }

    iMutex.Signal();

    aResponse.Start();
    aResponse.End();
}

void PlaylistImpl::DeleteAll(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    iMutex.Wait();

    if(iList.size() > 0) {
        list<Track*>::iterator i = iList.begin();
        for( ; i != iList.end(); ++i) {
            delete *i; 
        }

        iList.clear();
        UpdateIdArray();
    }

    iMutex.Signal();

    aResponse.Start();
    aResponse.End();
}

void PlaylistImpl::TracksMax(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue)
{
    TUint tracksMax;
    GetPropertyTracksMax(tracksMax);
    aResponse.Start();
    aValue.Write(tracksMax);
    aResponse.End();
}

void PlaylistImpl::IdArray(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aToken, Net::IInvocationResponseBinary& aArray)
{
    iMutex.Wait();

    aResponse.Start();
    aToken.Write(iToken);
    aArray.Write(iIdArray);
    aArray.WriteFlush();
    aResponse.End();

    iMutex.Signal();
}

void PlaylistImpl::IdArrayChanged(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aToken, Net::IInvocationResponseBool& aValue)
{
    TBool changed = false;

    iMutex.Wait();
    if(aToken != iToken) {
        changed = true;
    }
    iMutex.Signal();

    aResponse.Start();
    aValue.Write(changed);
    aResponse.End();
}

void PlaylistImpl::ProtocolInfo(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aValue)
{
    Brhz protocolInfo;
    GetPropertyProtocolInfo(protocolInfo);

    aResponse.Start();
    aValue.Write(protocolInfo);
    aValue.WriteFlush();
    aResponse.End();
}

//Must be called with iMutex held
void PlaylistImpl::UpdateIdArray()
{
    iToken++;

    iIdArray.SetBytes(0);
    WriterBuffer writer(iIdArray);
    WriterBinary binary(writer);

    for(list<Track*>::const_iterator i = iList.begin(); i != iList.end(); ++i) {
        binary.WriteUint32Be((*i)->Id());
    }
    SetPropertyIdArray(iIdArray);
}


