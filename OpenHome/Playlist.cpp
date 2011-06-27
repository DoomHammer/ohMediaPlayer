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
static const Brn kInvalidRequestMsg("Space separated id request list invalid");

static const Brn kPlaying("Playing");
static const Brn kPaused("Paused");
static const Brn kStopped("Stopped");
static const Brn kBuffering("Buffering");

const Brn ProviderPlaylist::kProvider("av.openhome.org/Providers/Playlist");

ProviderPlaylist::ProviderPlaylist(Net::DvDevice& aDevice, TUint aTracksMax, const Brx& aProtocolInfo, IPlayer& aPlayer)
    : DvProviderAvOpenhomeOrgPlaylist1(aDevice)
    , iIdCounter(1)
    , iToken(0)
    , iIdArray(sizeof(TUint)*aTracksMax)
    , iMutex("Play")
    , iPlayer(aPlayer)
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

void ProviderPlaylist::Next(TUint aAfterId, TUint& aId, Bwx& aUri)
{
    iMutex.Wait();

    list<Track*>::const_iterator i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),aId));

    if(i == iList.end()) {
        aId = 0;
        aUri.Replace(Brx::Empty());
    }
    else {
        aId = (*i)->Id();
        aUri.Replace((*i)->Uri());
    }

    iMutex.Signal();
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

    iMutex.Wait();

    switch(iState) {
        case ePlaying:
        case eStopped:
        case eBuffering: 
            {
                if(iList.empty() == false) {
                    list<Track*>::const_iterator i;
                    TUint id;
                    GetPropertyId(id);
                    if(id == 0) {
                        i = iList.begin();
                    }
                    else {
                        i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),id));
                        if(i == iList.end()) {
                            ASSERTS();
                        }
                    }
                    iPlayer.Play((*i)->Id(), (*i)->Uri(), 0, kProvider);
                }
            }
            break;
        case ePaused:
            iPlayer.Unpause();
            break;
        default:    
            ASSERTS();
    }

    iMutex.Signal();
}

void ProviderPlaylist::Pause(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();

    iMutex.Wait();

    switch(iState) {
        case ePlaying:
            iPlayer.Pause();
            break;
        case eStopped:
        case eBuffering:
        case ePaused:
            break;
        default:    
            ASSERTS();
    }

    iMutex.Signal();
}

void ProviderPlaylist::Stop(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();

    iMutex.Wait();

    switch(iState) {
        case ePlaying:
        case eBuffering:
        case ePaused:
            iPlayer.Stop();
            break;
        case eStopped:
            break;
        default:    
            ASSERTS();
    }

    iMutex.Signal();
}

void ProviderPlaylist::Next(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();

    iMutex.Wait();

    if(iList.empty()) {
        iMutex.Signal();
        return;
    }

    switch(iState) {
        case ePlaying:
        case eBuffering:
        case ePaused:
        case eStopped:
            {
                //1) There's something in the list

                TUint id;
                //2) Get the current id
                GetPropertyId(id);

                list<Track*>::const_iterator i;

                //3) If that id is 0, we play from beginning
                if(id == 0) {
                    i = iList.begin();    
                }
                //4) If it's not 0, we find the id and play the following track, if it exists
                else {
                    i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),id));
                    if(i == iList.end()) {
                        //Id doesn't exist in list, something wacky
                        ASSERTS();
                    }
                    ++i;
                    if(i == iList.end()) {
                        TBool repeat;
                        GetPropertyRepeat(repeat);
                        if(repeat) {
                            i = iList.begin();
                        }
                        else {
                            //TODO: Pre seek to first track?
                            iPlayer.Stop();
                            break;
                        }
                    }
                }
                iPlayer.Play((*i)->Id(), (*i)->Uri(), 0, kProvider);
            }
            break;
        default:    
            ASSERTS();
    }

    iMutex.Signal();
}

void ProviderPlaylist::Previous(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.End();

    iMutex.Wait();

    if(iList.empty()) {
        iMutex.Signal();
        return;
    }

    switch(iState) {
        case ePlaying:
        case eBuffering:
        case ePaused:
        case eStopped:
            {
                //1) There's something in the list

                TUint id;
                //2) Get the current id
                GetPropertyId(id);

                list<Track*>::const_iterator i;

                //3) If that id is 0, we play from end
                if(id == 0) {
                    TBool repeat;
                    GetPropertyRepeat(repeat);
                    if(repeat) {
                        i = iList.end();
                        --i;
                    }
                    else {
                        iPlayer.Stop();
                        break;
                    }
                }
                //4) If it's not 0, we find the id and play the previous track, if it exists
                else {
                    i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),id));
                    if(i == iList.end()) {
                        //Id doesn't exist in list, something wacky
                        ASSERTS();
                    }
                    if(i == iList.begin()) {
                        TBool repeat;
                        GetPropertyRepeat(repeat);
                        if(repeat) {
                            i = iList.end();
                        }
                        else {
                            //TODO: Pre seek to first track?
                            iPlayer.Stop();
                            break;
                        }
                    }
                }
                iPlayer.Play((*i)->Id(), (*i)->Uri(), 0, kProvider);
            }
            break;
        default:    
            ASSERTS();
    }

    iMutex.Signal();
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

    iMutex.Wait();

    if(iList.empty() == false) {
        list<Track*>::const_iterator i;
        TUint id;
        GetPropertyId(id);
        if(id == 0) {
            i = iList.begin();
        }
        else {
            i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),id));
            if(i == iList.end()) {
                ASSERTS();
            }
        }
        iPlayer.Play((*i)->Id(), (*i)->Uri(), aValue, kProvider);
    }

    iMutex.Signal();
}

void ProviderPlaylist::SeekSecondRelative(Net::IInvocationResponse& aResponse, TUint aVersion, TInt aValue)
{
    aResponse.Start();
    aResponse.End();

    iMutex.Wait();

    if(iList.empty() == false) {
        list<Track*>::const_iterator i;
        TUint id;
        GetPropertyId(id);
        if(id == 0) {
            i = iList.begin();
        }
        else {
            i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),id));
            if(i == iList.end()) {
                ASSERTS();
            }
        }
        //TODO: iPlayer.PlayRelative required
        iPlayer.Play((*i)->Id(), (*i)->Uri(), aValue, kProvider);
    }

    iMutex.Signal();
}

void ProviderPlaylist::SeekId(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    aResponse.Start();
    aResponse.End();

    iMutex.Wait();

    list<Track*>::const_iterator i;
    i = find_if(iList.begin(), iList.end(), bind2nd(mem_fun(&Track::IsId),aValue));
    if(i != iList.end()) {
        iPlayer.Play((*i)->Id(), (*i)->Uri(), 0, kProvider);
    }

    //Not found, do nothing

    iMutex.Signal();
}

void ProviderPlaylist::SeekIndex(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    aResponse.Start();
    aResponse.End();

    iMutex.Wait();

    list<Track*>::const_iterator i = iList.begin();

    bool hitEnd = false;
    for(TUint j=0; j < aValue; j++, i++) {
        if(i == iList.end()) {
            hitEnd = true;
            break;
        }
    }

    if(hitEnd == false) {
        iPlayer.Play((*i)->Id(), (*i)->Uri(), 0, kProvider);
    }
    
    iMutex.Signal();
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

    iList.insert(i, new Track(iIdCounter, aUri, aMetadata));
    aResponse.Start();
    aNewId.Write(iIdCounter++);
    aResponse.End();
    UpdateIdArray();

    iMutex.Signal();
}

void ProviderPlaylist::DeleteId(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    Log::Print("ProviderPlaylist::DeleteId\n");
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

void ProviderPlaylist::DeleteAll(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    Log::Print("ProviderPlaylist::DeleteAll\n");
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


