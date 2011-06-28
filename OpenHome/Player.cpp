#include "Player.h"
#include <Debug.h>
#include "Product.h"

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;

// Product

Player::Player(Net::DvDevice& aDevice
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
    : iId(0)
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

    //iInfo = new ProviderInfo();
    //iTime = new ProviderTime();
}

uint32_t Player::AddSource(Source* aSource)
{
    return iProduct->AddSource(aSource);
}

Source& Player::GetSource(uint32_t aIndex)
{
    return iProduct->GetSource(aIndex);
}

void Player::Finished(uint32_t aId) 
{
    Log::Print("Player::Finished %d\n", aId); 
}

void Player::Next(uint32_t& aId, std::string& aUri, std::string& aProvider)
{
    Log::Print("Player::Next\n");
}

void Player::Buffering(uint32_t aId)
{
    Log::Print("Player::Buffering\n");
}

void Player::Started(uint32_t aId, uint32_t aDuration, uint32_t aBitRate, uint32_t aSampleRate, bool aLossless, std::string aCodecName)
{
    Log::Print("Player::Started %d\n", aId);
}

void Player::Playing(uint32_t aId, uint32_t aSeconds)
{
    Log::Print("Player::Playing %d\n", aId);
}

void Player::Metatext(uint32_t aId, const std::string& aDidlLite)
{
    Log::Print("Player::Metatext %d\n", aId);
}

void Player::Play(uint32_t aId, const Brx& aUri, uint32_t aSecond, const Brx& aProvider)  
{
    Log::Print("Player::Play %d\n", aId);
}

void Player::Pause()
{
    Log::Print("Player::Pause\n");
}

void Player::Unpause()
{
    Log::Print("Player::Unpause\n");
}

void Player::Stop()
{
    Log::Print("Player::Stop\n");
}

void Player::Deleted(uint32_t aId)
{
    Log::Print("Player::Deleted: %d\n", aId);
}

void Player::DeletedAll() 
{
    Log::Print("Player::DeletedAll\n");
}

uint32_t Player::NewId() 
{
    uint32_t id;
    iMutex.Wait();
    id = ++iId;
    iMutex.Signal();
    return id;
}
