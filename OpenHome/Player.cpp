#include "Player.h"
#include <Debug.h>
#include "Product.h"
#include "Playlist.h"

#include <algorithm>
#include <functional>

using namespace OpenHome;
using namespace OpenHome::MediaPlayer;

// Track

Track::Track(uint32_t aId, const Brx& aUri, const Brx& aMetadata)
    : iId(aId)
{
    iUri.Replace(aUri);
    iMetadata.Replace(aMetadata);
}

bool Track::IsId(TUint aId) const
{
    return (aId == iId);
}

uint32_t Track::Id() const
{
    return iId;
}

void Track::Uri(const uint8_t*& aUri, uint32_t& aBytes) const
{
    aUri = (const uint8_t*)(iUri.Ptr());
    aBytes = iUri.Bytes();
}

void Track::Metadata(const uint8_t*& aMetadata, uint32_t& aBytes) const
{
    aMetadata = (const uint8_t*)(iMetadata.Ptr());
    aBytes = iMetadata.Bytes();
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
{
    aDevice.SetAttribute("Upnp.Domain", "av.openhome.org");
    aDevice.SetAttribute("Upnp.Type", "av.openhome.org");
    aDevice.SetAttribute("Upnp.Version", "1");

    Bwh tmp(strlen(aProductName) + strlen(aProductRoom) + 2);
    tmp.Append(aProductRoom);
    tmp.Append(':');
    tmp.Append(aProductName);
    tmp.Append('\0');
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
    iVolume = new ProviderVolume(aDevice, *this);

    iRenderer->SetStatusHandler(*this);
}

Player::~Player()
{
    delete iRenderer;
    delete iInfo;
    delete iTime;
    delete iVolume;
    delete iProduct;
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
    if(next) {
        PlayLocked(aHandle, next, 0);
    }
    else {
        PipelineClear();
        StopLocked(aHandle);
    }
    iMutex.Signal();
}

const ITrack* Player::Next(uint32_t aHandle, uint32_t aAfterId)
{
    iMutex.Wait();

    Log::Print("Player::Next\n");
    
    const Track* track = GetSource(aHandle).GetTrack(aAfterId, 1);
    if(track) {
        //track reference already incremented by GetSource.  This reference is
        //"owned" by PipelineAppend
        PipelineAppend(track);

        //But increment it again for return to Renderer
        track->IncRef();
    }
    iMutex.Signal();

    return track;
}

void Player::Buffering(uint32_t aHandle, uint32_t aId)
{
    iMutex.Wait();

    Log::Print("Player::Buffering\n");
    GetSource(aHandle).Buffering(aId);
    iState = eBuffering;

    iMutex.Signal();
}

void Player::Playing(uint32_t aHandle, uint32_t aId, uint32_t aDuration, uint32_t aBitRate, uint32_t aBitDepth, uint32_t aSampleRate, bool aLossless, const char* aCodecName)
{
    iMutex.Wait();

    const Track* track = iPipeline.front();
    ASSERT(track->Id() == aId);

    Log::Print("Player::Started %d, Duration: %d, BitRate: %d, BitDepth: %d\n", aId, aDuration, aBitRate, aBitDepth);
    iInfo->SetTrack(*track);
    iInfo->SetDetails(aDuration, aBitRate, aBitDepth, aSampleRate, aLossless, Brn(aCodecName));
    iTime->SetDuration(aDuration);

    iState = ePlaying;
    GetSource(aHandle).Playing(aId);

    iMutex.Signal();
}

void Player::Time(uint32_t aHandle, uint32_t aId, uint32_t aSeconds) 
{
    Log::Print("Player::Time %d, Second: %d\n", aId, aSeconds);
    iTime->SetSeconds(aSeconds);
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
        iState = ePlaying;
        ASSERT(!iPipeline.empty());
        GetSource(aHandle).Playing(iPipeline.front()->Id());
        iRenderer->Unpause();

        iMutex.Signal();
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

    if(track) {
        PlayLocked(aHandle, track, 0);
    }
    //If after all that, the track returned from GetTrack is 0 (ie off the end
    //of the playlist), then we Stop the renderer
    else {
        PipelineClear();
        StopLocked(aHandle);
    }
    iMutex.Signal();
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

    if(track) {
        PlayLocked(aHandle, track, aSecond);
    }

    iMutex.Signal();
}

void Player::PlaySecondRelative(uint32_t aHandle, int32_t aSecond)
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
    TUint current = iTime->Seconds();
    TUint duration = iTime->Duration();

    TUint request;
    if(aSecond < 0) {
        if( (current + aSecond) > current) {
            //overflow -> seeked backwards past 0, set to 0
            request = 0;
        }
        else {
            request = current + aSecond;
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

void Player::Pause(uint32_t aHandle)
{
    iMutex.Wait();
    Log::Print("Player::Pause\n");

    if(iState == ePlaying) {
        GetSource(aHandle).Paused(iPipeline.front()->Id());
        iState = ePaused;
        iRenderer->Pause();
    }

    iMutex.Signal();
}

void Player::Stop(uint32_t aHandle)
{
    iMutex.Wait();
    Log::Print("Player::Stop\n");

    StopLocked(aHandle);
        
    iMutex.Signal();
}

void Player::Deleted(uint32_t aHandle, uint32_t aId, const Track* aReplacement)
{
    iMutex.Wait();

    Log::Print("Player::Deleted: %d\n", aId);

    if(iPipeline.empty()) {
        //Nothing in pipeline, so deletion doesn't affect anything
        iMutex.Signal();
        if(aReplacement) {
            aReplacement->DecRef();
        }
        return;
    }

    if(aId == 0) {
        //Entire playlist deleted, no matter what state clear pipeline and stop
        PipelineClear();
        StopLocked(aHandle);

        iMutex.Signal();
        if(aReplacement) {
            aReplacement->DecRef();
        }
        return;
    }

    if(aId == iPipeline.front()->Id()) {
        //Deleted id is current one.
        if(aReplacement) {
            if(iState == ePlaying) {
                PlayLocked(aHandle, aReplacement, 0);
            }
            else {
                //replacement available, but we're not to play.
                PipelineClear();
                PipelineAppend(aReplacement);
                StopLocked(aHandle);
            }
        }
        else {
            //No replacement, clear pipeline and stop
            PipelineClear();
            StopLocked(aHandle);
        }
    }
    else if(iPipeline.size() > 1) {
        //Check if deleted id is in pipeline
        std::list<const Track*>::iterator i = iPipeline.begin();
        i++;
        i = std::find_if(i, iPipeline.end(), std::bind2nd(std::mem_fun(&Track::IsId),aId));

        if(i != iPipeline.end()) {
            Log::Print("Player::Deleted id: %d, found in Pipeline, finishing id: %d first\n", aId, iPipeline.front()->Id());
            iRenderer->FinishAfter(iPipeline.front()->Id());
        }
        if(aReplacement) {
            aReplacement->DecRef();
        }
    }

    iMutex.Signal();
}

uint32_t Player::NewId() 
{
    return iAtomicInt.Inc();
}

void Player::SetVolume(uint32_t aValue)
{
    iMutex.Wait();
    iRenderer->SetVolume(aValue);
    iMutex.Signal();
}

void Player::SetMute(bool aValue)
{
    iMutex.Wait();
    iRenderer->SetMute(aValue);
    iMutex.Signal();
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

    GetSource(aHandle).Buffering(aTrack->Id());
    iState = eBuffering;

    iRenderer->Play(aHandle, *aTrack, aSecond);

    //Preadvance time to target, means UI will update quickly and all future
    //seeks will happen relative to here.
    iTime->SetSeconds(aSecond);

}

void Player::StopLocked(uint32_t aHandle) 
{
    if(iState != eStopped) {
        uint32_t id = 0;
        if(!iPipeline.empty()) {
            id = iPipeline.front()->Id(); 
        }
        GetSource(aHandle).Stopped(id);
        iState = eStopped;
        iRenderer->Stop();
    }
}
