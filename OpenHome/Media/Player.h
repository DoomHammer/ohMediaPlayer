#ifndef HEADER_OPENHOME_MEDIA_PLAYER
#define HEADER_OPENHOME_MEDIA_PLAYER

#include <OhNetTypes.h>
#include <Core/DvDevice.h>
#include <Thread.h>
#include <Buffer.h>
#include <RefCounter.h>
#include "Renderer.h"
#include "Standard.h"
#include "Info.h"
#include "Time.h"
#include "Volume.h"

#include <list>

namespace OpenHome {
namespace Media {

class Source;

class IStandbyHandler
{
public:
    virtual void SetStandby(TBool aValue) = 0;
    virtual ~IStandbyHandler() {}
};

class ISourceIndexHandler
{
public:
    virtual void SetSourceIndex(TUint aValue) = 0;
    virtual ~ISourceIndexHandler() {}
};

class ITrack 
{
public:
    virtual uint32_t Id() const = 0;
    virtual void Uri(const uint8_t*& aUri, uint32_t& aBytes) const = 0;
    virtual void Metadata(const uint8_t*& aMetadata, uint32_t& aBytes) const = 0;
};

class Track : public RefCounter, public ITrack
{
public:
    static const TUint kMaxUriBytes = 1024;
    static const TUint kMaxMetadataBytes = 5 * 1024;
public:
    Track(TUint aId, const Brx& aUri, const Brx& aMetadata);
    bool IsId(TUint aId) const;
    virtual uint32_t Id() const;
    virtual void Uri(const uint8_t*& aUri, uint32_t& aBytes) const;
    virtual void Metadata(const uint8_t*& aMetadata, uint32_t& aBytes) const;
    const Brx& Uri() const;
    const Brx& Metadata() const;

private:
    TUint iId;
    Bws<kMaxUriBytes> iUri;
    Bws<kMaxMetadataBytes> iMetadata;
};

class IPlayer
{
public:
    virtual void Play(uint32_t aHandle, const Track* aTrack, uint32_t aSecond) = 0;
    virtual void Play(uint32_t aHandle, int32_t aRelativeIndex) = 0;
    virtual void PlaySecondAbsolute(uint32_t aHandle, uint32_t aSecond) = 0;
    virtual void PlaySecondRelative(uint32_t aHanlde, int32_t aSecond) = 0;
    virtual void Pause(uint32_t aHandle) = 0;
    virtual void Stop(uint32_t aHandle) = 0;
    virtual void Deleted(uint32_t aHandle, uint32_t aId, const Track* aReplacement) = 0;
    virtual uint32_t NewId() = 0;
    virtual ~IPlayer() {}
};

class IVolume
{
public:
    virtual void SetVolume(uint32_t aValue) = 0;
    virtual void SetMute(bool aValue) = 0;
    virtual ~IVolume() {}
};

class ProviderProduct;
class ProviderTime;
class ProviderInfo;

class Player : public IRendererStatus, public IPlayer, public IVolume
{
//External Api
public:
    Player(IRenderer* aRenderer
        , Net::DvDevice& aDevice
        , IStandbyHandler& aStandbyHandler
        , ISourceIndexHandler& aSourceIndexHandler
        , bool aStandby
        , const char* aAtrributes
        , const char* aManufacturerName
        , const char* aManufacturerInfo
        , const char* aManufacturerUrl
        , const char* aManufacturerImageUri
        , const char* aModelName
        , const char* aModelInfo
        , const char* aModelUrl
        , const char* aModelImageUri
        , const char* aProductRoom
        , const char* aProductName
        , const char* aProductInfo
        , const char* aProductUrl
        , const char* aProductImageUri); 
    ~Player();

    uint32_t AddSource(Source* aSource);
    Source& GetSource(uint32_t aIndex);

//Internal Api

    //from IRendererStatus, to be called by IRenderer implementations
public:
    virtual void Finished(uint32_t aHandle, uint32_t aId);
    virtual const ITrack* Next(uint32_t aHandle, uint32_t aAfterId);
    virtual void Buffering(uint32_t aHandle, uint32_t aId);
    virtual void Playing(uint32_t aHandle, uint32_t aId, uint32_t aDuration, uint32_t aBitRate, uint32_t aBitDepth, uint32_t aSampleRate, bool aLossless, const char* aCodecName);
    virtual void Time(uint32_t aHandle, uint32_t aId, uint32_t aSeconds);
    virtual void Metatext(uint32_t aHandle, uint32_t aId, uint8_t aMetatext[], uint32_t aMetatextBytes);

    //from IPlayer, to be called from Source implementations
public:
    virtual void Play(uint32_t aHandle, const Track* aTrack, uint32_t aSecond);
    virtual void Play(uint32_t aHandle, int32_t aRelativeIndex);
    virtual void PlaySecondAbsolute(uint32_t aHandle, uint32_t aSecond);
    virtual void PlaySecondRelative(uint32_t aHanlde, int32_t aSecond);
    virtual void Pause(uint32_t aHandle);
    virtual void Stop(uint32_t aHandle);
    virtual void Deleted(uint32_t aHandle, uint32_t aId, const Track* aReplacement);
    virtual uint32_t NewId();

    //from IVolume, to be called from Provider Volume
public:
    virtual void SetVolume(uint32_t aValue);
    virtual void SetMute(bool aValue);

private:
    void PipelineClear();
    void PipelineAppend(const Track* aTrack);
    void PlayLocked(uint32_t aHandle, const Track* aTrack, uint32_t aSecond);
    void StopLocked(uint32_t aHandle);

private:
    ProviderProduct* iProduct;
    ProviderInfo* iInfo;
    ProviderTime* iTime;
    ProviderVolume* iVolume;
    IRenderer* iRenderer;

    AtomicInt iAtomicInt;

    enum ETransportState
    {
        ePlaying = 0,
        ePaused = 1,
        eStopped = 2,
        eBuffering = 3
    };

    Mutex iMutex;
    std::list<const Track*> iPipeline;
    ETransportState iState;
};


} // namespace Media
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIA_PLAYER

