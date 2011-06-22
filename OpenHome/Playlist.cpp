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

PlaylistImpl::PlaylistImpl(Net::DvDevice& aDevice, TUint aTracksMax, const Brx& aProtocolInfo)
    : DvProviderAvOpenhomeOrgPlaylist1(aDevice)
    , iTransportState("Stopped")
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
    SetPropertyRepeat(false);
    SetPropertyShuffle(false);
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
    SetPropertyRepeat(aValue);
    aResponse.Start();
    aResponse.End();
}

void PlaylistImpl::Repeat(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseBool& aValue)
{
    TBool repeat;
    GetPropertyRepeat(repeat);
    aResponse.Start();
    aValue.Write(repeat);
    aResponse.End();
}

void PlaylistImpl::SetShuffle(Net::IInvocationResponse& aResponse, TUint aVersion, TBool aValue)
{
    SetPropertyShuffle(aValue);
    aResponse.Start();
    aResponse.End();
}

void PlaylistImpl::Shuffle(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseBool& aValue)
{
    TBool shuffle;
    GetPropertyShuffle(shuffle);
    aResponse.Start();
    aValue.Write(shuffle);
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
    Log::Print("PlaylistImpl::TransportState\n");
    Brhz transportState;
    GetPropertyTransportState(transportState);
    aResponse.Start();
    aValue.Write(transportState);
    aValue.WriteFlush();
    aResponse.End();
}

void PlaylistImpl::Id(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue)
{
    Log::Print("PlaylistImpl::Id\n");
    TUint id;
    GetPropertyId(id);
    aResponse.Start();
    aValue.Write(id);
    aResponse.End();
}

void PlaylistImpl::Read(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aId, Net::IInvocationResponseString& aUri, Net::IInvocationResponseString& aMetadata)
{
    Log::Print("PlaylistImpl::Read\n");
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
    Log::Print("PlaylistImpl::ReadList\n");
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
        iMutex.Signal();

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
    }

    aTrackList.Write(Brn("</TrackList>"));
    aTrackList.WriteFlush();
    aResponse.End();
}

void PlaylistImpl::Insert(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aAfterId, const Brx& aUri, const Brx& aMetadata, Net::IInvocationResponseUint& aNewId)
{
    Log::Print("PlaylistImpl::Insert\n");
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
    Log::Print("PlaylistImpl::DeleteId\n");
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
    Log::Print("PlaylistImpl::DeleteAll\n");
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
    Log::Print("PlaylistImpl::TracksMax\n");
    TUint tracksMax;
    GetPropertyTracksMax(tracksMax);
    aResponse.Start();
    aValue.Write(tracksMax);
    aResponse.End();
}

void PlaylistImpl::IdArray(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aToken, Net::IInvocationResponseBinary& aArray)
{
    Log::Print("PlaylistImpl::IdArray\n");
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
    Log::Print("PlaylistImpl::IdArrayChanged\n");
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


