#include <Debug.h>
#include "Playlist.h"
#include <functional>
#include <algorithm>
#include <Parser.h>
#include <Ascii.h>
#include <Converter.h>

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;
using namespace std;

static const TInt kIdNotFound = 800;
static const Brn kIdNotFoundMsg("Id not found");
static const TInt kPlaylistFull = 801;
static const Brn kPlaylistFullMsg("Playlist full");
static const TInt kInvalidRequest = 802;
static const Brn kInvalidRequestMsg("Space separated id request list invalid");

static const Brn kPlaying("Playing");
static const Brn kPaused("Paused");
static const Brn kStopped("Stopped");
static const Brn kBuffering("Buffering");

const Brn ProviderPlaylist::kProvider("av.openhome.org/Providers/Playlist");

ProviderPlaylist::ProviderPlaylist(Net::DvDevice& aDevice, TUint aTracksMax, const Brx& aProtocolInfo, Source& aSource)
    : DvProviderAvOpenhomeOrgPlaylist1(aDevice)
    , iToken(0)
    , iIdArray(sizeof(TUint)*aTracksMax)
    , iMutex("Play")
    , iSource(aSource)
    , iState(eStopped)
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

    SetPropertyTransportState(kStopped);
    SetPropertyRepeat(false);
    SetPropertyShuffle(false);
    SetPropertyId(0);
    SetPropertyTracksMax(aTracksMax);
    SetPropertyProtocolInfo(aProtocolInfo);

    UpdateIdArray();
    SetPropertyIdArray(iIdArray);
}

const Track* ProviderPlaylist::Next(TUint aId, TInt aIndex)
{
    const Track* track;

    iMutex.Wait();

    list<Track*>::const_iterator i;
    i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),aId));
    if(i == iList.end()) {
        ASSERTS();
    }
    else if(aIndex > 0) {
        track = IterateForwards(i, aIndex); 
        track->IncRef();
    }
    else if(aIndex < 0) {
        track = IterateBackwards(i, abs(aIndex)); 
        track->IncRef();
    }
    else {
        track = *i;
        track->IncRef();
    }

    iMutex.Signal();

    return track;
}

void ProviderPlaylist::SetTransportState(ETransportState aState)
{
    iMutex.Wait();
    
    if(aState != iState) {
        iState = aState;
        switch(iState) {
            case ePlaying:
                SetPropertyTransportState(kPlaying);
                break;
            case ePaused:
                SetPropertyTransportState(kPaused);
                break;
            case eStopped:
                SetPropertyTransportState(kStopped);
                break;
            case eBuffering:
                SetPropertyTransportState(kBuffering);
                break;
            default:    
                ASSERTS();
        }
    }

    iMutex.Signal();
}

void ProviderPlaylist::SetId(TUint aId)
{
    SetPropertyId(aId);
}

void ProviderPlaylist::Play(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();

    iSource.Play(0);
}

void ProviderPlaylist::Pause(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();
    
    iSource.Pause();
}

void ProviderPlaylist::Stop(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();

    iSource.Stop();
}

void ProviderPlaylist::Next(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();

    iSource.Play(1);
}

void ProviderPlaylist::Previous(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();

    iSource.Play(-1);
}

void ProviderPlaylist::SetRepeat(Net::IInvocationResponse& aResponse, TUint aVersion, TBool aValue)
{
    SetPropertyRepeat(aValue);
    aResponse.Start();
    aResponse.End();
}

void ProviderPlaylist::Repeat(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseBool& aValue)
{
    TBool repeat;
    GetPropertyRepeat(repeat);
    aResponse.Start();
    aValue.Write(repeat);
    aResponse.End();
}

void ProviderPlaylist::SetShuffle(Net::IInvocationResponse& aResponse, TUint aVersion, TBool aValue)
{
    SetPropertyShuffle(aValue);
    aResponse.Start();
    aResponse.End();
}

void ProviderPlaylist::Shuffle(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseBool& aValue)
{
    TBool shuffle;
    GetPropertyShuffle(shuffle);
    aResponse.Start();
    aValue.Write(shuffle);
    aResponse.End();
}

void ProviderPlaylist::SeekSecondAbsolute(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    aResponse.Start();
    aResponse.End();

    iSource.PlaySecondAbsolute(aValue);
}

void ProviderPlaylist::SeekSecondRelative(Net::IInvocationResponse& aResponse, TUint aVersion, TInt aValue)
{
    aResponse.Start();
    aResponse.End();

    iSource.PlaySecondRelative(aValue);
}

void ProviderPlaylist::SeekId(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    aResponse.Start();
    aResponse.End();

    const Track* track = 0;

    iMutex.Wait();

    list<Track*>::const_iterator i;
    i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),aValue));
    if(i != iList.end()) {
        track = (*i);
        track->IncRef();
    }

    iMutex.Signal();

    if(track) {
        iSource.Play(track, 0);
    }
}

void ProviderPlaylist::SeekIndex(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    aResponse.Start();
    aResponse.End();

    const Track* track = 0;

    iMutex.Wait();

    list<Track*>::const_iterator i = iList.begin();

    bool found = false;
    for(TUint j=0; i != iList.end(); j++, i++) {
        if(j == aValue) {
            found = true;
            break;
        }
    }

    if(found == true) {
        track = (*i);
        track->IncRef();
    }
    
    iMutex.Signal();

    if(track) {
        iSource.Play(track, 0);
    }
}

void ProviderPlaylist::TransportState(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aValue)
{
    Log::Print("ProviderPlaylist::TransportState\n");
    Brhz transportState;
    GetPropertyTransportState(transportState);
    aResponse.Start();
    aValue.Write(transportState);
    aValue.WriteFlush();
    aResponse.End();
}

void ProviderPlaylist::Id(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue)
{
    Log::Print("ProviderPlaylist::Id\n");
    TUint id;
    GetPropertyId(id);
    aResponse.Start();
    aValue.Write(id);
    aResponse.End();
}

void ProviderPlaylist::Read(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aId, Net::IInvocationResponseString& aUri, Net::IInvocationResponseString& aMetadata)
{
    Log::Print("ProviderPlaylist::Read\n");
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

void ProviderPlaylist::ReadList(Net::IInvocationResponse& aResponse, TUint aVersion, const Brx& aIdList, Net::IInvocationResponseString& aTrackList)
{
    Log::Print("ProviderPlaylist::ReadList\n");
    uint32_t tracksMax;
    GetPropertyTracksMax(tracksMax);
    vector<uint32_t> v;
    v.reserve(tracksMax);

    try {
        Parser parser(aIdList);
        Brn id;
        id.Set(parser.Next(' '));

        for( ; id != Brx::Empty(); id.Set(parser.Next(' ')) ) {
            v.push_back(Ascii::Uint(id));
            if(v.size() > tracksMax) {
                THROW(AsciiError);
            }
        }
    }
    catch(AsciiError) {
        aResponse.Error(kInvalidRequest, kInvalidRequestMsg);
    }

    Brn entryStart("<Entry>");
    Brn entryEnd("</Entry>");
    Brn idStart("<Id>");
    Brn idEnd("</Id>");
    Brn uriStart("<Uri>");
    Brn uriEnd("</Uri>");
    Brn metaStart("<Metadata>");
    Brn metaEnd("</Metadata>");

    aResponse.Start();
    aTrackList.Write(Brn("<TrackList>"));

    for( vector<uint32_t>::const_iterator id=v.begin(); id!= v.end(); id++) {

        iMutex.Wait();
        list<Track*>::const_iterator i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),*id));

        if(i != iList.end()) {
            aTrackList.Write(entryStart);

            aTrackList.Write(idStart);
            Ascii::StreamWriteUint(aTrackList, *id);
            aTrackList.Write(idEnd);

            aTrackList.Write(uriStart);
            Converter::ToXmlEscaped(aTrackList, (*i)->Uri());
            aTrackList.Write(uriEnd);

            aTrackList.Write(metaStart);
            Converter::ToXmlEscaped(aTrackList, (*i)->Metadata());
            aTrackList.Write(metaEnd);

            aTrackList.Write(entryEnd);
        }
        iMutex.Signal();
    }

    aTrackList.Write(Brn("</TrackList>"));
    aTrackList.WriteFlush();
    aResponse.End();
}

void ProviderPlaylist::Insert(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aAfterId, const Brx& aUri, const Brx& aMetadata, Net::IInvocationResponseUint& aNewId)
{
    Log::Print("ProviderPlaylist::Insert\n");
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

    uint32_t id = iSource.NewId();
    iList.insert(i, new Track(id, aUri, aMetadata));
    aResponse.Start();
    aNewId.Write(id);
    aResponse.End();
    UpdateIdArray();

    iMutex.Signal();
}

void ProviderPlaylist::DeleteId(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    Log::Print("ProviderPlaylist::DeleteId\n");

    bool deleted = false;

    iMutex.Wait();

    list<Track*>::iterator i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),aValue));


    if(i != iList.end()) {
        (*i)->DecRef();
        iList.erase(i);
        UpdateIdArray();
        deleted = true;
    }

    iMutex.Signal();

    aResponse.Start();
    aResponse.End();

    if(deleted) {
        iSource.Deleted(aValue);
    }
}

void ProviderPlaylist::DeleteAll(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    Log::Print("ProviderPlaylist::DeleteAll\n");

    bool deleted = false;

    iMutex.Wait();

    if(iList.size() > 0) {
        list<Track*>::iterator i = iList.begin();
        for( ; i != iList.end(); ++i) {
            (*i)->DecRef(); 
        }

        iList.clear();
        UpdateIdArray();
        deleted = true;
    }

    iMutex.Signal();

    aResponse.Start();
    aResponse.End();

    if(deleted) {
        iSource.Deleted(0); //Special value of 0 indicates entire playlist was deleted
    }
}

void ProviderPlaylist::TracksMax(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue)
{
    Log::Print("ProviderPlaylist::TracksMax\n");
    TUint tracksMax;
    GetPropertyTracksMax(tracksMax);
    aResponse.Start();
    aValue.Write(tracksMax);
    aResponse.End();
}

void ProviderPlaylist::IdArray(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aToken, Net::IInvocationResponseBinary& aArray)
{
    Log::Print("ProviderPlaylist::IdArray\n");
    iMutex.Wait();

    aResponse.Start();
    aToken.Write(iToken);
    aArray.Write(iIdArray);
    aArray.WriteFlush();
    aResponse.End();

    iMutex.Signal();
}

void ProviderPlaylist::IdArrayChanged(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aToken, Net::IInvocationResponseBool& aValue)
{
    Log::Print("ProviderPlaylist::IdArrayChanged\n");
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

void ProviderPlaylist::ProtocolInfo(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aValue)
{
    Brhz protocolInfo;
    GetPropertyProtocolInfo(protocolInfo);

    aResponse.Start();
    aValue.Write(protocolInfo);
    aValue.WriteFlush();
    aResponse.End();
}

//Must be called with iMutex held
void ProviderPlaylist::UpdateIdArray()
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

bool ProviderPlaylist::IsRepeat()
{
    bool repeat;
    GetPropertyRepeat(repeat);
    return repeat;
}

bool ProviderPlaylist::IsShuffle()
{
    bool shuffle;
    GetPropertyRepeat(shuffle);
    return shuffle;
}

const Track* ProviderPlaylist::IterateForwards(list<Track*>::const_iterator aIter, uint32_t aCount)
{
    while( (aCount--) > 0) {
        if(++aIter == iList.end()) {
            if(IsRepeat()) {
                aIter = iList.begin();
            }
            else {
                return Track::Zero();
            }
        }
    }
    return *aIter;
}

const Track* ProviderPlaylist::IterateBackwards(list<Track*>::const_iterator aIter, uint32_t aCount)
{
    while( (aCount--) > 0) {
        if(--aIter == iList.begin()) {
            if(IsRepeat()) {
                aIter = iList.end();
            }
            else {
                return Track::Zero();
            }
        }
    }
    return *aIter;
}


