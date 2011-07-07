#include "Player.h"
#include <Debug.h>
#include "Product.h"
#include "Playlist.h"

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;

// Track

const Track* Track::iZero = new Track(0, Brn(""), Brn(""));

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
    , iState(eStopped)
    , iId(0)
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
    iMutex.Wait();

    Log::Print("Player::Finished %d\n", aId); 

    const Track* finished = iPipeline.front();
    ASSERT(finished->Id() == aId);

    const Track* next = GetSource(aHandle).GetTrack(aId, 1);
    PipelineClear();
    if(next->Id() != 0) {
        PipelineAppend(next);
        
        iRenderer->Play(aHandle, next->Id(), next->Uri().Ptr(), next->Uri().Bytes(), 0);
    }
    else {
        next->DecRef();
    }

    iMutex.Signal();
}

void Player::Next(uint32_t aHandle, uint32_t aAfterId, uint32_t& aId, uint8_t aUri[], uint32_t& aUriBytes)
{
    iMutex.Wait();

    Log::Print("Player::Next\n");
    
    const Track* track = GetSource(aHandle).GetTrack(aAfterId, 1);
    aId = track->Id();
    ASSERT(aUriBytes >= Track::kMaxUriBytes);
    memcpy(aUri, track->Uri().Ptr(), track->Uri().Bytes());
    aUriBytes = track->Uri().Bytes();

    PipelineAppend(track);

    iMutex.Signal();
}

void Player::Buffering(uint32_t aHandle, uint32_t aId)
{
    iMutex.Wait();

    Log::Print("Player::Buffering\n");
    GetSource(aHandle).Buffering(aId);
    iState = eBuffering;

    iMutex.Signal();
}

void Player::Stopped(uint32_t aHandle, uint32_t aId)
{
    iMutex.Wait();

    Log::Print("Player::Stopped\n");
    GetSource(aHandle).Stopped(aId);
    iState = eStopped;

    iMutex.Signal();
}

void Player::Paused(uint32_t aHandle, uint32_t aId)
{
    iMutex.Wait();

    Log::Print("Player::Paused\n");
    GetSource(aHandle).Paused(aId);
    iState = ePaused;

    iMutex.Signal();
}

void Player::Started(uint32_t aHandle, uint32_t aId, uint32_t aDuration, uint32_t aBitRate, uint32_t aBitDepth, uint32_t aSampleRate, bool aLossless, const char* aCodecName)
{
    iMutex.Wait();

    const Track* track = iPipeline.front();
    ASSERT(track->Id() == aId);

    Log::Print("Player::Started %d, Duration: %d, BitRate: %d, BitDepth: %d\n", aId, aDuration, aBitRate, aBitDepth);
    iInfo->SetTrack(*track);
    iInfo->SetDetails(aDuration, aBitRate, aBitDepth, aSampleRate, aLossless, Brn(aCodecName));
    iTime->SetDuration(aDuration);

    iMutex.Signal();
}

void Player::Playing(uint32_t aHandle, uint32_t aId, uint32_t aSeconds)
{
    iMutex.Wait();

    Log::Print("Player::Playing %d, Second: %d\n", aId, aSeconds);
    iTime->SetSeconds(aSeconds);

    //iId is subtly different from iPipeline.front()->Id()
    //iId is updated when we get a ::Playing back from the renderer
    //iPipeline.front()->Id() is updated when we call iRenderer->Play()
    if( (iId != aId) || (iState != ePlaying) ) {
        iState = ePlaying;
        iId = aId;
        GetSource(aHandle).Playing(aId);
    }

    iMutex.Signal();
}

void Player::Metatext(uint32_t aHandle, uint32_t aId, uint8_t aMetatext[], uint32_t aMetatextBytes)
{
    Log::Print("Player::Metatext %d\n", aId);
    iInfo->SetMetatext(Brn(aMetatext, aMetatextBytes));
}

void Player::Play(uint32_t aHandle, const Track* aTrack, uint32_t aSecond)  
{
    iMutex.Wait();

    PlayLocked(aHandle, aTrack, aSecond);

    iMutex.Signal();
}

void Player::Play(uint32_t aHandle, int32_t aRelativeIndex)
{
    iMutex.Wait();

    Log::Print("Player::Play with RelativeIndex: %d\n", aRelativeIndex);

    //If we're paused and no skip is requested, then this command is an Unpause
    if(aRelativeIndex == 0 && iState == ePaused) {
        iMutex.Signal();
        Unpause();
        return;
    }

    //Else, 2 options:
    uint32_t id;
    if(iPipeline.empty()) {
        //Nothing in pipeline -> interpret aRelativeIndex from id 0
        id = 0;
    } 
    else {
        //Something in pipeline -> interpret aRelativeIndex from that id
        id = (iPipeline.front())->Id();
    }
    const Track* track = GetSource(aHandle).GetTrack(id, aRelativeIndex);

    //If after all that, the track returned from GetTrack is 0 (ie off the end
    //of the playlist), then we Stop the renderer
    if(track->Id() == 0) {
        iMutex.Signal();

        track->DecRef();
        iRenderer->Stop();
    }
    else {
        PlayLocked(aHandle, track, 0);
        iMutex.Signal();
    }
}

void Player::PlaySecondAbsolute(uint32_t aHandle, uint32_t aSecond)
{
    const Track* track;

    iMutex.Wait();

    if(iPipeline.empty()) {
        track = GetSource(aHandle).GetTrack(0, 0);
    }
    else {
        track = iPipeline.front();
        track->IncRef();
    }

    PlayLocked(aHandle, track, aSecond);

    iMutex.Signal();
}

void Player::PlaySecondRelative(uint32_t aHandle, int32_t aSecond)
{
    iMutex.Wait();

    if(iPipeline.empty()) {
        iMutex.Signal();
        return;
    }

    const Track* track = iPipeline.front();
    track->IncRef();
    TUint current = iTime->Seconds();
    TUint duration = iTime->Duration();

    TUint request;
    if(aSecond < 0) {
        if( (current + aSecond) > current) {
            //overflow -> seeked backwards past 0, set to 0
            request = 0;
        }
    }
    else {
        request = current + aSecond; 
    }

    if(request > duration) {
        request = duration;
    }

    PlayLocked(aHandle, track, request);

    iMutex.Signal();
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
    return iAtomicInt.Inc();
}

void Player::PipelineClear()
{
    if(iPipeline.size() > 0) {
        std::list<const Track*>::iterator i = iPipeline.begin();
        for( ; i != iPipeline.end(); ) {
            (*i)->DecRef();
            std::list<const Track*>::iterator j = i;
            i++;
            iPipeline.erase(j);
        }
    }
}

void Player::PipelineAppend(const Track* aTrack)
{
    iPipeline.push_back(aTrack);
}

void Player::PlayLocked(uint32_t aHandle, const Track* aTrack, uint32_t aSecond)
{
    Log::Print("Player::PlayLocked %d, Second: %d\n", aTrack->Id(), aSecond);

    PipelineClear();
    PipelineAppend(aTrack);

    iRenderer->Play(aHandle, aTrack->Id(), aTrack->Uri().Ptr(), aTrack->Uri().Bytes(), aSecond);
}
