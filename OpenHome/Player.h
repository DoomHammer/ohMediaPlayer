#ifndef HEADER_OPENHOME_MEDIAPLAYER_PLAYER
#define HEADER_OPENHOME_MEDIAPLAYER_PLAYER

#include <OhNetTypes.h>
#include <Core/DvDevice.h>
#include <Buffer.h>
#include "Renderer.h"
#include "Standard.h"
#include "Source.h"

namespace OpenHome {
namespace MediaPlayer {

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

class IPlayer
{
public:
    virtual void Play(uint32_t aSourceId, const Brx& aUri, uint32_t aSecond, const Brx& aProvider) = 0;
    virtual void Pause() = 0;
    virtual void Unpause() = 0;
    virtual void Stop() = 0;
    virtual void Deleted(uint32_t aSourceId) = 0;
    virtual void DeletedAll() = 0;
    virtual ~IPlayer() {}
};

class ProviderProduct;
class ProviderTime;
class ProviderInfo;

class Player : public IRendererStatus, public IPlayer
{
//External Api
public:
    Player(Net::DvDevice& aDevice
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

    uint32_t AddSource(Source* aSource);
    Source& GetSource(uint32_t aIndex);

//Internal Api

    //from IRendererStatus
public:
    virtual void Finished(uint32_t aId);
    virtual void Next(uint32_t& aId, std::string& aUri, std::string& aProvider);
    virtual void Buffering(uint32_t aId);
    virtual void Started(uint32_t aId, uint32_t aDuration, uint32_t aBitRate, uint32_t aSampleRate, bool aLossless, std::string aCodecName);
    virtual void Playing(uint32_t aId, uint32_t aSeconds);
    virtual void Metatext(uint32_t aId, std::string aDidlLite);

    //from IPlayer
public:
    virtual void Play(uint32_t aSourceId, const Brx& aUri, uint32_t aSecond, const Brx& aProvider);
    virtual void Pause();
    virtual void Unpause();
    virtual void Stop();
    virtual void Deleted(uint32_t aSourceId);
    virtual void DeletedAll();

private:
    ProviderProduct* iProduct;
    //ProviderInfo* iInfo;
    //ProviderTime* iTime;
};


} // namespace MediaPlayer
} // namespace OpenHome

#endif // HEADER_OPENHOME_MEDIAPLAYER_PLAYER

