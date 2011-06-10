#include "Playlist.h"

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;

Track::Track(const Brx& aUri, const Brx& aMetadata)
{
    iUri.Replace(aUri);
    iMetadata.Replace(aMetadata);
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

const Brn kProtocolInfo("http-get:*:audio/x-flac:*,http-get:*:audio/wav:*,http-get:*:audio/wave:*,http-get:*:audio/x-wav:*,http-get:*:audio/mpeg:*,http-get:*:audio/x-mpeg:*,http-get:*:audio/mp1:*,http-get:*:audio/aiff:*,http-get:*:audio/x-aiff:*,http-get:*:audio/x-m4a:*,http-get:*:audio/x-ms-wma:*,rtsp-rtp-udp:*:audio/x-ms-wma:*,http-get:*:audio/x-scpls:*,http-get:*:audio/x-mpegurl:*,http-get:*:audio/x-ms-asf:*,http-get:*:audio/x-ms-wax:*,http-get:*:audio/x-ms-wvx:*,http-get:*:video/x-ms-asf:*,http-get:*:video/x-ms-wax:*,http-get:*:video/x-ms-wvx:*,http-get:*:text/xml:*,http-get:*:audio/aac:*,http-get:*:audio/aacp:*,http-get:*:audio/mp4:*,http-get:*:audio/ogg:*,http-get:*:audio/x-ogg:*,http-get:*:application/ogg:*");

PlaylistImpl::PlaylistImpl(DvDevice &Device, TUint aTracksMax, const Brx& aProtocolInfo)
    : DvProviderAvOpenhomeOrgPlaylist1(aDevice)
    , iTransportState("Stopped")
    , iRepeat(false)
    , iShuffle(false)
    , iId(0)
    , iIdArray(sizeof(TUint)*aTracksMax)
    , kTracksMax(aTracksMax)
    , iProtocolInfo(aProtocolInfo.Bytes())
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

    UpdateIdArray();
    SetPropertyIdArray(iIdArray);

    SetPropertyTracksMax(kTracksMax);

    iProtocolInfo.Replace(aProtocolInfo);
    SetPropertyProtocolInfo(iProtocolInfo);
}

void PlaylistImpl::Play(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.Stop();
}

void PlaylistImpl::Pause(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.Stop();
}

void PlaylistImpl::Stop(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.Stop();
}

void PlaylistImpl::Next(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.Stop();
}

void PlaylistImpl::Previous(Net::IInvocationResponse& aResponse, TUint aVersion)
{
    aResponse.Start();
    aResponse.Stop();
}

void PlaylistImpl::SetRepeat(Net::IInvocationResponse& aResponse, TUint aVersion, TBool aValue)
{
    iRepeat = aValue;
    aResponse.Start();
    aResponse.Stop();
}

void PlaylistImpl::Repeat(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseBool& aValue)
{
    TBool value = iRepeat;
    aResponse.Start();
    aValue.Write(value);
    aValue.WriteFlush();
    aResponse.Stop();
}

void PlaylistImpl::SetShuffle(Net::IInvocationResponse& aResponse, TUint aVersion, TBool aValue)
{
    iShuffle = aValue;
    aResponse.Start();
    aResponse.Stop();
}

void PlaylistImpl::Shuffle(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseBool& aValue)
{
    TBool value = iShuffle;
    aResponse.Start();
    aValue.Write(value);
    aValue.WriteFlush();
    aResponse.Stop();
}

void PlaylistImpl::SeekSecondAbsolute(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    Log::Print("SeekSecondAbsolute: %d\n", aValue);
    aResponse.Start();
    aResponse.Stop();
}

void PlaylistImpl::SeekSecondRelative(Net::IInvocationResponse& aResponse, TUint aVersion, TInt aValue)
{
    Log::Print("SeekSecondRelative: %d\n", aValue);
    aResponse.Start();
    aResponse.Stop();
}

void PlaylistImpl::SeekId(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    Log::Print("SeekId: %d\n", aValue);
    aResponse.Start();
    aResponse.Stop();
}

void PlaylistImpl::SeekIndex(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
    Log::Print("SeekIndex: %d\n", aValue);
    aResponse.Start();
    aResponse.Stop();
}

void PlaylistImpl::TransportState(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aValue)
{
    Brhz transportState;
    GetPropertyTransportState(transportState);
    aResponse.Start();
    aValue.Write(transportState);
    aValue.WriteFlush();
    aResponse.Stop();
}

void PlaylistImpl::Id(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue)
{
    TUint id;
    GetPropertyId(id);
    aResponse.Start();
    aValue.Write(id);
    aValue.WriteFlush();
    aResponse.Stop();
}

void PlaylistImpl::Read(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aId, Net::IInvocationResponseString& aUri, Net::IInvocationResponseString& aMetadata)
{
    iMutex.Wait();


}

void PlaylistImpl::ReadList(Net::IInvocationResponse& aResponse, TUint aVersion, const Brx& aIdList, Net::IInvocationResponseString& aTrackList)
{
}

void PlaylistImpl::Insert(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aAfterId, const Brx& aUri, const Brx& aMetadata, Net::IInvocationResponseUint& aNewId)
{
}

void PlaylistImpl::DeleteId(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aValue)
{
}

void PlaylistImpl::DeleteAll(Net::IInvocationResponse& aResponse, TUint aVersion)o{
}

void PlaylistImpl::TracksMax(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aValue)
{
}

void PlaylistImpl::IdArray(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseUint& aToken, Net::IInvocationResponseBinary& aArray)
{
}

void PlaylistImpl::IdArrayChanged(Net::IInvocationResponse& aResponse, TUint aVersion, TUint aToken, Net::IInvocationResponseBool& aValue)
{
}

void PlaylistImpl::ProtocolInfo(Net::IInvocationResponse& aResponse, TUint aVersion, Net::IInvocationResponseString& aValue)
{
}

void PlaylistImpl::UpdateIdArray()
{
    for(list<Track>::const_iterator i = iList.begin(); i != iList.end(); ++i) {
        iIdArray.AppendUint32((*i).Id());
    }
}


