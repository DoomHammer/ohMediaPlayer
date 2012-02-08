#include <OpenHome/Private/Debug.h>
#include <OpenHome/Media/Playlist.h>
#include <functional>
#include <algorithm>
#include <OpenHome/Private/Parser.h>
#include <OpenHome/Private/Ascii.h>
#include <OpenHome/Private/Converter.h>

using namespace OpenHome;
using namespace OpenHome::Media;
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

    EnablePropertyTransportState();
    EnablePropertyRepeat();
    EnablePropertyShuffle();
    EnablePropertyId();
    EnablePropertyTracksMax();
    EnablePropertyProtocolInfo();
    EnablePropertyIdArray();
    
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

const Track* ProviderPlaylist::GetTrack(TUint aId, TInt aOffset)
{
    const Track* track;
    list<Track*>::const_iterator i;

    iMutex.Wait();

    if(iList.empty()) {
        iMutex.Signal();
        return 0;
    }

    if(aId == 0) {
        i = iList.begin();        
    }
    else {
        i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),aId));
        if(i == iList.end()) {
            ASSERTS();
        }
    }
    if(aOffset > 0) {
        track = IterateForwards(i, aOffset);
    }
    else if(aOffset < 0) {
        track = IterateBackwards(i, abs(aOffset));
    }
    else {
        track = *i;
    }

    if(track) {
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

void ProviderPlaylist::Play(Net::IDvInvocation& aResponse)
{
    aResponse.StartResponse();
    aResponse.EndResponse();

    iSource.Play(0);
}

void ProviderPlaylist::Pause(Net::IDvInvocation& aResponse)
{
    aResponse.StartResponse();
    aResponse.EndResponse();
    
    iSource.Pause();
}

void ProviderPlaylist::Stop(Net::IDvInvocation& aResponse)
{
    aResponse.StartResponse();
    aResponse.EndResponse();

    iSource.Stop();
}

void ProviderPlaylist::Next(Net::IDvInvocation& aResponse)
{
    aResponse.StartResponse();
    aResponse.EndResponse();

    iSource.Play(1);
}

void ProviderPlaylist::Previous(Net::IDvInvocation& aResponse)
{
    aResponse.StartResponse();
    aResponse.EndResponse();

    iSource.Play(-1);
}

void ProviderPlaylist::SetRepeat(Net::IDvInvocation& aResponse, TBool aValue)
{
    SetPropertyRepeat(aValue);
    aResponse.StartResponse();
    aResponse.EndResponse();
}

void ProviderPlaylist::Repeat(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseBool& aValue)
{
    TBool repeat;
    GetPropertyRepeat(repeat);
    aResponse.StartResponse();
    aValue.Write(repeat);
    aResponse.EndResponse();
}

void ProviderPlaylist::SetShuffle(Net::IDvInvocation& aResponse, TBool aValue)
{
    SetPropertyShuffle(aValue);
    aResponse.StartResponse();
    aResponse.EndResponse();
}

void ProviderPlaylist::Shuffle(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseBool& aValue)
{
    TBool shuffle;
    GetPropertyShuffle(shuffle);
    aResponse.StartResponse();
    aValue.Write(shuffle);
    aResponse.EndResponse();
}

void ProviderPlaylist::SeekSecondAbsolute(Net::IDvInvocation& aResponse, TUint aValue)
{
    aResponse.StartResponse();
    aResponse.EndResponse();

    iSource.PlaySecondAbsolute(aValue);
}

void ProviderPlaylist::SeekSecondRelative(Net::IDvInvocation& aResponse, TInt aValue)
{
    aResponse.StartResponse();
    aResponse.EndResponse();

    iSource.PlaySecondRelative(aValue);
}

void ProviderPlaylist::SeekId(Net::IDvInvocation& aResponse, TUint aValue)
{
    aResponse.StartResponse();
    aResponse.EndResponse();

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

void ProviderPlaylist::SeekIndex(Net::IDvInvocation& aResponse, TUint aValue)
{
    aResponse.StartResponse();
    aResponse.EndResponse();

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

void ProviderPlaylist::TransportState(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseString& aValue)
{
    Log::Print("ProviderPlaylist::TransportState\n");
    Brhz transportState;
    GetPropertyTransportState(transportState);
    aResponse.StartResponse();
    aValue.Write(transportState);
    aValue.WriteFlush();
    aResponse.EndResponse();
}

void ProviderPlaylist::Id(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aValue)
{
    Log::Print("ProviderPlaylist::Id\n");
    TUint id;
    GetPropertyId(id);
    aResponse.StartResponse();
    aValue.Write(id);
    aResponse.EndResponse();
}

void ProviderPlaylist::Read(Net::IDvInvocation& aResponse, TUint aId, Net::IDvInvocationResponseString& aUri, Net::IDvInvocationResponseString& aMetadata)
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
        const Track* track = *i;
        track->IncRef();
        iMutex.Signal();

        aResponse.StartResponse();
        aUri.Write(track->Uri());
        aUri.WriteFlush();
        aMetadata.Write(track->Metadata());
        aMetadata.WriteFlush();
        aResponse.EndResponse();

        track->DecRef();
    }
}

void ProviderPlaylist::ReadList(Net::IDvInvocation& aResponse, const Brx& aIdList, Net::IDvInvocationResponseString& aTrackList)
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

    aResponse.StartResponse();
    aTrackList.Write(Brn("<TrackList>"));

    for( vector<uint32_t>::const_iterator id=v.begin(); id!= v.end(); id++) {

        const Track* track = 0;

        iMutex.Wait();
        list<Track*>::const_iterator i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),*id));
        if(i != iList.end()) {
            track = *i;
            track->IncRef();
        }   
        iMutex.Signal();

        if(track) {
            aTrackList.Write(entryStart);

            aTrackList.Write(idStart);
            Ascii::StreamWriteUint(aTrackList, *id);
            aTrackList.Write(idEnd);

            aTrackList.Write(uriStart);
            Converter::ToXmlEscaped(aTrackList, track->Uri());
            aTrackList.Write(uriEnd);

            aTrackList.Write(metaStart);
            Converter::ToXmlEscaped(aTrackList, track->Metadata());
            aTrackList.Write(metaEnd);

            aTrackList.Write(entryEnd);

            track->DecRef();
        }
    }

    aTrackList.Write(Brn("</TrackList>"));
    aTrackList.WriteFlush();
    aResponse.EndResponse();
}

void ProviderPlaylist::Insert(Net::IDvInvocation& aResponse, TUint aAfterId, const Brx& aUri, const Brx& aMetadata, Net::IDvInvocationResponseUint& aNewId)
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
    aResponse.StartResponse();
    aNewId.Write(id);
    aResponse.EndResponse();
    UpdateIdArray();

    iMutex.Signal();
}

void ProviderPlaylist::DeleteId(Net::IDvInvocation& aResponse, TUint aValue)
{
    Log::Print("ProviderPlaylist::DeleteId\n");

    bool deleted = false;

    iMutex.Wait();

    list<Track*>::iterator i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),aValue));

    const Track* replacement = 0;
    if(i != iList.end()) {
        replacement = IterateForwards(i, 1);
        if(replacement) {
            replacement->IncRef();
        }

        (*i)->DecRef();
        iList.erase(i);

        UpdateIdArray();
        deleted = true;
    }

    iMutex.Signal();

    aResponse.StartResponse();
    aResponse.EndResponse();

    if(deleted) {
        iSource.Deleted(aValue, replacement);
    }
}

void ProviderPlaylist::DeleteAll(Net::IDvInvocation& aResponse)
{
    Log::Print("ProviderPlaylist::DeleteAll\n");

    bool deleted = false;

    iMutex.Wait();

    if(iList.size() > 0) {
        list<Track*>::iterator i = iList.begin();
        for( ; i != iList.end(); ) {
            (*i)->DecRef(); 
            list<Track*>::iterator j = i;
            i++;
            iList.erase(j);
        }

        UpdateIdArray();
        deleted = true;
    }

    iMutex.Signal();

    aResponse.StartResponse();
    aResponse.EndResponse();

    if(deleted) {
        iSource.Deleted(0, 0); //Special value of 0 indicates entire playlist was deleted
    }
}

void ProviderPlaylist::TracksMax(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aValue)
{
    Log::Print("ProviderPlaylist::TracksMax\n");
    TUint tracksMax;
    GetPropertyTracksMax(tracksMax);
    aResponse.StartResponse();
    aValue.Write(tracksMax);
    aResponse.EndResponse();
}

void ProviderPlaylist::IdArray(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseUint& aToken, Net::IDvInvocationResponseBinary& aArray)
{
    Log::Print("ProviderPlaylist::IdArray\n");
    iMutex.Wait();

    aResponse.StartResponse();
    aToken.Write(iToken);
    aArray.Write(iIdArray);
    aArray.WriteFlush();
    aResponse.EndResponse();

    iMutex.Signal();
}

void ProviderPlaylist::IdArrayChanged(Net::IDvInvocation& aResponse, TUint aToken, Net::IDvInvocationResponseBool& aValue)
{
    Log::Print("ProviderPlaylist::IdArrayChanged\n");
    TBool changed = false;

    iMutex.Wait();
    if(aToken != iToken) {
        changed = true;
    }
    iMutex.Signal();

    aResponse.StartResponse();
    aValue.Write(changed);
    aResponse.EndResponse();
}

void ProviderPlaylist::ProtocolInfo(Net::IDvInvocation& aResponse, Net::IDvInvocationResponseString& aValue)
{
    Brhz protocolInfo;
    GetPropertyProtocolInfo(protocolInfo);

    aResponse.StartResponse();
    aValue.Write(protocolInfo);
    aValue.WriteFlush();
    aResponse.EndResponse();
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
                return 0;
            }
        }
    }
    return *aIter;
}

const Track* ProviderPlaylist::IterateBackwards(list<Track*>::const_iterator aIter, uint32_t aCount)
{
    while( (aCount--) > 0) {
        if(aIter == iList.begin()) {
            if(IsRepeat()) {
                aIter = iList.end();
            }
            else {
                return 0;
            }
        }
        --aIter;
    }
    return *aIter;
}


