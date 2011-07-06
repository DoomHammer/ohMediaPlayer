#include "Player.h"
#include <Debug.h>
#include "Product.h"
#include "Playlist.h"

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;

// Track

Track::iZero = new Track(0, Brx::Empty(), Brx::Empty());

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

// Player

Player::Player(IRenderer* aRenderer
    , Net::DvDevice& aDevice
    , IStandbyHandler& aStandbyHandler
    , ISourceIndexHandler& aSourceIndexHandler
	, bool aStandby
	, const TChar* aAttributes
	, const TChar* aManufacturerName
	, const TChar* aManufacturerInfo
	, const TChar* aManufacturerUrl
	, const TChar* aManufacturerImageUri
	, const TChar* aModelName
	, const TChar* aModelInfo
	, const TChar* aModelUrl
	, const TChar* aModelImageUri
	, const TChar* aProductRoom
	, const TChar* aProductName
	, const TChar* aProductInfo
	, const TChar* aProductUrl
	, const TChar* aProductImageUri)
    : iRenderer(aRenderer)
    , iMutex("PLYR")
{
    aDevice.SetAttribute("Upnp.Domain", "av.openhome.org");
    aDevice.SetAttribute("Upnp.Type", "av.openhome.org");
    aDevice.SetAttribute("Upnp.Version", "1");

    Bwh tmp(strlen(aProductName) + strlen(aProductRoom) + 1);
    tmp.Append(aProductRoom);
    tmp.Append(':');
    tmp.Append(aProductName);
    Brhz friendlyName;
    tmp.TransferTo(friendlyName);
    aDevice.SetAttribute("Upnp.FriendlyName", friendlyName.CString());

    aDevice.SetAttribute("Upnp.Manufacturer", aManufacturerName);
    aDevice.SetAttribute("Upnp.ManufacturerUrl", aManufacturerUrl);
    aDevice.SetAttribute("Upnp.ModelDescription", aModelInfo);
    aDevice.SetAttribute("Upnp.ModelName", aModelName);
    aDevice.SetAttribute("Upnp.ModelNumber", "");
    aDevice.SetAttribute("Upnp.ModelUrl", aModelUrl);
    aDevice.SetAttribute("Upnp.SerialNumber", "");
    aDevice.SetAttribute("Upnp.Upc", "");

    iProduct = new ProviderProduct(aDevice,
        aStandbyHandler,
        aSourceIndexHandler,
        aStandby,
        aAttributes,
        aManufacturerName,
        aManufacturerInfo,
        aManufacturerUrl,
        aManufacturerImageUri,
        aModelName,
        aModelInfo,
        aModelUrl,
        aModelImageUri,
        aProductRoom,
        aProductName,
        aProductInfo,
        aProductUrl,
        aProductImageUri);

    iInfo = new ProviderInfo(aDevice);
    iTime = new ProviderTime(aDevice);

    iRenderer->SetStatusHandler(*this);
}

Player::~Player()
{
    delete iRenderer;
    delete iInfo;
}

uint32_t Player::AddSource(Source* aSource)
{
    uint32_t handle = iProduct->AddSource(aSource);
    aSource->SetHandle(handle);
    return handle;
}

Source& Player::GetSource(uint32_t aIndex)
{
    return iProduct->GetSource(aIndex);
}

void Player::Finished(uint32_t aHandle, uint32_t aId) 
{
    Log::Print("Player::Finished %d\n", aId); 
    GetSource(aHandle).Finished(aId);
}

void Player::Next(uint32_t aHandle, uint32_t aAfterId, uint32_t& aId, std::string& aUri)
{
    Log::Print("Player::Next\n");
    
    aUri.reserve(Track::kMaxUriBytes);
    Bwn uri(aUri.data(), aUri.capacity());

    GetSource(aHandle).Next(aAfterId, aId, uri);
}

void Player::Buffering(uint32_t aHandle, uint32_t aId)
{
    Log::Print("Player::Buffering\n");
    GetSource(aHandle).Buffering(aId);
}

void Player::Started(uint32_t aHandle, uint32_t aId, uint32_t aDuration, uint32_t aBitRate, uint32_t aBitDepth, uint32_t aSampleRate, bool aLossless, const char* aCodecName)
{
    Log::Print("Player::Started %d, Duration: %d, BitRate: %d, BitDepth: %d\n", aId, aDuration, aBitRate, aBitDepth);
    iInfo->SetDetails(aDuration, aBitRate, aBitDepth, aSampleRate, aLossless, Brn(aCodecName));
    iTime->SetDuration(aDuration);
}

void Player::Playing(uint32_t aHandle, uint32_t aId, uint32_t aSeconds)
{
    Log::Print("Player::Playing %d, Second: %d\n", aId, aSeconds);
    //TODO: Is it really necessary to call this _every_ second tick?
    GetSource(0).Playing(aId);
    iTime->SetSeconds(aSeconds);
}

void Player::Metatext(uint32_t aHandle, uint32_t aId, const std::string& aDidlLite)
{
    Log::Print("Player::Metatext %d\n", aId);
    iInfo->SetMetatext(Brn((const TByte*)(aDidlLite.data()), aDidlLite.size()));
}

void Player::Play(uint32_t aHandle, const Track* aTrack, uint32_t aSecond)  
{
    Log::Print("Player::Play %d\n", aTrack->Id());
    std::string uri(reinterpret_cast<const char*>(aTrack->Uri().Ptr()), aTrack->Uri().Bytes());
    iRenderer->Play(aHandle, aTrack->Id(), uri, aSecond);
    iInfo->SetTrack(aTrack->Uri(), aTrack->Metadata());
    aTrack->DecRef();
}

void Player::Play(uint32_t aHandle, int32_t aRelativeIndex)
{
    const Track* track = GetSource(aHandle).Next(iId, aRelativeIndex);
    std::string uri(reinterpret_cast<const char*>(aTrack->Uri().Ptr()), aTrack->Uri().Bytes());
    iRenderer->Play(aHandle, aTrack->Id(), uri, aSecond);
    iInfo->SetTrack(aTrack->Uri(), aTrack->Metadata());
    track->DecRef();
}

void Player::PlayAbsolute(uint32_t aHandle, uint32_t aSecond)
{
    const Track* track = GetSource(aHandle).Next(iId, 0);
    std::string uri(reinterpret_cast<const char*>(aTrack->Uri().Ptr()), aTrack->Uri().Bytes());
    iRenderer->Play(aHandle, aTrack->Id(), uri, aSecond);
    iInfo->SetTrack(aTrack->Uri(), aTrack->Metadata());
    track->DecRef();
}

void Player::PlayRelative(uint32_t aHanlde, int32_t aSecond)
{
    const Track* track = GetSource(aHandle).Next(iId, 0);
    std::string uri(reinterpret_cast<const char*>(aTrack->Uri().Ptr()), aTrack->Uri().Bytes());
    iRenderer->Play(aHandle, aTrack->Id(), uri, aSecond);
    iInfo->SetTrack(aTrack->Uri(), aTrack->Metadata());
    track->DecRef();
}

void Player::Pause()
{
    Log::Print("Player::Pause\n");
    iRenderer->Pause();
}

void Player::Unpause()
{
    Log::Print("Player::Unpause\n");
    iRenderer->Unpause();
}

void Player::Stop()
{
    Log::Print("Player::Stop\n");
    iRenderer->Stop();
}

void Player::Deleted(uint32_t aId, const Track* aReplacement)
{
    Log::Print("Player::Deleted: %d\n", aId);
    aReplacement->DecRef();
}

uint32_t Player::NewId() 
{
    return iAtomic.Inc();
}
